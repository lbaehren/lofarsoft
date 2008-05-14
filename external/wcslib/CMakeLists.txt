##------------------------------------------------------------------------------
## $Id:: CMakeLists.txt 489 2007-08-01 11:40:38Z baehren                       $
##------------------------------------------------------------------------------

## -----------------------------------------------------------------------------
## Project name and version number number

## Name of the project
project (WCSLIB)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.4)

set (WCSLIB_VERSION_MAJOR 4)
set (WCSLIB_VERSION_MINOR 2)
set (WCSLIB_VERSION "${WCSLIB_VERSION_MAJOR}.${WCSLIB_VERSION_MINOR}")

include (CTest)

## -----------------------------------------------------------------------------
## User defined build/install options

option (WCSLIB_PYTHON_WRAPPER "Generate Python wrapper for WCSLIB?" 0)

## ------------------------------------------------------------------------------
## Set CMAKE_MODULE_PATH to load custom CMake modules

find_path (USG_ROOT devel_common/cmake/CMakeSettings.cmake
  PATHS 
  ${WCSLIB_SOURCE_DIR}
  ${WCSLIB_SOURCE_DIR}/..
  ${WCSLIB_SOURCE_DIR}/../..
  ${WCSLIB_SOURCE_DIR}/../../..
  ENV LOFARSOFT
  )

if (USG_ROOT)
  include (${USG_ROOT}/devel_common/cmake/CMakeSettings.cmake)
else (USG_ROOT)
  message (FATAL_ERROR "Unable to locate additional CMake scripts!")
endif (USG_ROOT)


set (CFITSIO_FIND_QUIETLY 1)
set (PGPLOT_FIND_QUIETLY 1)
set (BOOST_FIND_QUIETLY 1)
set (PYTHON_FIND_QUIETLY 1)

include (FindCFITSIO)
#  include (FindPGPLOT)
include (FindBoost)
include (FindPython)

## libm -----------------------------------------

find_library (libm m)

## -----------------------------------------------------------------------------
## Check for additional tools which are required

find_program (flex_bin flex)

## -----------------------------------------------------------------------------
## Subdirectories

add_subdirectory (${WCSLIB_SOURCE_DIR}/C)
add_subdirectory (${WCSLIB_SOURCE_DIR}/FORTRAN)
add_subdirectory (${WCSLIB_SOURCE_DIR}/pgsbox)

if (WCSLIB_PYTHON_WRAPPER AND HAVE_PYTHON)
  add_subdirectory (${WCSLIB_SOURCE_DIR}/Python)
endif (WCSLIB_PYTHON_WRAPPER AND HAVE_PYTHON)

## -----------------------------------------------------------------------------
## Installation

find_path (WCSLIB_INSTALL_PREFIX release_area.txt
  PATHS
  ${WCSLIB_SOURCE_DIR}
  PATH_SUFFIXES
  ../release
  ../../release
  NO_DEFAULT_PATH
  )

if (WCSLIB_INSTALL_PREFIX)
  message (STATUS "Installation area located for package WCSLIB.")
  set (CMAKE_INSTALL_PREFIX ${WCSLIB_INSTALL_PREFIX}) 
endif (WCSLIB_INSTALL_PREFIX)
