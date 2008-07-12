
## ------------------------------------------------------------------------------
## Sub-directories

foreach (src arithmetic array comparison control debug facilities
    iteration
    list
    logical
    repetition
    selection
    seq
    slot
    tuple
    )
  ## compiler instructions
  add_executable (${src} ${src}.cpp)
#  ## linker instrucions
#  target_link_libraries (${src} boost_signals)
  ## testing
  add_test (${src} ${src})
endforeach (src)
