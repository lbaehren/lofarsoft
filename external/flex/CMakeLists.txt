
## ------------------------------------------------------------------------------
#
#  CMake wrapper around the GNU Autotools-based configure/build of the FLEX
#  library.
#
## ------------------------------------------------------------------------------

## Name of the project
project (FLEX)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.8)

## -----------------------------------------------------------------------------
## Only force rebuild of the library from the provided source code, if no system
## installation can be found. In order to handle this properly, we use the CMake
## find script

option (FLEX_FORCE_BUILD       "Force rebuild and local installation"      NO  )
option (FLEX_VERBOSE_BUILD     "Be verbose during build process?"          NO  )

## ------------------------------------------------------------------------------
## Set CMAKE_MODULE_PATH to load custom CMake modules

find_path (USG_ROOT devel_common/cmake/CMakeSettings.cmake
  PATHS 
  ${FLEX_SOURCE_DIR}
  ${FLEX_SOURCE_DIR}/..
  ${FLEX_SOURCE_DIR}/../..
  ${FLEX_SOURCE_DIR}/../../..
  $ENV{LOFARSOFT}
  )

if (USG_ROOT)
  include (${USG_ROOT}/devel_common/cmake/CMakeSettings.cmake)
else (USG_ROOT)
  message (FATAL_ERROR "[Flex] Unable to locate additional CMake scripts!")
endif (USG_ROOT)


## ------------------------------------------------------------------------------
## Check for system-wide FLEX library installation

find_program (HAVE_FLEX flex PATHS ${bin_locations})

## <-- begin build condition --------------------------------------------------->

if (NOT HAVE_FLEX OR FLEX_FORCE_BUILD)
  
  include (ExternalProject)

  set (FLEX_VERSION     "2.5.35")
  set (FLEX_DIRECTORY   "flex-${FLEX_VERSION}")
  set (FLEX_TAR_ARCHIVE "${FLEX_DIRECTORY}.tar.gz")
  
  ##__________________________________________________________________
  ## Build the project

  find_file (have_flex_tgz ${FLEX_TAR_ARCHIVE} ${FLEX_SOURCE_DIR})

  if (have_flex_tgz)
    ExternalProject_Add (flex
      PREFIX ${FLEX_BINARY_DIR}
      DOWNLOAD_DIR ${FLEX_BINARY_DIR}
      SOURCE_DIR ${FLEX_BINARY_DIR}
      URL ${have_flex_tgz}
      BUILD_IN_SOURCE 1
      CONFIGURE_COMMAND ${FLEX_BINARY_DIR}/configure --prefix=${USG_INSTALL_PREFIX}
      )
  else (have_flex_tgz)
    message (WARNING "[Flex] Unable to build project - missing source!")
  endif (have_flex_tgz)
  
  ## <-- end build condition ----------------------------------------------------->
  
else (NOT HAVE_FLEX OR FLEX_FORCE_BUILD)
  add_custom_target (flex
    WORKING_DIRECTORY ${FLEX_BINARY_DIR}
    COMMENT "[Flex] Found system-wide installation; skipping rebuild!"
    )
endif (NOT HAVE_FLEX OR FLEX_FORCE_BUILD)
