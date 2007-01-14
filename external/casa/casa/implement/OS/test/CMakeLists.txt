
## ------------------------------------------------------------------------------
## Set up the list with test program sources

set (tests
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

foreach (src ${tests})
  add_executable (${src} ${src}.cc)
  target_link_libraries (${src} casa)
endforeach (src)

