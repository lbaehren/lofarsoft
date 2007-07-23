
## ------------------------------------------------------------------------------
## Test programs for casa/Quanta

SET (tests
tQuantum
tQuantumHolder
tUnit
  )

## ------------------------------------------------------------------------------
## Build the test programs

FOREACH (src ${tests})
  ADD_EXECUTABLE (${src} ${src}.cc)
  TARGET_LINK_LIBRARIES (${src} casa)
ENDFOREACH (src)
