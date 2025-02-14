# `zhtml2pdf` - Ruby bindings for libzhtml2pdf

`zhtml2pdf` is a Ruby library that provides bindings to the `libzhtml2pdf` library, which allows you to convert HTML content into a PDF document. This is useful for generating PDFs dynamically from HTML content, making it perfect for use cases like generating reports, invoices, and other documents directly from HTML.

---



## Install the Gem

Before using the `zhtml2pdf` gem, make sure you have it installed. Download repo and run

```ruby
rake build
```

## Example Usage

Below is an example of how to use the `zhtml2pdf` gem to convert an HTML document into a PDF. This demonstrates the basic functionality, from setting up the converter to providing HTML content and converting it into a PDF.
```ruby
require 'zhtml2pdf'

converter = ZHtml2Pdf::Converter.new

# Sample HTML content to convert. This could be a string of HTML or a file reference.
# `file:///path/to/your/file.html`
html_text = "data:text/html;charset=utf-8,<!DOCTYPE html>
    <html lang='en'>
    <head>
        <meta charset='UTF-8'>
        <meta name='viewport' content='width=device-width, initial-scale=1.0'>
        <title>Sample PDF Content</title>
        <style>
            body { font-family: Arial, sans-serif; margin: 20px; }
            h1 { color: #333366; }
            p { font-size: 14px; color: #555555; }
            .container { border: 1px solid #ddd; padding: 15px; border-radius: 5px; }
        </style>
    </head>
    <body>
        <div class='container'>
            <h1>PDF Generation Test</h1>
            <p>This is a sample HTML document for testing PDF conversion.</p>
            <ul>
                <li>Item 1</li>
                <li>Item 2</li>
                <li>Item 3</li>
            </ul>
        </div>
    </body>
    </html>"

# Second argument is webkit settings string in .ini format.
# Last argument is default_css.
content = converter.convert(html_text, nil, nil)
puts content
```
