
## Name of the project
project (BLITZ)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.8)

## -----------------------------------------------------------------------------
## Only force rebuild of the library from the provided source code, if no system
## installation can be found. In order to handle this properly, we use the CMake
## find script

option (BLITZ_FORCE_BUILD       "Force rebuild and local installation?"     NO  )
option (BLITZ_CMAKE_BUILD       "Use CMake to configure and build Blitz++?" YES )
option (BLITZ_VERBOSE_CONFIGURE "Be verbose during configuration?"          NO  )
option (BLITZ_VERBOSE_BUILD     "Be verbose during build process?"          NO  )

## ------------------------------------------------------------------------------
## Set CMAKE_MODULE_PATH to load custom CMake modules

find_path (USG_ROOT devel_common/cmake/CMakeSettings.cmake
  PATHS 
  ${BLITZ_SOURCE_DIR}
  ${BLITZ_SOURCE_DIR}/..
  ${BLITZ_SOURCE_DIR}/../..
  ${BLITZ_SOURCE_DIR}/../../..
  $ENV{LOFARSOFT}
  )

if (USG_ROOT)
  include (${USG_ROOT}/devel_common/cmake/CMakeSettings.cmake)
else (USG_ROOT)
  message (FATAL_ERROR "Unable to locate additional CMake scripts!")
endif (USG_ROOT)

## ------------------------------------------------------------------------------
## Check if the program/package/library is already available

if (NOT BLITZ_VERBOSE_CONFIGURE)
  set (BLITZ_FIND_QUIETLY  YES )
endif (NOT BLITZ_VERBOSE_CONFIGURE)

include (FindBlitz)

## <-- begin build condition --------------------------------------------------->

if (NOT HAVE_BLITZ OR BLITZ_FORCE_BUILD)
  
  ##__________________________________________________________________
  ## Build the project
  
  include (ExternalProject)
  
  find_file (have_blitz_tgz blitz-0.9.tar.gz ${BLITZ_SOURCE_DIR})
  
  if (have_blitz_tgz)
    ExternalProject_Add (blitz
      PREFIX ${BLITZ_BINARY_DIR}
      DOWNLOAD_DIR ${BLITZ_BINARY_DIR}/blitz
      SOURCE_DIR ${BLITZ_BINARY_DIR}/blitz
      URL ${have_blitz_tgz}
      BUILD_IN_SOURCE 1
      CONFIGURE_COMMAND ${BLITZ_BINARY_DIR}/blitz/configure --prefix=${USG_INSTALL_PREFIX}
      )
  else (have_blitz_tgz)
    status (WARNING "[Blitz++] Unable to build project - no sources found!")
  endif (have_blitz_tgz)
  
  ## <-- end build condition ----------------------------------------------------->
  
else (NOT HAVE_BLITZ OR BLITZ_FORCE_BUILD)
  message (STATUS "[Blitz++] Found system-wide installation; skipping rebuild!")
endif (NOT HAVE_BLITZ OR BLITZ_FORCE_BUILD)
