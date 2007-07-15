
## ------------------------------------------------------------------------------
## Test programs for casa/BasicSL

set (tests
  tComplex
  tConstants
  tString
  )

## ------------------------------------------------------------------------------
## Build the test programs

FOREACH (src ${tests})
  ADD_EXECUTABLE (${src} ${src}.cc)
  TARGET_LINK_LIBRARIES (${src} casa)
ENDFOREACH (src)
