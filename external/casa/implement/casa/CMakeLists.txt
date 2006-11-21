## CASA/implement/casa

PROJECT (CASA)

## -----------------------------------------------------------------------------
## Location of the header files

INCLUDE_DIRECTORIES (
  .
  ..
  ../..
  ${CASA_SOURCE_DIR}/implement
  ${CASA_SOURCE_DIR}/implement/casa
  /usr/include
  /usr/local/include
  /usr/include/c++/4.0.0
  )

## -----------------------------------------------------------------------------
## libcasa

add_definitions (-DAIPS_STDLIB -DAIPS_DARWIN -DAIPS_BIG_ENDIAN -DNATIVE_EXCP)

ADD_LIBRARY (casa
  # casa/implement
  aips.cc
  version2.cc
  # casa/implement/Arrays
  Arrays/Array2.cc
  Arrays/Array2Math.cc
  Arrays/ArrayBase.cc
  Arrays/ArrayError.cc
  Arrays/ArrayIO.cc
  Arrays/ArrayIter.cc
  Arrays/ArrayLogical.cc
  Arrays/ArrayMath.cc
  Arrays/ArrayPosIter.cc
  Arrays/ArrayUtil2.cc
  Arrays/AxesMapping.cc
  Arrays/AxesSpecifier.cc
  Arrays/Cube.cc
  Arrays/ExtendSpecifier.cc
  Arrays/IPosition2.cc
  Arrays/MaskArrIO.cc
  Arrays/MaskArrLogi.cc
  Arrays/MaskArrMath2.cc
  Arrays/MaskedArray.cc
  Arrays/Matrix2Math.cc
  Arrays/MatrixIter.cc
  Arrays/MatrixMath.cc
  Arrays/Slicer.cc
  Arrays/Vector2.cc
  Arrays/VectorIter.cc
  Arrays/Array.cc
  Arrays/ArrayUtil.cc
  Arrays/IPosition.cc
  Arrays/MaskArrMath.cc
  Arrays/Matrix.cc
  Arrays/Vector.cc
  )
