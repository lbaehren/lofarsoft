
## ------------------------------------------------------------------------------
## Set up the list with test program sources

SET (tests
  dLogging2
  dLogging
  tLoggerHolder
  tLogging
  )

## ------------------------------------------------------------------------------
## Build the test programs

FOREACH (src ${tests})
  ADD_EXECUTABLE (${src} ${src}.cc)
  TARGET_LINK_LIBRARIES (${src} tables casa)
ENDFOREACH (src)
