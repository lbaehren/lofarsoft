
## ------------------------------------------------------------------------------
## Sub-directories

foreach (src test_re2c_lexer test_slex_lexer)
  ## compiler instructions
  add_executable (${src} ${src}.cpp)
  ## linker instrucions
  target_link_libraries (${src} boost_wave)
  ## testing
  add_test (${src} ${src})
endforeach (src)
