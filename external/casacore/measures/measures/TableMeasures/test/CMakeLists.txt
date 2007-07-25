
## ------------------------------------------------------------------------------
## Test programs for measures/TableMeasures

SET (tests
  tTableMeasures
  tTableQuantum
  )

## ------------------------------------------------------------------------------
## Build the test programs

FOREACH (src ${tests})
  ADD_EXECUTABLE (${src} ${src}.cc)
  TARGET_LINK_LIBRARIES (${src} measures)
ENDFOREACH (src)
