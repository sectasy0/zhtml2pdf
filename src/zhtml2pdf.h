#ifndef ZHTML2PDF_H
#define ZHTML2PDF_H

void init_zhtml2pdf();
void init_loop_zhtml2pdf();

int zhtml2pdf(
    const char* input,
    const char* settings,
    const char* css,
    unsigned char** output
);

void zhtml2pdf_free(void** buffer);

void deinit_zhtml2pdf();
void deinit_loop_zhtml2pdf();

#endif //ZHTML2PDF_H
