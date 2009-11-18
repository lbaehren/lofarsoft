
## Name of the project
project (SWIG)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.8)

## -----------------------------------------------------------------------------
## Only force rebuild of the library from the provided source code, if no system
## installation can be found. In order to handle this properly, we use the CMake
## find script

option (SWIG_FORCE_BUILD       "Force rebuild and local installation?"  NO  )
option (SWIG_VERBOSE_CONFIGURE "Be verbose during configuration?"       YES )
option (SWIG_VERBOSE_BUILD     "Be verbose during build process?"       NO  )

## ------------------------------------------------------------------------------
## Set CMAKE_MODULE_PATH to load custom CMake modules

find_path (USG_ROOT devel_common/cmake/CMakeSettings.cmake
  PATHS 
  ${SWIG_SOURCE_DIR}
  ${SWIG_SOURCE_DIR}/..
  ${SWIG_SOURCE_DIR}/../..
  ${SWIG_SOURCE_DIR}/../../..
  $ENV{LOFARSOFT}
  )

if (USG_ROOT)
  include (${USG_ROOT}/devel_common/cmake/CMakeSettings.cmake)
else (USG_ROOT)
  message (FATAL_ERROR "[SWIG] Unable to locate additional CMake scripts!")
endif (USG_ROOT)

## ------------------------------------------------------------------------------
## Check if the program/package/library is already available

include (FindSWIG)

if (SWIG_FOUND)
  ## report found SWIG installation
  if (SWIG_VERBOSE_CONFIGURE)
    message (STATUS "SWIG_DIR        = ${SWIG_DIR}")
    message (STATUS "SWIG_EXECUTABLE = ${SWIG_EXECUTABLE}")
    message (STATUS "SWIG_VERSION    = ${SWIG_VERSION}")
  endif (SWIG_VERBOSE_CONFIGURE)
  
  string (COMPARE GREATER ${SWIG_VERSION} "1.3.31" SWIG_VERSION_OK)

  if (SWIG_VERSION_OK)
    set (HAVE_SWIG TRUE)
  else (SWIG_VERSION_OK)
    message (STATUS "SWIG version too old - forcing build of newer version!")
    set (HAVE_SWIG FALSE)
  endif (SWIG_VERSION_OK)
else (SWIG_FOUND)
    set (HAVE_SWIG FALSE)
endif (SWIG_FOUND)

## <-- begin build condition --------------------------------------------------->

if (NOT HAVE_SWIG OR SWIG_FORCE_BUILD)
  
  ##__________________________________________________________________
  ## Locate the installation area
  
  find_path (SWIG_prefix release_area.txt
    PATHS
    ${SWIG_SOURCE_DIR}
    PATH_SUFFIXES
    ../release
    ../../release
    ../../../release
    NO_DEFAULT_PATH
    )
  
  if (NOT SWIG_prefix)
    message (FATAL_ERROR "[Swig] Unable to locate the installation area")
  endif (NOT SWIG_prefix)
  
  ##__________________________________________________________________
  ## Tools and definitions

  find_program (HAVE_WGET  wget  ${bin_locations} )
  find_program (HAVE_CURL  curl  ${bin_locations} )
  find_program (HAVE_TAR   tar   ${bin_locations} )

  set (SWIG_VERSION "1.3.38")
  set (SWIG_DIRECTORY "swig-${SWIG_VERSION}")
  set (SWIG_TAR_ARCHIVE "${SWIG_DIRECTORY}.tar.gz")
  set (SWIG_URL "${USG_DOWNLOAD}/software/${SWIG_TAR_ARCHIVE}")

  ##__________________________________________________________________
  ## Build the project

  include (ExternalProject)

  find_file (have_swig_tgz ${SWIG_TAR_ARCHIVE} ${SWIG_SOURCE_DIR})
  
  if (have_swig_tgz)
    ExternalProject_Add (swig
      PREFIX ${USG_ROOT}/build/swig
      DOWNLOAD_DIR ${SWIG_BINARY_DIR}/swig
      SOURCE_DIR ${SWIG_BINARY_DIR}/swig
      URL ${have_swig_tgz}
      CONFIGURE_COMMAND ${SWIG_BINARY_DIR}/swig/configure --prefix=${SWIG_prefix}
      )
  else (have_swig_tgz)
    ExternalProject_Add (swig
      PREFIX ${USG_ROOT}/build/swig
      DOWNLOAD_DIR ${SWIG_BINARY_DIR}/swig
      SOURCE_DIR ${SWIG_BINARY_DIR}/swig
      URL ${SWIG_URL}
      CONFIGURE_COMMAND ${SWIG_BINARY_DIR}/swig/configure --prefix=${SWIG_prefix}
      )
  endif (have_swig_tgz)
  
## <-- end build condition ----------------------------------------------------->
  
else (NOT HAVE_SWIG OR SWIG_FORCE_BUILD)
  message (STATUS "Found system-wide installation of Swig; skipping rebuild!")
endif (NOT HAVE_SWIG OR SWIG_FORCE_BUILD)
