#include <stdio.h>
#include <webkit2/webkit2.h>
#include <sys/stat.h>

#include "zhtml2pdf.h"

static int nonthreaded_init = 0;

static GThread *loop_thread = 0;
static GMainLoop *loop = 0;


struct ZHTML2PDF_CONTEXT {
    GtkPrintSettings *settings;
    WebKitPrintOperation *operation;
    GMainLoop *loop;
};

struct html2pdf_params {
    const char *input;
    const char *output;
    const char *settings;
    const char *css;

    GCond *wait_cond;
    GMutex *wait_mutex;
    int *wait_data;
};

#define CONTEXT ((struct ZHTML2PDF_CONTEXT*)user_data)

static int _html2pdf(struct html2pdf_params* params);
static void* _event_loop_function(void* data);
static int _read_file_contents(const char* filename, unsigned char** output);
void _encode_base64(const unsigned char* input, size_t length, unsigned char* output);


void init_zhtml2pdf() {
    gtk_init_check(NULL, NULL);
    nonthreaded_init = 1;
}

void deinit_zhtml2pdf() {
    gtk_init_check(NULL, NULL);
}

void init_loop_zhtml2pdf() {
    if (loop_thread != NULL) return;
    struct timespec ts;
    ts.tv_sec = 0;
    ts.tv_nsec = 1000000;

    loop_thread = g_thread_new("zhtml2pdf_event_loop", _event_loop_function, NULL);

    while (loop == NULL) nanosleep(&ts, NULL);

    while (!g_main_loop_is_running(loop)) nanosleep(&ts, NULL);
}

void deinit_loop_zhtml2pdf() {
    g_main_loop_quit(loop);
    g_thread_join(loop_thread);
    g_main_loop_unref(loop);
}

void zhtml2pdf_free(void** buffer) {
    if (buffer != NULL || *buffer != NULL) {
        free(*buffer);
        *buffer = NULL;
    }
}


int zhtml2pdf(const char* input, const char* settings, const char* css, unsigned char** output) {
    struct html2pdf_params *params = malloc(sizeof(struct html2pdf_params));
    if (params == NULL) return -1;

    params->input = input;
    params->output = "file:///tmp/zhtml2pdf_tempfile";
    params->settings = settings;
    params->css = css;

    params->wait_mutex = NULL;
    params->wait_cond = NULL;
    params->wait_data = NULL;

    if (!nonthreaded_init && g_main_context_default() != NULL) {
        GMutex wait_mutex;
        GCond wait_cond;
        int wait_data = 0;

        g_mutex_init(&wait_mutex);
        g_cond_init(&wait_cond);

        params->wait_cond = &wait_cond;
        params->wait_mutex = &wait_mutex;
        params->wait_data = &wait_data;

        g_idle_add((GSourceFunc)_html2pdf, params);

        g_mutex_lock(&wait_mutex);
        while (wait_data == 0) {
            g_cond_wait(&wait_cond, &wait_mutex);
        }

        g_mutex_unlock(&wait_mutex);

        g_mutex_clear(&wait_mutex);
        g_cond_clear(&wait_cond);
        free(params);

        return -2;
    }

    _html2pdf(params);
    free(params);

    return _read_file_contents("/tmp/zhtml2pdf_tempfile", output);
}

static void* _event_loop_function(void* data) {
    gtk_init_check (NULL,  NULL);
    loop = g_main_loop_new(NULL,false);

    g_main_loop_run(loop);
}

static void _web_view_load_changed(WebKitWebView* web_view, WebKitLoadEvent load_event, void* user_data) {
    switch (load_event) {
        case WEBKIT_LOAD_STARTED:
            break;
        case WEBKIT_LOAD_REDIRECTED:
            break;
        case WEBKIT_LOAD_COMMITTED:
            break;
        case WEBKIT_LOAD_FINISHED: {
            WebKitPrintOperation *operation = CONTEXT->operation;
            webkit_print_operation_print(operation);
            break;
        }
    }
}

static void _print_finished(WebKitPrintOperation* operation, void* user_data) {
    g_main_loop_quit(CONTEXT->loop);
}

