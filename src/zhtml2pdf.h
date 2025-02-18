#ifndef ZHTML2PDF_H
#define ZHTML2PDF_H

int init_zhtml2pdf();

int zhtml2pdf(
    const char* input,
    const char* settings,
    const char* css,
    unsigned char** output
);

void zhtml2pdf_free(void* buffer);

int deinit_zhtml2pdf();

#endif //ZHTML2PDF_H
