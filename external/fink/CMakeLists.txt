
## Name of the project
project (FINK)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.8)

## -----------------------------------------------------------------------------
## Only force rebuild of the library from the provided source code, if no system
## installation can be found. In order to handle this properly, we use the CMake
## find script

option (FINK_FORCE_BUILD       "Force rebuild and local installation?"    NO  )
option (FINK_CMAKE_BUILD       "Use CMake to configure and build Fink?"   YES )
option (FINK_VERBOSE_CONFIGURE "Be verbose during configuration?"         NO  )
option (FINK_VERBOSE_BUILD     "Be verbose during build process?"         NO  )

## ------------------------------------------------------------------------------
## Set CMAKE_MODULE_PATH to load custom CMake modules

find_path (USG_ROOT devel_common/cmake/CMakeSettings.cmake
  PATHS 
  ${FINK_SOURCE_DIR}
  ${FINK_SOURCE_DIR}/..
  ${FINK_SOURCE_DIR}/../..
  ${FINK_SOURCE_DIR}/../../..
  $ENV{LOFARSOFT}
  )

if (USG_ROOT)
  include (${USG_ROOT}/devel_common/cmake/CMakeSettings.cmake)
else (USG_ROOT)
  message (FATAL_ERROR "[Fink] Unable to locate additional CMake scripts!")
endif (USG_ROOT)

## ------------------------------------------------------------------------------
## Check if the program/package/library is already available

if (NOT FINK_VERBOSE_CONFIGURE)
  set (FINK_FIND_QUIETLY  YES )
endif (NOT FINK_VERBOSE_CONFIGURE)

#include (FindFink)

## <-- begin build condition --------------------------------------------------->

if (NOT HAVE_FINK OR FINK_FORCE_BUILD)
  
  include (ExternalProject)
  
  find_file (have_fink_tgz fink-0.29.10.tar.gz  ${FINK_SOURCE_DIR})
  
  if (have_fink_tgz)
    ExternalProject_Add (fink
      PREFIX ${FINK_BINARY_DIR}
      DOWNLOAD_DIR ${FINK_BINARY_DIR}
      SOURCE_DIR ${FINK_BINARY_DIR}
      URL ${have_fink_tgz}
      BUILD_IN_SOURCE 1
      CONFIGURE_COMMAND ${FINK_BINARY_DIR}/bootstrap
      )
  else (have_fink_tgz)
    status (WARNING "[Fink] Unable to build project - no sources found!")
  endif (have_fink_tgz)
  
  ## <-- end build condition ----------------------------------------------------->
  
else (NOT HAVE_FINK OR FINK_FORCE_BUILD)
  add_custom_target (fink
    WORKING_DIRECTORY ${PLPLOT_BINARY_DIR}
    COMMENT "[Fink] Found system-wide installation; skipping rebuild!"
    )
endif (NOT HAVE_FINK OR FINK_FORCE_BUILD)

## ------------------------------------------------------------------------------
## Install Fink packages

add_custom_target (fink_packages
  COMMAND fink install atlas
  COMMAND fink install bison
  COMMAMD find install doxygen
  COMMAND fink install flex
  COMMAND fink install fftw3
  COMMAND fink install fftw3-shlibs
  COMMAND fink install gfortran-shlibs
  COMMAND fink install gsl
  COMMAND fink install gsl-shlibs
  COMMAND fink install numpy-py26
  COMMAND fink install qt4-mac
  COMMAND fink install sip-py26
  COMMAND fink install swig
  COMMAND fink install pyqt4-py26
  COMMAND fink install wget
  COMMENT "[Fink] Installing system packages ..."
  WORKING_DIRCTORY ${FINK_SOURCE_DIR}
  )
