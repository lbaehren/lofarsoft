## Name of the project
project (smosaic)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.6)

## ------------------------------------------------------------------------------
## Set CMAKE_MODULE_PATH to load custom CMake modules

if (NOT LUS_ROOT)
  find_path (LUS_ROOT devel_common/cmake/CMakeSettings.cmake
    PATHS 
    ${smosaic_SOURCE_DIR}
    ${smosaic_SOURCE_DIR}/..
    ${smosaic_SOURCE_DIR}/../..
    ${smosaic_SOURCE_DIR}/../../..
    ENV LOFARSOFT
    )
  
  if (LUS_ROOT)
    include (${LUS_ROOT}/devel_common/cmake/CMakeSettings.cmake)
  else (LUS_ROOT)
    message (FATAL_ERROR "Unable to locate additional CMake scripts!")
  endif (LUS_ROOT)
endif (NOT LUS_ROOT)

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

foreach (varPackage
    CASACORE
    CFITSIO
    HDF5
    LAPACK
    WCSLIB
    )

    ## Load CMake module
    find_package(${varPackage})

    if (${varPackage}_INCLUDES)
        include_directories (${${varPackage}_INCLUDES})
    endif (${varPackage}_INCLUDES)

endforeach (varPackage)

## =============================================================================
##
##  Build instructions
##
## =============================================================================

add_executable (smosaic smosaic.cc)

target_link_libraries (smosaic
  ${CASACORE_LIBRARIES}
  ${HDF5_LIBRARIES}
  ${CFITSIO_LIBRARIES}
  ${WCSLIB_LIBRARIES}
  ${LAPACK_LIBRARIES}
  )
