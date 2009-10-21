##------------------------------------------------------------------------------
## $Id:: CMakeLists.txt 489 2007-08-01 11:40:38Z baehren                       $
##------------------------------------------------------------------------------

## -----------------------------------------------------------------------------
## Project name and version number number

## Name of the project
project (PYBDSM)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.6)

## -----------------------------------------------------------------------------
## Check for required external libraries

## Locate additional CMake find scripts

find_path (USG_ROOT devel_common/cmake/CMakeSettings.cmake
  PATHS 
  ${PYBDSM_SOURCE_DIR}
  ${PYBDSM_SOURCE_DIR}/..
  ${PYBDSM_SOURCE_DIR}/../..
  ${PYBDSM_SOURCE_DIR}/../../..
  )

if (USG_ROOT)
  include (${USG_ROOT}/devel_common/cmake/CMakeSettings.cmake)
else (USG_ROOT)
  message (FATAL_ERROR "Unable to locate additional CMake scripts!")
endif (USG_ROOT)

## ==============================================================================
##
##  Options
##
## ==============================================================================

option (PYBDSM_DEBUGGING_MESSAGES "Print debugging information?"           NO )
option (PYBDSM_VERBOSE_CONFIGURE  "Verbose output during configuration?"   NO )

## Handle option: verbose more during configuration on/off

if (NOT PYBDSM_VERBOSE_CONFIGURE)
  set (BOOST_FIND_QUIETLY     YES )
  set (NUMPY_FIND_QUIETLY     YES )
  set (NUM_UTIL_FIND_QUIETLY  YES )
  set (PYTHON_FIND_QUIETLY    YES )
endif (NOT PYBDSM_VERBOSE_CONFIGURE)

## ==============================================================================
##
##  External dependencies
##
## ==============================================================================

## Boost
include (FindBoost)
## Python
include (FindPython)
## Python modules
include (FindNumUtil)
include (FindNumPy)

## -----------------------------------------------------------------------------
## Subdirectories

add_subdirectory (implement)
add_subdirectory (scripts)

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

message (STATUS "+============================================================+")
message (STATUS "| pybdsm: Summary of configuration settings                  |")
message (STATUS "+------------------------------------------------------------+")
message (STATUS " Verbose mode during configuration = ${PYBDSM_VERBOSE_CONFIGURE}")
message (STATUS " Print debugging messages          = ${PYBDSM_DEBUGGING_MESSAGES}")

message (STATUS " num_util headers = ${NUM_UTIL_INCLUDES}")