
## Name of the project
project (ROOT)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.8)

## -----------------------------------------------------------------------------
## Only force rebuild of the library from the provided source code, if no system
## installation can be found. In order to handle this properly, we use the CMake
## find script

option (ROOT_FORCE_BUILD       "Force rebuild and local installation?"    NO  )
option (ROOT_CMAKE_BUILD       "Use CMake to configure and build ROOT?"  YES )
option (ROOT_VERBOSE_CONFIGURE "Be verbose during configuration?"         NO  )
option (ROOT_VERBOSE_BUILD     "Be verbose during build process?"         NO  )

## ------------------------------------------------------------------------------
## Set CMAKE_MODULE_PATH to load custom CMake modules

find_path (USG_ROOT devel_common/cmake/CMakeSettings.cmake
  PATHS 
  ${ROOT_SOURCE_DIR}
  ${ROOT_SOURCE_DIR}/..
  ${ROOT_SOURCE_DIR}/../..
  ${ROOT_SOURCE_DIR}/../../..
  $ENV{LOFARSOFT}
  )

if (USG_ROOT)
  include (${USG_ROOT}/devel_common/cmake/CMakeSettings.cmake)
else (USG_ROOT)
  message (FATAL_ERROR "[ROOT] Unable to locate additional CMake scripts!")
endif (USG_ROOT)

## ------------------------------------------------------------------------------
## Check if the program/package/library is already available

set (ROOT_FIND_QUIETLY YES)

include (FindROOT)

## <-- begin build condition --------------------------------------------------->

if (NOT HAVE_ROOT OR ROOT_FORCE_BUILD)
  
  include (ExternalProject)
  
  if (UNIX)
    if (APPLE)
      set (root_architecture "macosx")
    else (APPLE)
      set (root_architecture "linux")
    endif (APPLE)
  else (UNIX)
    if (WIN32)
      set (root_architecture "win32gcc")
    endif (WIN32)
  endif (UNIX)
  
  ExternalProject_Add (root
    PREFIX ${ROOT_BINARY_DIR}
    DOWNLOAD_DIR ${ROOT_BINARY_DIR}/root
    SOURCE_DIR ${ROOT_BINARY_DIR}/root
    SVN_REPOSITORY http://root.cern.ch/svn/root/trunk
    CONFIGURE_COMMAND ${ROOT_BINARY_DIR}/root/configure ${root_architecture} --prefix=${USG_INSTALL_PREFIX}
    )
  
  ## <-- end build condition ----------------------------------------------------->
  
else (NOT HAVE_ROOT OR ROOT_FORCE_BUILD)
  add_custom_target (root
    WORKING_DIRECTORY ${ROOT_BINARY_DIR}
    COMMENT "[ROOT] Found system-wide installation; skipping rebuild!"
    )
endif (NOT HAVE_ROOT OR ROOT_FORCE_BUILD)
