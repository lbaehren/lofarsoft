
## ==============================================================================
##
##  CMake project settings
##
## ==============================================================================

## Name of the project handled by CMake
project (NUM_UTIL)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.8)

## Enforced CMake policy 
cmake_policy (VERSION 2.8)

## ==============================================================================
##
##  Options
##
## ==============================================================================

option (NUM_UTIL_FORCE_BUILD       "Force rebuild and local installation?"    NO  )
option (NUM_UTIL_CMAKE_BUILD       "Use CMake to configure and build NUM_UTIL?"  YES )
option (NUM_UTIL_VERBOSE_CONFIGURE "Be verbose during configuration?"         NO  )
option (NUM_UTIL_VERBOSE_BUILD     "Be verbose during build process?"         NO  )

## ------------------------------------------------------------------------------
## Set CMAKE_MODULE_PATH to load custom CMake modules

if (NOT LUS_CMAKE_MODULES)
  find_path (LUS_CMAKE_MODULES template_FindXX.cmake LUS_CMakeSettings.cmake
    PATHS 
    ${PROJECT_SOURCE_DIR}/..
    ${PROJECT_SOURCE_DIR}/../..
    ${PROJECT_SOURCE_DIR}/../../..
    $ENV{LOFARSOFT}
    PATH_SUFFIXES
    cmake
    devel_common/cmake
    )
endif (NOT LUS_CMAKE_MODULES)

if (LUS_CMAKE_MODULES)
  ## Augment CMAKE_MODULE_PATH variable
  set (CMAKE_MODULE_PATH ${LUS_CMAKE_MODULES} CACHE PATH
    "CMake module path"
    FORCE)
  ## Load custom CMake settings
  include (LUS_CMakeSettings)
endif (LUS_CMAKE_MODULES)

## ==============================================================================
##
##  System inspection
##
## ==============================================================================

if (NOT NUM_UTIL_VERBOSE_CONFIGURE)
  set (BOOST_FIND_QUIETLY     YES )
  set (NUMPY_FIND_QUIETLY     YES )
  set (NUM_UTIL_FIND_QUIETLY  YES )
  set (PYTHON_FIND_QUIETLY    YES )
endif (NOT NUM_UTIL_VERBOSE_CONFIGURE)

include (FindBoost)
include (FindNumPy)
include (FindNumUtil)

## ==============================================================================
##
##  Build instructions
##
## ==============================================================================

if (HAVE_BOOST AND HAVE_NUMPY)
  
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
    ${NUMPY_INCLUDES}
    ${NUMPY_INCLUDES}/numpy
    ${PYTHON_INCLUDES}
    )
  
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
  
else (HAVE_BOOST AND HAVE_NUMPY)

  add_custom_target (install
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "[num_util] Missing required packages - skipping build!"
    )

  message (STATUS "BOOST_INCLUDES = ${BOOST_INCLUDES}")
  message (STATUS "NUMPY_INCLUDES = ${NUMPY_INCLUDES}")
  
endif (HAVE_BOOST AND HAVE_NUMPY)
