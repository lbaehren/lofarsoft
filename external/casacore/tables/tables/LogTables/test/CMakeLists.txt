
## ------------------------------------------------------------------------------
## Test programs for tables/LogTables

SET (tests
dLogging
dLogging2
tLoggerHolder
tLogging
  )

## ------------------------------------------------------------------------------
## Build the test programs

FOREACH (src ${tests})
  ADD_EXECUTABLE (${src} ${src}.cc)
  TARGET_LINK_LIBRARIES (${src} tables)
ENDFOREACH (src)
