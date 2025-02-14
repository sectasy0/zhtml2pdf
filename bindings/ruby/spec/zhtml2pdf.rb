require 'rspec'
require 'base64'
require_relative '../lib/zhtml2pdf'

describe ZHtml2Pdf do
  let(:converter) { ZHtml2Pdf::Converter.new }
  let(:html_text) {
    "data:text/html;charset=utf-8,<!DOCTYPE html>
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
  }
  let(:base64_encoded) { Base64.strict_encode64(File.open("#{File.dirname(__FILE__)}/test.pdf", 'rb').read).encode('ASCII-8BIT') }

  describe '#initialize' do
    it 'initializes the converter' do
      ZHtml2Pdf::Converter.new
    end
  end

  describe '#convert' do
    it 'calls zhtml2pdf with correct parameters' do
      content = converter.convert(html_text, nil, nil)
    end

    it 'returns a string base64 encoded with at least 98% similarity' do
      content = converter.convert(html_text, nil, nil)

      total_chars = [content.length, base64_encoded.length].min
      match_count = 0

      content.chars.zip(base64_encoded.chars).each_with_index do |(a, b), index|
        match_count += 1 if a == b
      end

      # I test here against similarity cause WebKit2 during print
      # adds timestamps to metadata and we ain't want to get rid off those guys
      similarity = match_count.to_f / total_chars
      expect(similarity).to be >= 0.98
    end
  end
end
