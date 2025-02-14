# `libzhtml2pdf` - WebKit2-based HTML-to-PDF Converter with Simple API

`libzhtml2pdf` is a C-based library designed for converting HTML documents into PDF format. This library uses **WebKit2GTK-4.0** as its rendering engine for HTML content, allowing you to create high-quality PDFs from HTML with ease. It offers a simple API that can be integrated into various applications and projects.

---

## Prerequisites

Before compiling `libzhtml2pdf`, you need to ensure that the following dependencies are installed on your system:

### 1. **WebKit2GTK-4.0**
   - **Version**: `4.0` (or later)

### 2. **Zig Compiler**
   - **Version**: `0.13` (or later)

---

## Building `libzhtml2pdf`

Once you have the prerequisites installed, you can proceed to compile `libzhtml2pdf` on your system. Follow these steps:

1. **Clone the Repository**:

   First, clone the `libzhtml2pdf` repository from GitHub or another source:
   ```bash
   git clone https://github.com/your-repository/libzhtml2pdf.git
   cd libzhtml2pdf
   build zig
   ```

   After successfully compilation it will produce static library to `zig-out/lib`


## CAPI Example usage

```c
#include "zhtml2pdf.h"

int main() {
    init_zhtml2pdf();

    const char* html_content = "<html><body><h1>Sample PDF</h1><p>This is a test PDF generated from HTML.</p></body></html>";

    unsigned char* pdf_data = NULL;
    int size = zhtml2pdf_convert(html_content);
    if (size < 0) {
        printf("Failed to convert html to pdf!\n");
        return -1;
    }

    printf("PDF successfully created!\n");
    // Do whatever you want with pdf, output is in base64

    zhtml2pdf_free(pdf_data);
    deinit_zhtml2pdf();

    return 0;
}

```
