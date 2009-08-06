
## -----------------------------------------------------------------------------
##
##  blas
##  `-- cblas
##      |-- lib
##      |-- src
##      `-- test
##
## -----------------------------------------------------------------------------

## Project name
project (BLAS Fortran C)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.6)

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

## ------------------------------------------------------------------------------
## Set CMAKE_MODULE_PATH to load custom CMake modules

find_path (USG_ROOT devel_common/cmake/CMakeSettings.cmake
  PATHS 
  ${BLAS_SOURCE_DIR}
  ${BLAS_SOURCE_DIR}/..
  ${BLAS_SOURCE_DIR}/../..
  ${BLAS_SOURCE_DIR}/../../..
  ENV LOFARSOFT
  )

if (USG_ROOT)
  include (${USG_ROOT}/devel_common/cmake/CMakeSettings.cmake)
else (USG_ROOT)
  message (FATAL_ERROR "Unable to locate additional CMake scripts!")
endif (USG_ROOT)

include (FindGFortran)

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

