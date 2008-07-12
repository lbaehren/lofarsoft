
## ------------------------------------------------------------------------------
## Sub-directories

foreach (src dead_slot_test deletion_test ordering_test random_signal_system
    signal_n_test
    signal_test
    trackable_test)
  ## compiler instructions
  add_executable (${src} ${src}.cpp)
  ## linker instrucions
  target_link_libraries (${src} boost_signals)
  ## testing
  add_test (${src} ${src})
endforeach (src)
