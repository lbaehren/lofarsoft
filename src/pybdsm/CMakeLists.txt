##------------------------------------------------------------------------------
## $Id:: CMakeLists.txt 489 2007-08-01 11:40:38Z baehren                       $
##------------------------------------------------------------------------------

## -----------------------------------------------------------------------------
## Project name and version number number

## Name of the project
project (PYBDSM)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.4)

#set (PYBDSM_VERSION_MAJOR 0)
#set (PYBDSM_VERSION_MINOR 0)
#set (PYBDSM_VERSION "${PYBDSM_VERSION_MAJOR}.${PYBDSM_VERSION_MINOR}")

## -----------------------------------------------------------------------------
## User defined build/install options



## -----------------------------------------------------------------------------
## Standard locations where to look for required components

set (bin_locations
  /usr/bin
  /usr/local/bin
  /sw/bin
  ${PYBDSM_SOURCE_DIR}/../release/bin
  ${PYBDSM_SOURCE_DIR}/../../release/bin
  )

set (include_locations
  /usr/include
  /usr/local/include
  /Developer/SDKs/MacOSX10.4u.sdk/usr/include/c++/4.0.0
  /sw/include
  /sw/lib/gcc4.2/include/c++/4.2.1
)

set (lib_locations
  /usr/lib
  /usr/local/lib
  /sw/lib
)

include (CheckFunctionExists)
include (CheckIncludeFiles)
include (CheckLibraryExists)
include (CPack)

## -----------------------------------------------------------------------------
## Check for required external libraries

## Locate additional CMake find scripts

find_path (PYBDSM_cmake FindCFITSIO.cmake FindG2C.cmake FindLAPACK.cmake
  PATHS 
  ${PYBDSM_SOURCE_DIR}
  ${PYBDSM_SOURCE_DIR}/..
  ${PYBDSM_SOURCE_DIR}/../..
  ${PYBDSM_SOURCE_DIR}/../../..
  PATH_SUFFIXES
  devel_common/cmake
  )

## locate external packages

if (PYBDSM_cmake)
  ## Boost
  set (BOOST_FIND_QUIETLY 1)
  include (${PYBDSM_cmake}/FindBoost.cmake)
  ## Python
  set (PYTHON_FIND_QUIETLY 1)
  include (${PYBDSM_cmake}/FindPython.cmake)
else (PYBDSM_cmake)
  message (SEND_ERROR "Unable to locate additional CMake find scripts!")
endif (PYBDSM_cmake)


## -----------------------------------------------------------------------------
## Check for additional tools which are required



## -----------------------------------------------------------------------------
## Subdirectories

add_subdirectory (${PYBDSM_SOURCE_DIR}/implement)

## -----------------------------------------------------------------------------
## Installation

find_path (PYBDSM_INSTALL_PREFIX release_area.txt
  PATHS
  ${PYBDSM_SOURCE_DIR}
  PATH_SUFFIXES
  ../release
  ../../release
  NO_DEFAULT_PATH
  )

if (PYBDSM_INSTALL_PREFIX)
  message (STATUS "Installation area located for package PYBDSM.")
  set (CMAKE_INSTALL_PREFIX ${PYBDSM_INSTALL_PREFIX}) 
endif (PYBDSM_INSTALL_PREFIX)

## -----------------------------------------------------------------------------
## Summary of settings


