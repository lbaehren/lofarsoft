
## ------------------------------------------------------------------------------
## Test programs for msfits/MSFits

SET (tests
  tMSConcat
  tMSSelection
  tfits2ms
  )

## ------------------------------------------------------------------------------
## Build the test programs

FOREACH (src ${tests})
  ADD_EXECUTABLE (${src} ${src}.cc)
  TARGET_LINK_LIBRARIES (${src} msfits)
ENDFOREACH (src)
