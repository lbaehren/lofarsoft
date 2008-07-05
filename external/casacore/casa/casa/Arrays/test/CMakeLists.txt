
## ------------------------------------------------------------------------------
## Test programs for casa/Arrays

SET (tests
  dArrayAccessor
  tArray
  tArrayAccessor
  tArrayIO
  tArrayIO2
  tArrayIO3
  tArrayIter
  tArrayIter1
  tArrayIteratorSTL
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
  tSlidingArrayMath
  tStringArray
  )

## ------------------------------------------------------------------------------
## Build the test programs

FOREACH (src ${tests})
  ## compiler instructions
  ADD_EXECUTABLE (${src} ${src}.cc)
  ## linker instructions
  TARGET_LINK_LIBRARIES (${src} casa_casa ${libm})
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
