##------------------------------------------------------------------------------
## $Id$
##------------------------------------------------------------------------------

## -----------------------------------------------------------------------------
## Project name and version number number

## Name of the project
project (NUM_UTIL)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.4)

set (NUM_UTIL_VERSION_MAJOR 2)
set (NUM_UTIL_VERSION_MINOR 1)
set (NUM_UTIL_VERSION "${WCSLIB_VERSION_MAJOR}.${WCSLIB_VERSION_MINOR}")

## -----------------------------------------------------------------------------
## Standard locations where to look for required components

set (bin_locations
  /usr/bin
  /usr/local/bin
  /sw/bin
  ${NUM_UTIL_SOURCE_DIR}/../release/bin
  ${NUM_UTIL_SOURCE_DIR}/../../release/bin
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

##include (CheckFunctionExists)
##include (CheckIncludeFiles)
##include (CheckLibraryExists)
include (CPack)

## -----------------------------------------------------------------------------
## Check for required external libraries

## Locate additional CMake find scripts

find_path (NUM_UTIL_cmake FindCFITSIO.cmake FindG2C.cmake FindLAPACK.cmake
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
  ## Boost
  set (BOOST_FIND_QUIETLY 1)
  include (${NUM_UTIL_cmake}/FindBoost.cmake)
  ## Python
  set (PYTHON_FIND_QUIETLY 1)
  include (${NUM_UTIL_cmake}/FindPython.cmake)
else (NUM_UTIL_cmake)
  message (SEND_ERROR "Unable to locate additional CMake find scripts!")
endif (NUM_UTIL_cmake)

## -----------------------------------------------------------------------------
## Installation

find_path (NUM_UTIL_INSTALL_PREFIX release_area.txt
  PATHS
  ${NUM_UTIL_SOURCE_DIR}
  PATH_SUFFIXES
  ../release
  ../../release
  NO_DEFAULT_PATH
  )

if (NUM_UTIL_INSTALL_PREFIX)
  message (STATUS "Installation area located for package NUM_UTIL.")
  set (CMAKE_INSTALL_PREFIX ${NUM_UTIL_INSTALL_PREFIX}) 
endif (NUM_UTIL_INSTALL_PREFIX)

## -----------------------------------------------------------------------------
## Summary of settings

#message (STATUS "CFITSIO ............... = ${HAVE_CFITSIO}")


## -----------------------------------------------------------------------------
## Compilation targets

set (NUM_UTIL_sources num_util.cpp)
set (NUM_UTIL_headers num_util.h)

include_directories (
  ${BOOST_INCLUDES}/..
  ${PYTHON_INCLUDES}
  ${PYTHON_INCLUDES}/../../lib/python2.5/site-packages/numpy/core/include
)

add_library (num_util STATIC ${NUM_UTIL_sources})

install (TARGETS num_util DESTINATION lib)
install (FILES ${NUM_UTIL_headers} DESTINATION include)

## this is how one should use it
#add_library (simple_ext MODULE simpletest.cpp)
#target_link_libraries (simple_ext num_util ${BOOST_boost_python})
#set_target_properties (simple_ext PROPERTIES PREFIX "")
#set_target_properties(simple_ext PROPERTIES LINK_FLAGS "-bundle_loader ${PYTHON_EXECUTABLE}")
