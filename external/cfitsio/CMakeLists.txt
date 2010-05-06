
## Name of the project
project (CFITSIO)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.8)

## -----------------------------------------------------------------------------
## Only force rebuild of the library from the provided source code, if no system
## installation can be found. In order to handle this properly, we use the CMake
## find script

option (CFITSIO_FORCE_BUILD       "Force rebuild and local installation?"  NO  )
option (CFITSIO_VERBOSE_CONFIGURE "Be verbose during configuration?"       NO  )
option (CFITSIO_VERBOSE_BUILD     "Be verbose during build process?"       NO  )

option (CFITSIO_enable_lfs        "Enable large file system support?"      YES )
option (CFITSIO_enable_reentrant  "Enable reentrant multithreading?"       NO  )
option (CFITSIO_enable_hera       "Build for HERA project?"                NO  )

## ------------------------------------------------------------------------------
## Set CMAKE_MODULE_PATH to load custom CMake modules

find_path (USG_ROOT devel_common/cmake/CMakeSettings.cmake
  PATHS 
  ${CFITSIO_SOURCE_DIR}
  ${CFITSIO_SOURCE_DIR}/..
  ${CFITSIO_SOURCE_DIR}/../..
  ${CFITSIO_SOURCE_DIR}/../../..
  $ENV{LOFARSOFT}
  )

if (USG_ROOT)
  include (${USG_ROOT}/devel_common/cmake/CMakeSettings.cmake)
else (USG_ROOT)
  message (FATAL_ERROR "Unable to locate additional CMake scripts!")
endif (USG_ROOT)

## ------------------------------------------------------------------------------
## Check if the program/package/library is already available

if (NOT CFITSIO_VERBOSE_CONFIGURE)
  set (CFITSIO_FIND_QUIETLY YES )
endif (NOT CFITSIO_VERBOSE_CONFIGURE)

include (FindCFITSIO)

## <-- begin build condition --------------------------------------------------->

if (NOT HAVE_CFITSIO OR CFITSIO_FORCE_BUILD)

  set (cfitsio_version "3140")
  set (CFITSIO_TAR_ARCHIVE "cfitsio${cfitsio_version}.tar.gz")

  ##__________________________________________________________________
  ## Build the project
  
  include (ExternalProject)
  
  find_file (have_cfitsio_tgz ${CFITSIO_TAR_ARCHIVE} ${CFITSIO_SOURCE_DIR})
  
  if (have_cfitsio_tgz)
    if (CFITSIO_enable_lfs)
      ExternalProject_Add (cfitsio
	PREFIX ${CFITSIO_BINARY_DIR}
	DOWNLOAD_DIR ${CFITSIO_BINARY_DIR}
	SOURCE_DIR ${CFITSIO_BINARY_DIR}
	URL ${have_cfitsio_tgz}
	BUILD_IN_SOURCE 1
	CONFIGURE_COMMAND ${CFITSIO_BINARY_DIR}/configure --prefix=${USG_INSTALL_PREFIX}  --includedir=${USG_INSTALL_PREFIX}/include/cfitsio --enable-lfs
	)
    else (CFITSIO_enable_lfs)
      ExternalProject_Add (cfitsio
	PREFIX ${CFITSIO_BINARY_DIR}
	DOWNLOAD_DIR ${CFITSIO_BINARY_DIR}
	SOURCE_DIR ${CFITSIO_BINARY_DIR}
	URL ${have_cfitsio_tgz}
	BUILD_IN_SOURCE 1
	CONFIGURE_COMMAND ${CFITSIO_BINARY_DIR}/configure --prefix=${USG_INSTALL_PREFIX}  --includedir=${USG_INSTALL_PREFIX}/include/cfitsio
	)
    endif (CFITSIO_enable_lfs)
  else (have_cfitsio_tgz)
    add_custom_target (cfitsio
      WORKING_DIRECTORY ${CFITSIO_BINARY_DIR}
      COMMENT "[CFITSIO] Unable to build package - missing sources!"
      )
  endif (have_cfitsio_tgz)
  
  ## <-- end build condition ----------------------------------------------------->
  
else (NOT HAVE_CFITSIO OR CFITSIO_FORCE_BUILD)
  add_custom_target (cfitsio
    WORKING_DIRECTORY ${CFITSIO_BINARY_DIR}
    COMMENT "[CFITSIO] Found system-wide installation; skipping rebuild!"
    )
endif (NOT HAVE_CFITSIO OR CFITSIO_FORCE_BUILD)
