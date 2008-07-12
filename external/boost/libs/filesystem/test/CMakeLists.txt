
## ------------------------------------------------------------------------------
## Sub-directories

foreach (src convenience_test equivalent fstream_test large_file_support_test
    operations_test path_test wide_test)
  ## compiler instructions
  add_executable (${src} ${src}.cpp)
  ## linker instrucions
  target_link_libraries (${src} boost_filesystem)
  ## testing
  add_test (${src} ${src})
endforeach (src)
