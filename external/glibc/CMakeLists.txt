
## Name of the project
project (GLIBC)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.8)

## -----------------------------------------------------------------------------
## Only force rebuild of the library from the provided source code, if no system
## installation can be found. In order to handle this properly, we use the CMake
## find script

option (GLIBC_FORCE_BUILD       "Force rebuild and local installation?"    NO  )
option (GLIBC_CMAKE_BUILD       "Use CMake to configure and build GLIBC?"  YES )
option (GLIBC_VERBOSE_CONFIGURE "Be verbose during configuration?"         NO  )
option (GLIBC_VERBOSE_BUILD     "Be verbose during build process?"         NO  )

## ------------------------------------------------------------------------------
## Set CMAKE_MODULE_PATH to load custom CMake modules

find_path (USG_ROOT devel_common/cmake/CMakeSettings.cmake
  PATHS 
  ${GLIBC_SOURCE_DIR}
  ${GLIBC_SOURCE_DIR}/..
  ${GLIBC_SOURCE_DIR}/../..
  ${GLIBC_SOURCE_DIR}/../../..
  $ENV{LOFARSOFT}
  )

if (USG_ROOT)
  include (${USG_ROOT}/devel_common/cmake/CMakeSettings.cmake)
else (USG_ROOT)
  message (FATAL_ERROR "[GLIBC] Unable to locate additional CMake scripts!")
endif (USG_ROOT)

## ------------------------------------------------------------------------------
## Check if the program/package/library is already available

#include (FindGLIBC)

## <-- begin build condition --------------------------------------------------->

if (NOT HAVE_GLIBC OR GLIBC_FORCE_BUILD)
  
  include (ExternalProject)
  
  set (glibc_version "2.9")
  find_file (have_glibc_tgz glibc-${glibc_version}.tar.gz  ${GLIBC_SOURCE_DIR})
  
  if (have_glibc_tgz)
    ExternalProject_Add (glibc
      PREFIX ${GLIBC_BINARY_DIR}
      DOWNLOAD_DIR ${GLIBC_BINARY_DIR}
      SOURCE_DIR ${GLIBC_BINARY_DIR}
      URL ${have_glibc_tgz}
      CONFIGURE_COMMAND ${GLIBC_BINARY_DIR}/configure --prefix=${USG_INSTALL_PREFIX}
      )
  else (have_glibc_tgz)
    ExternalProject_Add (glibc
      PREFIX ${GLIBC_BINARY_DIR}
      DOWNLOAD_DIR ${GLIBC_BINARY_DIR}
      SOURCE_DIR ${GLIBC_BINARY_DIR}
      URL http://ftp.gnu.org/gnu/glibc/glibc-${glibc_version}.tar.gz
      CONFIGURE_COMMAND ${GLIBC_BINARY_DIR}/configure --prefix=${USG_INSTALL_PREFIX}
      )
  endif (have_glibc_tgz)
  
  ## <-- end build condition ----------------------------------------------------->
  
else (NOT HAVE_GLIBC OR GLIBC_FORCE_BUILD)
  add_custom_target (glibc
    WORKING_DIRECTORY ${GLIBC_BINARY_DIR}
    COMMENT "[GLIBC] Found system-wide installation; skipping rebuild!"
    )
endif (NOT HAVE_GLIBC OR GLIBC_FORCE_BUILD)
