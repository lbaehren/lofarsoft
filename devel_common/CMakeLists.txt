##------------------------------------------------------------------------------
## $Id:: CMakeLists.txt 528 2007-08-09 17:53:34Z baehren                       $
##------------------------------------------------------------------------------

## Name of the project
project (devel_common)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.5)

## -----------------------------------------------------------------------------
## Installation

find_path (prefix release_area.txt
  PATHS
  ${devel_common_SOURCE_DIR}
  PATH_SUFFIXES
  ../release
  ../../release
  NO_DEFAULT_PATH
  )

if (prefix)
  message (STATUS "Installation area located for package devel_common.")
  set (CMAKE_INSTALL_PREFIX ${prefix}) 
endif (prefix)

##------------------------------------------------------------------------------
## Installation of shell scripts

install (
  FILES scripts/newclass
  DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
  PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE
  )

