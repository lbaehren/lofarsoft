
## Name of the project
project (NUM_UTIL)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.6)

## -----------------------------------------------------------------------------
## Only force rebuild of the library from the provided source code, if no system
## installation can be found. In order to handle this properly, we use the CMake
## find script

option (NUM_UTIL_FORCE_BUILD       "Force rebuild and local installation?"    NO  )
option (NUM_UTIL_CMAKE_BUILD       "Use CMake to configure and build NUM_UTIL?"  YES )
option (NUM_UTIL_VERBOSE_CONFIGURE "Be verbose during configuration?"         NO  )
option (NUM_UTIL_VERBOSE_BUILD     "Be verbose during build process?"         NO  )

## ------------------------------------------------------------------------------
## Set CMAKE_MODULE_PATH to load custom CMake modules

find_path (USG_ROOT devel_common/cmake/CMakeSettings.cmake
  PATHS 
  ${NUM_UTIL_SOURCE_DIR}
  ${NUM_UTIL_SOURCE_DIR}/..
  ${NUM_UTIL_SOURCE_DIR}/../..
  ${NUM_UTIL_SOURCE_DIR}/../../..
  $ENV{LOFARSOFT}
  )

if (USG_ROOT)
  include (${USG_ROOT}/devel_common/cmake/CMakeSettings.cmake)
else (USG_ROOT)
  message (FATAL_ERROR "Unable to locate additional CMake scripts!")
endif (USG_ROOT)

## ------------------------------------------------------------------------------
## Check build conditions

if (NOT NUM_UTIL_VERBOSE_CONFIGURE)
  set (BOOST_FIND_QUIETLY  YES )
  set (NUMPY_FIND_QUIETLY  YES )
  set (PYTHON_FIND_QUIETLY YES )
endif (NOT NUM_UTIL_VERBOSE_CONFIGURE)

include (FindBoost)
include (FindNumPy)
include (FindNumUtil)

if (NOT HAVE_BOOST)
  message (FATAL_ERROR "[num_util] Unable to continue -- missing Boost!")
endif (NOT HAVE_BOOST)

if (NOT HAVE_NUMPY)
  message (FATAL_ERROR "[num_util] Unable to continue -- missing NumPy!")
endif (NOT HAVE_NUMPY)

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
  )

if (NUMPY_INCLUDES)
  include_directories (
    ${NUMPY_INCLUDES}
    ${NUMPY_INCLUDES}/numpy
    )
endif (NUMPY_INCLUDES)

add_definitions (
  -fPIC
  )

add_library (num_util STATIC
  num_util.cpp
  )

target_link_libraries (num_util
  ${BOOST_LIBRARIES}
  )

## -----------------------------------------------------------------------------
## Installation

## Installation of the library
install (TARGETS num_util
  DESTINATION lib
  )

## Installation of the header files
install (FILES num_util.h
  DESTINATION include/python/num_util
  )
