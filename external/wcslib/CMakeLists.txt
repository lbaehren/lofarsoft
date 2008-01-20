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

## -----------------------------------------------------------------------------
## Check for required external libraries

## Locate additional CMake find scripts

find_path (WCSLIB_cmake FindCFITSIO.cmake FindG2C.cmake FindLAPACK.cmake
  PATHS 
  ${WCSLIB_SOURCE_DIR}
  ${WCSLIB_SOURCE_DIR}/..
  ${WCSLIB_SOURCE_DIR}/../..
  ${WCSLIB_SOURCE_DIR}/../../..
  PATH_SUFFIXES
  devel_common/cmake
  )

## locate external packages

if (WCSLIB_cmake)
  ## CFITSIO
  set (CFITSIO_FIND_QUIETLY 1)
  include (${WCSLIB_cmake}/FindCFITSIO.cmake)
  ## PGPLOT
  set (PGPLOT_FIND_QUIETLY 1)
#  include (${WCSLIB_cmake}/FindPGPLOT.cmake)
  ## Boost
  set (BOOST_FIND_QUIETLY 1)
  include (${WCSLIB_cmake}/FindBoost.cmake)
  ## Python
  set (PYTHON_FIND_QUIETLY 1)
  include (${WCSLIB_cmake}/FindPython.cmake)
else (WCSLIB_cmake)
  message (SEND_ERROR "Unable to locate additional CMake find scripts!")
endif (WCSLIB_cmake)

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