static int _html2pdf(struct html2pdf_params* params) {
    struct ZHTML2PDF_CONTEXT context;

    GtkPrintSettings *settings = gtk_print_settings_new();
    gtk_print_settings_set_printer(settings, "Print to File");
    gtk_print_settings_set(settings, GTK_PRINT_SETTINGS_OUTPUT_FILE_FORMAT, "pdf");
    gtk_print_settings_set_quality(settings, GTK_PRINT_QUALITY_HIGH);
    gtk_print_settings_set_resolution(settings, 320);
    gtk_print_settings_set_page_set(settings, GTK_PAGE_SET_ALL);
    gtk_print_settings_set_orientation(settings, GTK_PAGE_ORIENTATION_PORTRAIT);
    gtk_print_settings_set_paper_width(settings, 500, GTK_UNIT_MM);

    GtkPageSetup *page_setup = gtk_page_setup_new();
    gtk_page_setup_set_orientation(page_setup, GTK_PAGE_ORIENTATION_PORTRAIT);

    if (params->settings != NULL) {
        GKeyFile *key_file = g_key_file_new();
        g_key_file_load_from_data(key_file, params->settings, (gsize)-1, G_KEY_FILE_NONE, NULL);
        gtk_page_setup_load_key_file(page_setup, key_file, NULL, NULL);
        gtk_print_settings_load_key_file(settings, key_file, NULL, NULL);
        g_key_file_free(key_file);
    }

    gtk_print_settings_set(settings, GTK_PRINT_SETTINGS_OUTPUT_URI, params->output);
    context.settings = settings;

    WebKitWebContext *web_context = webkit_web_context_new_ephemeral();
    WebKitWebView *web_view = WEBKIT_WEB_VIEW(webkit_web_view_new_with_context(web_context));
    // webkit_web_context_set_disk_cache_directory(web_context, "/dev/null");
    // webkit_web_context_set_cache_model(web_context, WEBKIT_CACHE_MODEL_DOCUMENT_VIEWER)

    g_object_ref_sink(web_view);

    WebKitSettings *view_settings = webkit_web_view_get_settings(web_view);
    webkit_settings_set_enable_javascript(view_settings, false);
    webkit_settings_set_enable_page_cache(view_settings, false);
    webkit_settings_set_enable_html5_database(view_settings, false);
    webkit_settings_set_enable_html5_local_storage(view_settings, false);
    webkit_settings_set_enable_offline_web_application_cache(view_settings, false);

    WebKitPrintOperation *operation = webkit_print_operation_new(web_view);
    webkit_print_operation_set_print_settings(operation, settings);
    webkit_print_operation_set_page_setup(operation, page_setup);
    g_signal_connect(operation, "finished", (void (*)(void))_print_finished, &context);
    context.operation = operation;

    GMainLoop *loop = g_main_loop_new(NULL, false);
    context.loop = loop;

    g_signal_connect(web_view, "load-changed", (void (*)(void))_web_view_load_changed, &context);

    webkit_web_view_load_uri(web_view, params->input);

    g_main_loop_run(loop);

    g_object_unref(G_OBJECT(operation));
    g_object_unref(G_OBJECT(settings));
    g_object_unref(G_OBJECT(page_setup));
    gtk_widget_destroy(GTK_WIDGET(web_view));
    g_object_unref(G_OBJECT(web_view));
    g_object_unref(G_OBJECT(web_context));
    g_main_loop_unref(loop);

    if (params->wait_mutex && params->wait_cond && params->wait_data) {
        g_mutex_lock(params->wait_mutex);
        (*params->wait_data)++;
        g_cond_signal(params->wait_cond);
        g_mutex_unlock(params->wait_mutex);
    }

    return G_SOURCE_REMOVE;
}

static int _read_file_contents(const char* filename, unsigned char** output) {
    struct stat st;

    if (stat(filename, &st) == -1) return -1;

    size_t file_size = st.st_size;

    FILE *file_pointer = fopen(filename, "rb");
    if (!file_pointer) return -2;

    unsigned char *file_content = malloc(file_size);
    if (!file_content) {
        fclose(file_pointer);
        return -3;
    }

    size_t bytes_read = fread(file_content, 1, file_size, file_pointer);
    if (bytes_read != file_size) {
        free(file_content);
        fclose(file_pointer);
        return -4;
    }

    int encoded_size = ((file_size + 2) / 3) * 4;
    unsigned char *encoded = malloc(encoded_size);
    if (!encoded) {
        free(file_content);
        fclose(file_pointer);
        return -5;
    }

    _encode_base64(file_content, file_size, encoded);

    free(file_content);
    *output = encoded;

    fclose(file_pointer);
    return encoded_size;
}

const char base64_table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

void _encode_base64(const unsigned char* input, size_t length, unsigned char* output) {
    int i, j = 0;
    unsigned char a, b, c;

    for (i = 0; i < length; i += 3) {
        a = input[i];
        b = (i + 1 < length) ? input[i + 1] : 0;
        c = (i + 2 < length) ? input[i + 2] : 0;

        output[j++] = base64_table[a >> 2];
        output[j++] = base64_table[((a & 0x03) << 4) | (b >> 4)];
        output[j++] = (i + 1 < length) ? base64_table[((b & 0x0F) << 2) | (c >> 6)] : '=';
        output[j++] = (i + 2 < length) ? base64_table[c & 0x3F] : '=';
    }
}
