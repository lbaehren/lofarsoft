
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

find_program (HAVE_FINK fink ${bin_locations})

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
## Check code tree settings

# test -r /sw/bin/init.sh && . /sw/bin/init.sh

# Trees: local/main stable/main stable/crypto unstable/main unstable/crypto
# UseBinaryDist: false

if (HAVE_FINK)
  find_file (have_fink_conf fink.conf /sw/etc)
endif (HAVE_FINK)

## ------------------------------------------------------------------------------
## Install Fink packages

if (HAVE_FINK)
  add_custom_target (fink_packages
    COMMAND ${HAVE_FINK} selfupdate
    COMMAND ${HAVE_FINK} index
    COMMAND ${HAVE_FINK} scanpackages
#    COMMAND ${HAVE_FINK} install atlas
    COMMAND ${HAVE_FINK} install bison
    COMMAMD ${HAVE_FINK} install doxygen
    COMMAND ${HAVE_FINK} install flex
    COMMAND ${HAVE_FINK} install fftw3
    COMMAND ${HAVE_FINK} install fftw3-shlibs
    COMMAND ${HAVE_FINK} install gfortran-shlibs
    COMMAND ${HAVE_FINK} install gsl
    COMMAND ${HAVE_FINK} install gsl-shlibs
    COMMAND ${HAVE_FINK} install numpy-py26
    COMMAND ${HAVE_FINK} install qt4-mac
    COMMAND ${HAVE_FINK} install sip-py26
    COMMAND ${HAVE_FINK} install swig
    COMMAND ${HAVE_FINK} install pyqt4-py26
    COMMAND ${HAVE_FINK} install wget
    COMMENT "[Fink] Installing system packages ..."
    WORKING_DIRCTORY ${FINK_SOURCE_DIR}
    )
endif (HAVE_FINK)
