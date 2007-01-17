
PROJECT (CASA_casa_tArrays)

## ------------------------------------------------------------------------------
## Set up the list with test program sources

SET (tests
  dArrayAccessor
  tArrayAccessor
  tArray
  tArrayIO2
  tArrayIO3
  tArrayIO
  tArrayIter1
  tArrayIteratorSTL
  tArrayIter
  tArrayLogical
  tArrayMath2
  tArrayPosIter
  tArrayUtil
  tAxesSpecifier
  tExtendSpecifier
  tIPosition
  tLinAlgebra
  tMaskArrExcp
  tMaskArrIO
  tMaskArrLogi
  tMaskArrMath0
  tMaskArrMath1
  tMaskArrMath2
  tMaskedArray
  tMatrixMath
  tSlicer
  tStringArray
  ## -- custom test files
  dArray
  )

## ------------------------------------------------------------------------------
## Build the test programs

FOREACH (src ${tests})
  ADD_EXECUTABLE (${src} ${src}.cc)
  TARGET_LINK_LIBRARIES (${src} casa)
ENDFOREACH (src)

