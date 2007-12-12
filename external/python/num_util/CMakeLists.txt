##------------------------------------------------------------------------------
## $Id::                                                                       $
##------------------------------------------------------------------------------

## -----------------------------------------------------------------------------
## Project name and version number number

## Name of the project
project (NUM_UTIL)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.4)

## -----------------------------------------------------------------------------
## Check for required external libraries

## Locate additional CMake find scripts

find_path (NUM_UTIL_cmake FindBoost.cmake FindPython.cmake FindNumarray.cmake
  PATHS 
  ${NUM_UTIL_SOURCE_DIR}
  ${NUM_UTIL_SOURCE_DIR}/..
  ${NUM_UTIL_SOURCE_DIR}/../..
  ${NUM_UTIL_SOURCE_DIR}/../../..
  PATH_SUFFIXES
  devel_common/cmake
  )

## locate external packages

if (NUM_UTIL_cmake)
  ## --- Boost -----------------------------------
  set (BOOST_FIND_QUIETLY 1)
  include (${NUM_UTIL_cmake}/FindBoost.cmake)
  ## --- Python ----------------------------------
  set (PYTHON_FIND_QUIETLY 1)
  include (${NUM_UTIL_cmake}/FindPython.cmake)
else (NUM_UTIL_cmake)
  message (SEND_ERROR "[NUM_UTIL] Unable to locate CMake find scripts!")
endif (NUM_UTIL_cmake)

## Set location of the header files

if (BOOST_INCLUDES)
  include_directories (${BOOST_INCLUDES})
endif (BOOST_INCLUDES)

if (PYTHON_INCLUDES)
  include_directories (${PYTHON_INCLUDES})
endif (PYTHON_INCLUDES)

if (NUMPY_INCLUDES)
  include_directories (${NUMPY_INCLUDES})
endif (NUMPY_INCLUDES)

## -----------------------------------------------------------------------------
## Compilation targets
#
# all: num_py_test.py simpletest.py
# num_py_test.py : num_test_ext.so
# 	$(PYTHON_DIR)/bin/python num_py_test.py
# simpletest.py : simple_ext.so
# 	$(PYTHON_DIR)/bin/python simpletest.py
# num_test_ext.so: num_util.o num_test.o num_test_ext.o
# num_test_ext.o : num_test_ext.cpp num_test.h
# simple_ext.so : num_util.o simpletest.o
# 	g++ -$(LDFLAGS) -o $@  $(LIBDIRS)  $(PYLIBS)  $^
# num_test.o  : num_test.cpp num_util.o num_test.h
# num_util.o  : num_util.cpp  num_util.h
# simpletest.o  : num_util.o num_util.h simpletest.cpp

include_directories (
  ${NUM_UTIL_SOURCE_DIR}/../../../release/include
  ${NUM_UTIL_SOURCE_DIR}/../../../release/lib/python2.5/site-packages/numpy/core/include
  ${NUM_UTIL_SOURCE_DIR}/../../../release/lib/python2.4/site-packages/numpy/core/include
  )

add_definitions (-fPIC)

add_library (num_util
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
