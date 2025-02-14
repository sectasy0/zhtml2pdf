Gem::Specification.new do |s|
  s.name        = 'zhtml2pdf'
  s.version     = '0.0.2'
  s.summary     = 'Convert HTML to PDF easily'
  s.description = 'Ruby bindings for libzhtml2pdf to convert HTML files to PDF using webkit2'
  s.authors     = ['sectasy0']
  s.email       = 'sectasy0@gmail.com'
  s.files       = Dir.glob('{LICENSE,README.md,Rakefile,zhtml2pdf.gemspec,bindings/ruby/**/*,src/**/*}')
  s.homepage    = 'https://rubygems.org/gems/zhtml2pdf'
  s.license     = 'MIT'

  s.require_paths = ['bindings/ruby/lib']

  s.extensions    = ['bindings/ruby/ext/zhtml2pdf/extconf.rb']

  s.required_ruby_version = Gem::Requirement.new('>= 3.1.0')
end
