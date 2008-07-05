##------------------------------------------------------------------------------
## $Id::                                                                       $
##------------------------------------------------------------------------------

## -----------------------------------------------------------------------------
## Project name and version number number

## Name of the project
project (NUM_UTIL)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.4)

## Locate additional CMake find scripts
find_path (NUM_UTIL_cmake CMakeSettings.cmake
  PATHS 
  ${NUM_UTIL_SOURCE_DIR}
  ${NUM_UTIL_SOURCE_DIR}/..
  ${NUM_UTIL_SOURCE_DIR}/../..
  ${NUM_UTIL_SOURCE_DIR}/../../..
  ENV LOFARSOFT
  PATH_SUFFIXES
  devel_common/cmake
  )

if (NUM_UTIL_cmake)
  list (APPEND CMAKE_MODULE_PATH ${NUM_UTIL_cmake})
else (NUM_UTIL_cmake)
  message (FATAL_ERROR "Unable to locate additional CMake scripts!")
endif (NUM_UTIL_cmake)

## locate external packages
set (BOOST_FIND_QUIETLY 1)
set (PYTHON_FIND_QUIETLY 1)

include (FindBoost)
include (FindPython)

## -----------------------------------------------------------------------------
## Compilation targets

#  simple_ext.so
#  |-- num_util.o
#  `-- simpletest.o           [T]
#      |-- num_util.o         [T]
#      |   |-- num_util.cpp
#      |   `-- num_util.h
#      |-- num_util.h
#      `-- simpletest.cpp

include_directories (
  ${BOOST_INCLUDES}
  ${PYTHON_INCLUDES}
  ${NUMPY_INCLUDES}
  )

add_definitions (
  -fPIC
  )

add_library (num_util STATIC
  num_util.cpp
  )

## -----------------------------------------------------------------------------
## Installation

find_path (NUM_UTIL_INSTALL_PREFIX release_area.txt
  PATHS
  ${NUM_UTIL_SOURCE_DIR}
  PATH_SUFFIXES
  ../release
  ../../release
  ../../../release
  NO_DEFAULT_PATH
  )

if (NUM_UTIL_INSTALL_PREFIX)
  message (STATUS "Installation area located for package NUM_UTIL.")
  ## Installation of the library
  install (TARGETS num_util
    DESTINATION ${NUM_UTIL_INSTALL_PREFIX}/lib
    )
  ## Installation of the header files
  install (FILES num_util.h
    DESTINATION ${NUM_UTIL_INSTALL_PREFIX}/include/python/num_util
    )
else (NUM_UTIL_INSTALL_PREFIX)
  message (STATUS "Unable to locate installation area for package NUM_UTIL.")
endif (NUM_UTIL_INSTALL_PREFIX)
