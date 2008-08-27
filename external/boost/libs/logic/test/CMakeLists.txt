
## ------------------------------------------------------------------------------
## Sub-directories

foreach (src tribool_io_test tribool_rename_test tribool_test)
  ## compiler instructions
  add_executable (${src} ${src}.cpp)
#  ## linker instrucions
#  target_link_libraries (${src} boost_signals)
  ## testing
  add_test (${src} ${src})
endforeach (src)
