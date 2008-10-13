
## ------------------------------------------------------------------------------
## Sub-directories

set (system_test_sources
  error_code_test
  error_code_user_test
  header_only_test
  initialization_test
  system_error_test
  )

foreach (src ${system_test_sources})
  ## compiler instructions
  add_executable (${src} ${src}.cpp)
  ## linker instrucions
  target_link_libraries (${src} boost_system)
  ## testing
  add_test (${src} ${src})
endforeach (src)
