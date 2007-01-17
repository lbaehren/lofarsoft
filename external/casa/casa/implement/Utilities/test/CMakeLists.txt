
PROJECT (CASA_casa_tUtilities)

## ------------------------------------------------------------------------------
## Set up the list with test program sources

set (tests
  dCOWptr
  dMUString
  tBinarySearch
  tBitVector
  tCompositeNumber
  tCopy
  tCountedPtr
  tCOWPtr
  tDataType
  tDefaultValue
  tDynBuffer
  tFallible
  tGenSort
  tLinearSearch
  tPtrHolder
  tRegex_1
  tRegex
  tRegister
  tSort_1
  tSort
  )

## ------------------------------------------------------------------------------
## Build the test programs

foreach (src ${tests})
  add_executable (${src} ${src}.cc)
  target_link_libraries (${src} casa)
endforeach (src)
