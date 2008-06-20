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

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.4)

## -----------------------------------------------------------------------------
## Optional user settings

option (ROOT_FORCE_INSTALL "Force installation from source package?" 0)

## ------------------------------------------------------------------------------
## Set CMAKE_MODULE_PATH to load custom CMake modules

find_path (USG_ROOT devel_common/cmake/CMakeSettings.cmake
  PATHS 
  ${ROOT_SOURCE_DIR}
  ${ROOT_SOURCE_DIR}/..
  ${ROOT_SOURCE_DIR}/../..
  ${ROOT_SOURCE_DIR}/../../..
  ENV LOFARSOFT
  )

if (USG_ROOT)
  include (${USG_ROOT}/devel_common/cmake/CMakeSettings.cmake)
else (USG_ROOT)
  message (FATAL_ERROR "Unable to locate additional CMake scripts!")
endif (USG_ROOT)

## -----------------------------------------------------------------------------
## [2] Run the find script to see if there is a system wide ROOT installation

include (${cmake_scripts}/FindROOT.cmake)

## -----------------------------------------------------------------------------
## [3] If no system-wide ROOT installation could be found, trigger (at least a
##     local) installation.

if (NOT HAVE_ROOT OR ROOT_FORCE_INSTALL)
  include (${cmake_scripts}/InstallROOT.cmake)
endif (NOT HAVE_ROOT OR ROOT_FORCE_INSTALL)
