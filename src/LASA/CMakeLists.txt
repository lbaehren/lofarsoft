##------------------------------------------------------------------------------
## $Id:: CMakeLists.txt 4104 2010-02-03 09:36:53Z swinbank                     $
##------------------------------------------------------------------------------

## Name of the project
project (LASA)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.6)

## ==============================================================================
##
##  CMake configuration settings
##
## ==============================================================================

find_path (USG_ROOT devel_common/cmake/CMakeSettings.cmake
  PATHS 
  ${LASA_SOURCE_DIR}
  ${LASA_SOURCE_DIR}/..
  ${LASA_SOURCE_DIR}/../..
  ${LASA_SOURCE_DIR}/../../..
  $ENV{LOFARSOFT}
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

option (LASA_ENABLE_TESTING     "Build the test programs?"                   YES )
option (LASA_COMPILER_WARNINGS  "Enable standard set of compiler warnings?"  ON  )
option (LASA_DEBUGGING_MESSAGES "Print debugging information?"               NO  )
option (LASA_VERBOSE_CONFIGURE  "Verbose output during configuration?"       NO  )

##__________________________________________________________
## Handle option: testing mode on/off

if (LASA_ENABLE_TESTING)
  ## Include CTest module
  include (CTest)
  enable_testing()
endif (LASA_ENABLE_TESTING)

##__________________________________________________________
## Handle option: verbose mode during configuration on/off

if (NOT LASA_VERBOSE_CONFIGURE)
  set (HDF5_FIND_QUIETLY YES)
  set (ROOT_FIND_QUIETLY YES)
endif (NOT LASA_VERBOSE_CONFIGURE)

##__________________________________________________________
## Handle option: debugging messages on/off

if (LASA_DEBUGGING_MESSAGES)
  add_definitions (-DDEBUGGING_MESSAGES)
endif (LASA_DEBUGGING_MESSAGES)

## ==============================================================================
##
##  Include directories and libraries
##
## ==============================================================================

include (CPack)

set (NO_DEFAULT_PATH YES)
set (lasa_link_libraries "")

include_directories (
  ${LASA_SOURCE_DIR}
  ${LASA_SOURCE_DIR}/analysis
  ${LASA_SOURCE_DIR}/implement
  ${LASA_SOURCE_DIR}/external/ftd2xx
  )

##__________________________________________________________
## Type sizes

if (SIZEOF_LONG_LONG)
  add_definitions (-DHAVE_LONG_LONG)
endif (SIZEOF_LONG_LONG)

if (CMAKE_SIZEOF_VOID_P)
  if (${CMAKE_SIZEOF_VOID_P} EQUAL 8)
    add_definitions (-DWORDSIZE_IS_64)
  endif (${CMAKE_SIZEOF_VOID_P} EQUAL 8)
endif (CMAKE_SIZEOF_VOID_P)

##__________________________________________________________
##                                                      HDF5

if (NOT HAVE_HDF5)
  include (FindHDF5)
endif (NOT HAVE_HDF5)

if (HAVE_HDF5)
  ## compiler flag
  add_definitions (-DHAVE_HDF5)
  ## include path
  include_directories (${HDF5_INCLUDES})
  ## link library
  list (APPEND lasa_link_libraries ${HDF5_LIBRARIES})
else (HAVE_HDF5)
  message (MESSAGE "[LASA] Missing HDF5")
endif (HAVE_HDF5)

##__________________________________________________________
##                                                      ROOT

if (NOT HAVE_ROOT)
  include (FindROOT)
endif (NOT HAVE_ROOT)

if (HAVE_ROOT)
  ## compiler flag
  add_definitions (-DHAVE_ROOT)
  ## include path
  include_directories (${ROOT_INCLUDES})
  ## link library
  list (APPEND lasa_link_libraries ${ROOT_LIBRARIES})
else (HAVE_ROOT)
  message (FATAL_ERROR "[LASA] Missing ROOT - unable to proceeed!")
endif (HAVE_ROOT)

## =============================================================================
##
##  Installation
##
## =============================================================================

find_path (prefix release_area.txt
  PATHS
  ${LASA_SOURCE_DIR}
  PATH_SUFFIXES
  ../release
  ../../release
  NO_DEFAULT_PATH
  )

if (prefix)
  message (STATUS "Installation area located for package LASA.")
  set (CMAKE_INSTALL_PREFIX ${prefix}) 
endif (prefix)

# skip the full RPATH for the build tree
set (CMAKE_SKIP_BUILD_RPATH YES)

# when building, use the install RPATH already
# (so it doesn't need to relink when installing)
set (CMAKE_BUILD_WITH_INSTALL_RPATH YES)

# the RPATH to be used when installing
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")

# add the automatically determined parts of the RPATH
# which point to directories outside the build tree to the install RPATH
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH YES)

## =============================================================================
##
##  Instructions for build and installation
##
## =============================================================================

add_subdirectory (external)
add_subdirectory (implement)
add_subdirectory (apps)

if (LASA_ENABLE_TESTING)
  add_subdirectory (test)
endif (LASA_ENABLE_TESTING)

## =============================================================================
##
##  Feedback
##
## =============================================================================

message (STATUS "+============================================================+")
message (STATUS "| LASA: Summary of configuration settings                     |")
message (STATUS "+------------------------------------------------------------+")
message (STATUS " Python version             = ${PYTHON_VERSION}"        )
message (STATUS " Print debugging messages   = ${LASA_DEBUGGING_MESSAGES}")
message (STATUS " HDF5 includes              = ${HDF5_INCLUDES}")
message (STATUS " HDF5 library               = ${HDF5_LIBRARIES}")
message (STATUS "+------------------------------------------------------------+")
