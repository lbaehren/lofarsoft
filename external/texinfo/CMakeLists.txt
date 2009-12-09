
## Name of the project
project (TEXINFO)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.8)

## -----------------------------------------------------------------------------
## Only force rebuild of the library from the provided source code, if no system
## installation can be found. In order to handle this properly, we use the CMake
## find script

option (TEXINFO_FORCE_BUILD       "Force rebuild and local installation?"    NO  )
option (TEXINFO_CMAKE_BUILD       "Use CMake to configure and build TEXINFO?"  YES )
option (TEXINFO_VERBOSE_CONFIGURE "Be verbose during configuration?"         NO  )
option (TEXINFO_VERBOSE_BUILD     "Be verbose during build process?"         NO  )

## ------------------------------------------------------------------------------
## Set CMAKE_MODULE_PATH to load custom CMake modules

find_path (USG_ROOT devel_common/cmake/CMakeSettings.cmake
  PATHS 
  ${TEXINFO_SOURCE_DIR}
  ${TEXINFO_SOURCE_DIR}/..
  ${TEXINFO_SOURCE_DIR}/../..
  ${TEXINFO_SOURCE_DIR}/../../..
  $ENV{LOFARSOFT}
  )

if (USG_ROOT)
  include (${USG_ROOT}/devel_common/cmake/CMakeSettings.cmake)
else (USG_ROOT)
  message (FATAL_ERROR "[TEXINFO] Unable to locate additional CMake scripts!")
endif (USG_ROOT)

## ------------------------------------------------------------------------------
## Check if the program/package/library is already available

#include (FindTEXINFO)

## <-- begin build condition --------------------------------------------------->

if (NOT HAVE_TEXINFO OR TEXINFO_FORCE_BUILD)
  
  include (ExternalProject)
  
  set (texinfo_version "4.9")
  find_file (have_texinfo_tgz texinfo-${texinfo_version}.tar.gz  ${TEXINFO_SOURCE_DIR})
  
  if (have_texinfo_tgz)
    ExternalProject_Add (texinfo
      PREFIX ${TEXINFO_BINARY_DIR}
      DOWNLOAD_DIR ${TEXINFO_BINARY_DIR}
      SOURCE_DIR ${TEXINFO_BINARY_DIR}
      URL ${have_texinfo_tgz}
      CONFIGURE_COMMAND ${TEXINFO_BINARY_DIR}/configure --prefix=${USG_INSTALL_PREFIX}
      )
  else (have_texinfo_tgz)
    ExternalProject_Add (texinfo
      PREFIX ${TEXINFO_BINARY_DIR}
      DOWNLOAD_DIR ${TEXINFO_BINARY_DIR}
      SOURCE_DIR ${TEXINFO_BINARY_DIR}
      URL http://ftp.gnu.org/gnu/texinfo/texinfo-${texinfo_version}.tar.gz
      CONFIGURE_COMMAND ${TEXINFO_BINARY_DIR}/configure --prefix=${USG_INSTALL_PREFIX}
      )
  endif (have_texinfo_tgz)
  
  ## <-- end build condition ----------------------------------------------------->
  
else (NOT HAVE_TEXINFO OR TEXINFO_FORCE_BUILD)
  add_custom_target (texinfo
    WORKING_DIRECTORY ${TEXINFO_BINARY_DIR}
    COMMENT "[TEXINFO] Found system-wide installation; skipping rebuild!"
    )
endif (NOT HAVE_TEXINFO OR TEXINFO_FORCE_BUILD)
