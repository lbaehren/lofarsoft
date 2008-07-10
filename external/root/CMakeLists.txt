##------------------------------------------------------------------------------
## $Id:: CMakeLists.txt 527 2007-08-09 17:47:37Z baehren                       $
##------------------------------------------------------------------------------

## =============================================================================
##
## Main CMake configuration file for building the "ROOT" package.
##
## =============================================================================

## Project name
project (ROOT)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.5)

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

## ------------------------------------------------------------------------------
## [2] Run the find script to see if there is a system wide ROOT installation

include (FindROOT)

## <-- begin build condition --------------------------------------------------->

if (NOT HAVE_ROOT OR ROOT_FORCE_INSTALL)
  
  ## ----------------------------------------------------------------------------
  ## URL : ftp://root.cern.ch/root/root_v5.16.00.source.tar.gz
  
  set (root_version_major 5)
  set (root_version_minor 16)
  set (root_version_patch 00)
  set (root_version "${root_version_major}.${root_version_minor}.${root_version_patch}")
  
  set (root_source_dir "root")
  set (root_source_file "root_v${root_version}.source.tar.gz")
  
  set (root_url "ftp://root.cern.ch/root/${root_source_file}")
  
  ## ----------------------------------------------------------------------------
  ## Locate required tools
  
  find_program (CURL_EXECUTABLE curl PATHS ${bin_locations})
  find_program (TAR_EXECUTABLE tar PATHS ${bin_locations})
  find_program (WGET_EXECUTABLE wget PATHS ${bin_locations})  

  ## ----------------------------------------------------------------------------
  ## Download the source tar-ball from the ROOT FTP server
  
  if (WGET_EXECUTABLE)
    execute_process (
      WORKING_DIRECTORY .
      COMMAND ${WGET_EXECUTABLE} -c ${root_url}
      TIMEOUT 300
      ERROR_VARIABLE wget_cerr
      OUTPUT_QUIET
      )
    if (wget_cerr)
      message (STATUS "[wget] Unable to download source package!")
      message (STATUS "${wget_cerr}")
    endif (wget_cerr)
  elseif (WGET_EXECUTABLE)
    message (STATUS "Missing wget to download source tar-ball!")
  endif (WGET_EXECUTABLE)

  ## ----------------------------------------------------------------------------
  ## Check of the download was successfull; if this is the case we can expand
  ## the source tar-ball

  find_file (HAVE_ROOT_SOURCE ${root_source_file} PATHS . ..)  
  
  if (TAR_EXECUTABLE AND HAVE_ROOT_SOURCE)
    message (STATUS "Unpacking source package ${HAVE_ROOT_SOURCE} ...")
    execute_process (
      WORKING_DIRECTORY .
      COMMAND ${TAR_EXECUTABLE} -xvzf ${HAVE_ROOT_SOURCE}
      TIMEOUT 120
      ERROR_VARIABLE tar_cerr
      OUTPUT_QUIET
      )
    if (tar_cerr)
      message (STATUS "[tar] Error expanding the source package!")
      message (STATUS ${tar_cerr})
    endif (tar_cerr)
  endif (TAR_EXECUTABLE AND HAVE_ROOT_SOURCE)  

  ## ----------------------------------------------------------------------------
  
  ## <-- end build condition --------------------------------------------------->
  
endif (NOT HAVE_ROOT OR ROOT_FORCE_INSTALL)
