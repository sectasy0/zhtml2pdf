require 'rake'

MAKEFILE_PATH = './Makefile'

task(:compile) do
  cd 'bindings/ruby/ext/zhtml2pdf' do
    ruby 'extconf.rb'
    sh 'make install'
  end
end

task(build: :compile) do
  sh('gem build zhtml2pdf.gemspec')
end
