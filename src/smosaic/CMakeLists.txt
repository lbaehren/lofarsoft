## Name of the project
project (smosaic)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.6)

## ------------------------------------------------------------------------------
## Set CMAKE_MODULE_PATH to load custom CMake modules

if (NOT USG_ROOT)
  find_path (USG_ROOT devel_common/cmake/CMakeSettings.cmake
    PATHS 
    ${smosaic_SOURCE_DIR}
    ${smosaic_SOURCE_DIR}/..
    ${smosaic_SOURCE_DIR}/../..
    ${smosaic_SOURCE_DIR}/../../..
    ENV LOFARSOFT
    )
  
  if (USG_ROOT)
    include (${USG_ROOT}/devel_common/cmake/CMakeSettings.cmake)
  else (USG_ROOT)
    message (FATAL_ERROR "Unable to locate additional CMake scripts!")
  endif (USG_ROOT)
endif (NOT USG_ROOT)

## =============================================================================
##
##  External dependencies
##
## =============================================================================

set (CASACORE_FIND_QUIETLY   YES )
set (CFITSIO_FIND_QUIETLY    YES )
set (HDF5_FIND_QUIETLY       YES )
set (LAPACK_FIND_QUIETLY     YES )
set (WCSLIB_FIND_QUIETLY     YES )

include (FindCASACORE)
include (FindCFITSIO)
include (FindHDF5)
include (FindLAPACK)
include (FindWCS)

## =============================================================================
##
##  Build instructions
##
## =============================================================================

include_directories (
  ${CASACORE_INCLUDES}
  ${CFITSIO_INCLUDES}
  ${HDF5_INCLUDES}
  ${WCSLIB_INCLUDES}
  )

add_executable (smosaic smosaic.cc)

target_link_libraries (smosaic
  ${CASACORE_LIBRARIES}
  ${HDF5_LIBRARIES}
  ${CFITSIO_LIBRARIES}
  ${WCSLIB_LIBRARIES}
  ${LAPACK_LIBRARIES}
  )
