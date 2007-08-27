
## ------------------------------------------------------------------------------
## Test programs for casa/IO

SET (tests
  dCOWptr
  dMUString
  tBinarySearch
  tBitVector
  tCOWPtr
  tCompositeNumber
  tCopy
  tCountedPtr
  tDataType
  tDefaultValue
  tDynBuffer
  tFallible
  tGenSort
  tLinearSearch
  tPtrHolder
  tRegex
  tRegex_1
  tRegister
  tSort
  tSort_1
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
