PROJECT (VLT)

SUBDIRS (test)

INCLUDE_DIRECTORIES (. .. ${VLT_SOURCE_DIR}/implement)

ADD_LIBRARY (aomath
  aomath.cc
  svd.cc
  vector.cc
  matrix.cc
  mathBase.cc
  memory.cc
  MAFilter.cc
  MAStdFilter.cc
  )

## ------------------------------------------------------------------------------
## Installation

## installation of libraries

install (
  TARGETS aomath
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )

## installation of header files

file (GLOB aomath_headers *.h)

install (FILES ${aomath_headers}
  DESTINATION include/vlt
  )
