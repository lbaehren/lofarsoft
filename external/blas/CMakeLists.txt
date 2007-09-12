
## -----------------------------------------------------------------------------
##
##  blas
##  `-- cblas
##      |-- lib
##      |-- src
##      `-- test
##
## -----------------------------------------------------------------------------

project (BLAS Fortran C)

subdirs (
  cblas
  )

include_directories (
  .
  ..
  cblas
  cblas/src
  cblas/testing
  )

## -----------------------------------------------------------------------------
## Required external libraries

## Locate additional CMake find scripts

find_path (BLAS_cmake FindG2C.cmake FindLAPACK.cmake
  PATHS 
  ${BLAS_SOURCE_DIR}
  ${BLAS_SOURCE_DIR}/..
  ${BLAS_SOURCE_DIR}/../..
  ${BLAS_SOURCE_DIR}/../../..
  PATH_SUFFIXES
  devel_common/cmake
  )

if (BLAS_cmake)
  include (${BLAS_cmake}/FindG2C.cmake)
endif (BLAS_cmake)

## -----------------------------------------------------------------------------
## Build "libblas" first

FILE (GLOB blas_sources *.f *.c)

add_library (blas ${blas_sources})

## -----------------------------------------------------------------------------
##  Installation

## check for location installation location

find_path (BLAS_INSTALL_PREFIX release_area.txt
  PATHS
  ${BLAS_SOURCE_DIR}
  PATH_SUFFIXES
  ../release
  ../../release
  NO_DEFAULT_PATH
  )

if (BLAS_INSTALL_PREFIX)
  message (STATUS "Installation area located for package BLAS.")
  set (CMAKE_INSTALL_PREFIX ${BLAS_INSTALL_PREFIX}) 
endif (BLAS_INSTALL_PREFIX)

## Installation of the library

install (TARGETS blas
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )

