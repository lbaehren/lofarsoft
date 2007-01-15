
## ------------------------------------------------------------------------------
## Set up the list with test program sources

set (tests
  tInput
  tParam
  )

## ------------------------------------------------------------------------------
## Build the test programs

foreach (src ${tests})
  add_executable (${src} ${src}.cc)
  target_link_libraries (${src} casa)
endforeach (src)

