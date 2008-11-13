##------------------------------------------------------------------------------
## $Id:: CMakeLists.txt 489 2007-08-01 11:40:38Z baehren                       $
##------------------------------------------------------------------------------

## -----------------------------------------------------------------------------
## Project name and version number number

## Name of the project
project (PYBDSM)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.5)

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

## locate external packages

if (USG_ROOT)
  ## Boost
  set (BOOST_FIND_QUIETLY 1)
  include (FindBoost)
  ## Python
  set (PYTHON_FIND_QUIETLY 1)
  include (FindPython)
else (USG_ROOT)
  message (SEND_ERROR "Unable to locate additional CMake find scripts!")
endif (USG_ROOT)

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


