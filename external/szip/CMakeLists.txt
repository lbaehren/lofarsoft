
## ------------------------------------------------------------------------------
#
#  CMake wrapper around the GNU Autotools-based configure/build of the SZIP
#  library.
#
## ------------------------------------------------------------------------------

## Name of the project
project (SZIP)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.8)

## -----------------------------------------------------------------------------
## Only force rebuild of the library from the provided source code, if no system
## installation can be found. In order to handle this properly, we use the CMake
## find script

option (SZIP_FORCE_BUILD       "Force rebuild and local installation"      NO  )
option (SZIP_VERBOSE_BUILD     "Be verbose during build process?"          NO  )

## ------------------------------------------------------------------------------
## Set CMAKE_MODULE_PATH to load custom CMake modules

find_path (USG_ROOT devel_common/cmake/CMakeSettings.cmake
  PATHS 
  ${SZIP_SOURCE_DIR}
  ${SZIP_SOURCE_DIR}/..
  ${SZIP_SOURCE_DIR}/../..
  ${SZIP_SOURCE_DIR}/../../..
  $ENV{LOFARSOFT}
  )

if (USG_ROOT)
  include (${USG_ROOT}/devel_common/cmake/CMakeSettings.cmake)
else (USG_ROOT)
  message (FATAL_ERROR "[Szip] Unable to locate additional CMake scripts!")
endif (USG_ROOT)


## ------------------------------------------------------------------------------
## Check for system-wide SZIP library installation

set (SZIP_FIND_QUIETLY YES)

include (FindSZIP)

## <-- begin build condition --------------------------------------------------->

if (NOT HAVE_SZIP OR SZIP_FORCE_BUILD)
  
  include (ExternalProject)

  set (SZIP_VERSION "2.1")
  set (SZIP_TAR_ARCHIVE "szip-${SZIP_VERSION}.tar.gz")

  ##__________________________________________________________________
  ## Build the project
  
  find_file (have_szip_tgz ${SZIP_TAR_ARCHIVE} ${SZIP_SOURCE_DIR})
  
  if (have_szip_tgz)
    ExternalProject_Add (szip
      PREFIX ${SZIP_BINARY_DIR}
      DOWNLOAD_DIR ${SZIP_BINARY_DIR}
      SOURCE_DIR ${SZIP_BINARY_DIR}
      URL ${have_szip_tgz}
      CONFIGURE_COMMAND ${SZIP_BINARY_DIR}/configure --prefix=${USG_INSTALL_PREFIX} --includedir=${USG_INSTALL_PREFIX}/include/szip
      )
  else (have_szip_tgz)
    message (WARNING "[Szip] Unable to build project - missing source!")
  endif (have_szip_tgz)
  
  ## <-- end build condition ----------------------------------------------------->
  
else (NOT HAVE_SZIP OR SZIP_FORCE_BUILD)
  add_custom_target (szip
    WORKING_DIRECTORY ${BOOST_BINARY_DIR}
    COMMENT "[Szip] Found system-wide installation; skipping rebuild!"
    )
  message (STATUS "[Szip] Found system-wide installation; skipping rebuild!")
endif (NOT HAVE_SZIP OR SZIP_FORCE_BUILD)
