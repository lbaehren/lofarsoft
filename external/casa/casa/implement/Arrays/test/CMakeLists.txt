
## ------------------------------------------------------------------------------
## Set up the list with test program sources

set (tests
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
  )

## ------------------------------------------------------------------------------
## Build the test programs

foreach (src ${tests})
  add_executable (${src} ${src}.cc)
  target_link_libraries (${src} casa)
endforeach (src)

