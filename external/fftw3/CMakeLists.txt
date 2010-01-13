
## Name of the project
project (FFTW3)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.8)

## -----------------------------------------------------------------------------
## Only force rebuild of the library from the provided source code, if no system
## installation can be found. In order to handle this properly, we use the CMake
## find script

option (FFTW3_FORCE_BUILD       "Force rebuild and local installation?"    NO  )
option (FFTW3_CMAKE_BUILD       "Use CMake to configure and build FFTW3?"  YES )
option (FFTW3_VERBOSE_CONFIGURE "Be verbose during configuration?"         NO  )
option (FFTW3_VERBOSE_BUILD     "Be verbose during build process?"         NO  )

## ------------------------------------------------------------------------------
## Set CMAKE_MODULE_PATH to load custom CMake modules

find_path (USG_ROOT devel_common/cmake/CMakeSettings.cmake
  PATHS 
  ${FFTW3_SOURCE_DIR}
  ${FFTW3_SOURCE_DIR}/..
  ${FFTW3_SOURCE_DIR}/../..
  ${FFTW3_SOURCE_DIR}/../../..
  $ENV{LOFARSOFT}
  )

if (USG_ROOT)
  include (${USG_ROOT}/devel_common/cmake/CMakeSettings.cmake)
else (USG_ROOT)
  message (FATAL_ERROR "[FFTW3] Unable to locate additional CMake scripts!")
endif (USG_ROOT)

## ------------------------------------------------------------------------------
## Check if the program/package/library is already available

set (FFTW3_FIND_QUIETLY YES)

include (FindFFTW3)

## <-- begin build condition --------------------------------------------------->

if (NOT HAVE_FFTW3 OR FFTW3_FORCE_BUILD)
  
  include (ExternalProject)
  
  find_file (have_fftw3_tgz fftw-3.2.2.tar.gz  ${FFTW3_SOURCE_DIR})
  
  if (have_fftw3_tgz)
    ExternalProject_Add (fftw3
      PREFIX ${FFTW3_BINARY_DIR}
      DOWNLOAD_DIR ${FFTW3_BINARY_DIR}
      SOURCE_DIR ${FFTW3_BINARY_DIR}
      URL ${have_fftw3_tgz}
      BUILD_IN_SOURCE 1
      CONFIGURE_COMMAND ${FFTW3_BINARY_DIR}/configure --prefix=${USG_INSTALL_PREFIX}
      )
  else (have_fftw3_tgz)
    ExternalProject_Add (fftw3
      PREFIX ${FFTW3_BINARY_DIR}
      DOWNLOAD_DIR ${FFTW3_BINARY_DIR}
      SOURCE_DIR ${FFTW3_BINARY_DIR}
      URL ftp://ftp.fftw.org/pub/fftw/fftw-3.2.2.tar.gz
      BUILD_IN_SOURCE 1
      CONFIGURE_COMMAND ${FFTW3_BINARY_DIR}/configure --prefix=${USG_INSTALL_PREFIX}
      )
  endif (have_fftw3_tgz)
  
  ## <-- end build condition ----------------------------------------------------->
  
else (NOT HAVE_FFTW3 OR FFTW3_FORCE_BUILD)
  add_custom_target (fftw3
    WORKING_DIRECTORY ${FFTW3_BINARY_DIR}
    COMMENT "[FFTW3] Found system-wide installation; skipping rebuild!"
    )
endif (NOT HAVE_FFTW3 OR FFTW3_FORCE_BUILD)
