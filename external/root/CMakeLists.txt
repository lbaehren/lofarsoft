##------------------------------------------------------------------------------
## $Id:: CMakeLists.txt 527 2007-08-09 17:47:37Z baehren                       $
##------------------------------------------------------------------------------

## =============================================================================
##
## Main CMake configuration file for building the "ROOT" package.
##
## =============================================================================

message (STATUS "[CMake package configuration] ROOT")

project (ROOT)

## -----------------------------------------------------------------------------
## [1] Locate the CMake find scripts

find_path (cmake_scripts FindROOT.cmake InstallROOT.cmake
  PATHS
  ..
  ../..
  PATH_SUFFIXES
  devel_common/cmake
)

if (cmake_scripts)
  message (STATUS "Found CMake script directory : ${cmake_scripts}")
elseif (cmake_scripts)
  message (FATAL_ERROR "Cannot find CMake script directory; unable to continue!")
endif (cmake_scripts)

## -----------------------------------------------------------------------------
## [2] Run the find script to see if there is a system wide ROOT installation

include (${cmake_scripts}/FindROOT.cmake)

## -----------------------------------------------------------------------------
## [3] If no system-wide ROOT installation could be found, trigger (at least a
##     local) installation.

if (NOT HAVE_ROOT)
  include (${cmake_scripts}/InstallROOT.cmake)
endif (NOT HAVE_ROOT)