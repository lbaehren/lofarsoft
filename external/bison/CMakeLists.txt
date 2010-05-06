
## ------------------------------------------------------------------------------
#
#  CMake wrapper around the GNU Autotools-based configure/build of the BISON
#  library.
#
## ------------------------------------------------------------------------------

## Name of the project
project (BISON)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.8)

## -----------------------------------------------------------------------------
## Only force rebuild of the library from the provided source code, if no system
## installation can be found. In order to handle this properly, we use the CMake
## find script

option (BISON_FORCE_BUILD       "Force rebuild and local installation"      NO  )
option (BISON_VERBOSE_BUILD     "Be verbose during build process?"          NO  )

## ------------------------------------------------------------------------------
## Set CMAKE_MODULE_PATH to load custom CMake modules

find_path (USG_ROOT devel_common/cmake/CMakeSettings.cmake
  PATHS 
  ${BISON_SOURCE_DIR}
  ${BISON_SOURCE_DIR}/..
  ${BISON_SOURCE_DIR}/../..
  ${BISON_SOURCE_DIR}/../../..
  $ENV{LOFARSOFT}
  )

if (USG_ROOT)
  include (${USG_ROOT}/devel_common/cmake/CMakeSettings.cmake)
else (USG_ROOT)
  message (FATAL_ERROR "[Bison] Unable to locate additional CMake scripts!")
endif (USG_ROOT)


## ------------------------------------------------------------------------------
## Check for system-wide BISON library installation

if (bison_executable)
  set (HAVE_BISON YES)
else (bison_executable)
  set (HAVE_BISON NO)
endif (bison_executable)

## <-- begin build condition --------------------------------------------------->

if (NOT HAVE_BISON OR BISON_FORCE_BUILD)
  
  include (ExternalProject)

  set (BISON_VERSION     "2.4")
  set (BISON_DIRECTORY   "bison-${BISON_VERSION}")
  set (BISON_TAR_ARCHIVE "${BISON_DIRECTORY}.tar.gz")
  
  ##__________________________________________________________________
  ## Build the project

  find_file (have_bison_tgz ${BISON_TAR_ARCHIVE} ${BISON_SOURCE_DIR})

  if (have_bison_tgz)
    ExternalProject_Add (bison
      PREFIX ${BISON_BINARY_DIR}
      DOWNLOAD_DIR ${BISON_BINARY_DIR}
      SOURCE_DIR ${BISON_BINARY_DIR}
      URL ${have_bison_tgz}
      BUILD_IN_SOURCE 1
      CONFIGURE_COMMAND ${BISON_BINARY_DIR}/configure --prefix=${USG_INSTALL_PREFIX}
      )
  else (have_bison_tgz)
    message (WARNING "[Bison] Unable to build project - missing source!")
  endif (have_bison_tgz)
  
  ## <-- end build condition ----------------------------------------------------->
  
else (NOT HAVE_BISON OR BISON_FORCE_BUILD)
  add_custom_target (bison
    WORKING_DIRECTORY ${BISON_BINARY_DIR}
    COMMENT "[Bison] Found system-wide installation; skipping rebuild!"
    )
endif (NOT HAVE_BISON OR BISON_FORCE_BUILD)
