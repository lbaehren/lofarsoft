
## ------------------------------------------------------------------------------
## Test programs for msvis/MSVis

SET (tests
  tMSCalEnums
  tVisIter
  tVisibilityIterator
  )

## ------------------------------------------------------------------------------
## Build the test programs

FOREACH (src ${tests})
  ADD_EXECUTABLE (${src} ${src}.cc)
  TARGET_LINK_LIBRARIES (${src} msvis)
ENDFOREACH (src)
