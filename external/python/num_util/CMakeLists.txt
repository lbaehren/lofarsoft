##------------------------------------------------------------------------------
## $Id::                                                                       $
##------------------------------------------------------------------------------

## -----------------------------------------------------------------------------
## Project name and version number number

## Name of the project
project (PYTHON_NUM_UTIL)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.4)

## -----------------------------------------------------------------------------
## Set CMAKE_MODULE_PATH to load custom CMake modules

find_path (USG_ROOT devel_common/cmake/CMakeSettings.cmake
  PATHS 
  ${PYTHON_NUM_UTIL_SOURCE_DIR}
  ${PYTHON_NUM_UTIL_SOURCE_DIR}/..
  ${PYTHON_NUM_UTIL_SOURCE_DIR}/../..
  ${PYTHON_NUM_UTIL_SOURCE_DIR}/../../..
  ${PYTHON_NUM_UTIL_SOURCE_DIR}/../../../..
  ENV LOFARSOFT
  )

if (USG_ROOT)
  ## load common CMake definitions
  include (${USG_ROOT}/devel_common/cmake/CMakeSettings.cmake)
  ## try locating required external packages
  set (PYTHON_FIND_QUIETLY  YES)
  include (FindPython)
else (USG_ROOT)
  message (STATUS "Unable to locate additional CMake scripts!")
endif (USG_ROOT)

## -----------------------------------------------------------------------------
## locate external packages

set (BOOST_FIND_QUIETLY  YES )
set (PYTHON_FIND_QUIETLY YES )

include (FindBoost)
include (FindPython)

if (NOT HAVE_BOOST)
  message (FATAL_ERROR "[num_util] Missing Boost.Python! Unable to build.")
endif (NOT HAVE_BOOST)

## search for local version of numpy header files

find_path (arrayobject_h numpy/arrayobject.h
  PATHS
  ${PYTHON_MODULES_SOURCE_DIR}
  ${PYTHON_NUM_UTIL_SOURCE_DIR}/..
  PATH_SUFFIXES
  numpy/numpy/core/include
  numpy/numpy/core/include/numpy
  )

if (arrayobject_h)
  list (APPEND NUMPY_INCLUDES ${arrayobject_h})
endif (arrayobject_h)

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

find_path (PYTHON_NUM_UTIL_INSTALL_PREFIX release_area.txt
  PATHS
  ${PYTHON_NUM_UTIL_SOURCE_DIR}
  PATH_SUFFIXES
  ../release
  ../../release
  ../../../release
  NO_DEFAULT_PATH
  )

if (PYTHON_NUM_UTIL_INSTALL_PREFIX)
  message (STATUS "Installation area located for package PYTHON_NUM_UTIL.")
  ## Installation of the library
  install (TARGETS num_util
    DESTINATION ${PYTHON_NUM_UTIL_INSTALL_PREFIX}/lib
    )
  ## Installation of the header files
  install (FILES num_util.h
    DESTINATION ${PYTHON_NUM_UTIL_INSTALL_PREFIX}/include/python/num_util
    )
else (PYTHON_NUM_UTIL_INSTALL_PREFIX)
  message (STATUS "Unable to locate installation area for package PYTHON_NUM_UTIL.")
endif (PYTHON_NUM_UTIL_INSTALL_PREFIX)

## -----------------------------------------------------------------------------
## Configuration summary

message (STATUS "+------------------------------------------------------------+")
message (STATUS "[num_util] Configuration summary.")
message (STATUS "+------------------------------------------------------------+")
message (STATUS "BOOST_INCLUDES  = ${BOOST_INCLUDES}")
message (STATUS "PYTHON_INCLUDES = ${PYTHON_INCLUDES}")
message (STATUS "NUMPY_INCLUDES  = ${NUMPY_INCLUDES}")
message (STATUS "+------------------------------------------------------------+")
