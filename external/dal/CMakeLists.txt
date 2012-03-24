## ==============================================================================
##
##  CMake project settings
##
## ==============================================================================

## Name of the project handled by CMake
project (DAL)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.8)

## Enforced CMake policy
cmake_policy (VERSION 2.8)

##  Package information
set (DAL_GIT_REPOSITORY https://github.com/nextgen-astrodata/DAL.git CACHE STRING "DAL git repository.")

## ==============================================================================
##
##  Options
##
## ==============================================================================

option (DAL_BUILD_SHARED_LIBS "Build DAL as shared library?" ON)

## ==============================================================================
##
##  System inspection
##
## ==============================================================================

if (NOT LUS_SOURCE_DIR)
  find_path (LUS_SOURCE_DIR devel_common/cmake/LUS_CMakeSettings.cmake
    PATHS
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/..
    ${PROJECT_SOURCE_DIR}/../..
    ${PROJECT_SOURCE_DIR}/../../..
    ENV LOFARSOFT
    )

  if (LUS_SOURCE_DIR)
    include (${LUS_SOURCE_DIR}/devel_common/cmake/LUS_CMakeSettings.cmake)
  else (LUS_SOURCE_DIR)
    message (FATAL_ERROR "Unable to locate additional CMake scripts!")
  endif (LUS_SOURCE_DIR)
endif (NOT LUS_SOURCE_DIR)

## ==============================================================================
##
##  Build instructions
##
## ==============================================================================

include (ExternalProject)

ExternalProject_Add (dal
  GIT_REPOSITORY ${DAL_GIT_REPOSITORY}
  CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX} -DHDF5_C_INCLUDE_DIR=${LUS_SOURCE_DIR}/include/hdf5 -DPYTHON_DEST=${CMAKE_INSTALL_PREFIX}/lib/python -DBUILD_SHARED_LIBS=${DAL_BUILD_SHARED_LIBS}
  )

add_dependencies (dal git)

