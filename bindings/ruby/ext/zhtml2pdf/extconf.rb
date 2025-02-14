require 'mkmf'

so_name = 'zhtml2pdf'
target_path = File.join(Dir.pwd, 'build', 'lib', 'libzhtml2pdf.so')
cache_path = File.join(Dir.pwd, '.zig-cache')
config = RbConfig::CONFIG

File.open('Makefile', 'w') do |f|
  f.puts <<~MAKEFILE
    all:
    \tRUBY_LIBDIR=#{config['libdir']} RUBY_HDRDIR=#{config['rubyhdrdir']} RUBY_ARCHHDRDIR=#{config['rubyarchhdrdir']} zig build -Doptimize=ReleaseFast -p ./build
    \tsh -c 'if [ -d '#{cache_path}' ]; then rm -r '#{cache_path}'; fi'
    \tsh -c 'if [ -d 'zig-out' ]; then rm -r 'zig-out'; fi'

    install: all
    \tcp #{target_path} ../../lib/#{so_name}.so
    \tsh -c 'if [ -d 'build' ]; then rm -r 'build'; fi'

    clean:
    \trm -rf build #{target_path}
    \tsh -c 'if [ -d '#{cache_path}' ]; then rm -r '#{cache_path}'; fi'
    \tsh -c 'if [ -d 'zig-out' ]; then rm -r 'zig-out'; fi'
    \tsh -c 'if [ -d 'build' ]; then rm -r 'build'; fi'

  MAKEFILE
end
