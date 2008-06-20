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

find_path (PYBDSM_cmake CMakeSettings.cmake
  PATHS 
  ${PYBDSM_SOURCE_DIR}
  ${PYBDSM_SOURCE_DIR}/..
  ${PYBDSM_SOURCE_DIR}/../..
  ${PYBDSM_SOURCE_DIR}/../../..
  PATH_SUFFIXES
  devel_common/cmake
  )

if (PYBDSM_cmake)
  list (APPEND CMAKE_MODULE_PATH ${PYBDSM_cmake})
else (PYBDSM_cmake)
  message (FATAL_ERROR "Unable to locate additional CMake scripts!")
endif (PYBDSM_cmake)

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


