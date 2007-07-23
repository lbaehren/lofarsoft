
## ------------------------------------------------------------------------------
## Test programs for casa/OS

SET (tests
tCanonicalConversion
tConversion
tDataConversion
tDirectory
tDirectoryIterator
tEnvVar
tFile
tHostInfo
tIBMConversion
tLECanonicalConversion
tMemory
tModcompConversion
tPath
tPrecTimer
tRegularFile
tSymLink
tTime
tTimer
tVAXConversion
  )

## ------------------------------------------------------------------------------
## Build the test programs

FOREACH (src ${tests})
  ADD_EXECUTABLE (${src} ${src}.cc)
  TARGET_LINK_LIBRARIES (${src} casa)
ENDFOREACH (src)
