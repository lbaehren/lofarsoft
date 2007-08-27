
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
  ## compiler instructions
  ADD_EXECUTABLE (${src} ${src}.cc)
  ## linker instructions
  TARGET_LINK_LIBRARIES (${src} casa_casa)
  ## testing 
  add_test (${src} ${src})
ENDFOREACH (src)

## ------------------------------------------------------------------------------
## Installation

install (TARGETS ${tests}
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)
