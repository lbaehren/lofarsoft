
## Name of the project
project (ROOT)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.5)

## -----------------------------------------------------------------------------
## Only force rebuild of the library from the provided source code, if no system
## installation can be found. In order to handle this properly, we use the CMake
## find script

option (ROOT_FORCE_BUILD "Force rebuild and local installation?"   NO  )
option (ROOT_CMAKE_BUILD "Use CMake to configure and build ROOT?"  YES )
option (ROOT_VERBOSE_CONFIGURE "Be verbose during configuration?"  NO  )
option (ROOT_VERBOSE_BUILD     "Be verbose during build process?"  NO  )

## ------------------------------------------------------------------------------
## Set CMAKE_MODULE_PATH to load custom CMake modules

find_path (USG_ROOT devel_common/cmake/CMakeSettings.cmake
  PATHS 
  ${ROOT_SOURCE_DIR}
  ${ROOT_SOURCE_DIR}/..
  ${ROOT_SOURCE_DIR}/../..
  ${ROOT_SOURCE_DIR}/../../..
  ENV LOFARSOFT
  )

if (USG_ROOT)
  include (${USG_ROOT}/devel_common/cmake/CMakeSettings.cmake)
else (USG_ROOT)
  message (FATAL_ERROR "Unable to locate additional CMake scripts!")
endif (USG_ROOT)

## ------------------------------------------------------------------------------
## Check if the program/package/library is already available

if (NOT ROOT_VERBOSE_CONFIGURE)
  set (ROOT_FIND_QUIETLY YES)
endif (NOT ROOT_VERBOSE_CONFIGURE)

include (FindROOT)

## <-- begin build condition --------------------------------------------------->

if (NOT HAVE_ROOT OR ROOT_FORCE_BUILD)
  
  ## ------------------------------------------------------------------------------
  ## Locate the installation area
  
  find_path (ROOT_prefix release_area.txt
    PATHS
    ${ROOT_SOURCE_DIR}
    PATH_SUFFIXES
    ../release
    ../../release
    ../../../release
    NO_DEFAULT_PATH
    )
  
  if (NOT ROOT_prefix)
    message (FATAL_ERROR "Unable to locate the installation area")
  endif (NOT ROOT_prefix)
  
  ## ------------------------------------------------------------------------------
  ## Check for required tools

  find_program (HAVE_WGET  wget  ${bin_locations} )
  find_program (HAVE_CURL  curl  ${bin_locations} )
  find_program (HAVE_TAR   tar   ${bin_locations} )

  ## ------------------------------------------------------------------------------
  ## Download the tar-archive from the USG server

  set (ROOT_VERSION "v5.20.00")
  set (ROOT_TAR_ARCHIVE "root_${ROOT_VERSION}.source.tar.gz")
  set (ROOT_URL "${USG_DOWNLOAD}/software/${ROOT_TAR_ARCHIVE}")

  if (ROOT_VERBOSE_CONFIGURE)
    message (STATUS "Root version      = ${ROOT_VERSION}")
    message (STATUS "Root download URL = ${ROOT_URL}")
    message (STATUS "wget binary       = ${HAVE_WGET}")
    message (STATUS "curl binary       = ${HAVE_CURL}")
  endif (ROOT_VERBOSE_CONFIGURE)
  
  if (HAVE_WGET)
    message (STATUS "Retrieving tar-archive from USG server ...")
    execute_process (
      WORKING_DIRECTORY ${ROOT_BINARY_DIR}
      COMMAND wget -c ${ROOT_URL}
      TIMEOUT 300
      ERROR_VARIABLE error_wget
      ERROR_FILE error.log
      )
  else (HAVE_WGET)
    if (HAVE_CURL)
      execute_process (
	WORKING_DIRECTORY ${ROOT_BINARY_DIR}
	COMMAND curl ${ROOT_URL}
	TIMEOUT 300
	ERROR_VARIABLE error_curl
	ERROR_FILE error.log
	)
    else (HAVE_CURL)
      message (FATAL_ERROR "[ROOT] No tool found to download tar-archive!")
    endif (HAVE_CURL)
  endif (HAVE_WGET)
  
  ## ------------------------------------------------------------------------------
  ## Expand the downloaded tar-archive

  find_file (HAVE_TAR_ARCHIVE ${ROOT_TAR_ARCHIVE}
    PATHS ${ROOT_BINARY_DIR}
    )

  if (NOT HAVE_TAR_ARCHIVE)
    message (FATAL_ERROR "Unable to find downloaded tar archive!")
  endif (NOT HAVE_TAR_ARCHIVE)

  if (HAVE_TAR)
    message (STATUS "[ROOT] Expanding downloaded tar-archive ...")
      execute_process (
	WORKING_DIRECTORY ${ROOT_BINARY_DIR}
	COMMAND tar -xvzf ${HAVE_TAR_ARCHIVE}
	TIMEOUT 300
	ERROR_VARIABLE error_tar
	ERROR_FILE error.log
	)
  else (HAVE_TAR)
    message (FATAL_ERROR "[ROOT] Unable to find executable tar; please install!")
  endif (HAVE_TAR)

  ## ------------------------------------------------------------------------------
  ## Configure and build

  if (NOT error_tar)
    message (STATUS "Configuring ROOT ...")    
    execute_process (
      WORKING_DIRECTORY ${ROOT_BINARY_DIR}/root
      COMMAND ./configure --prefix=${ROOT_prefix}
      TIMEOUT 300
      ERROR_VARIABLE error_configure
      ERROR_FILE error.log
      )
  endif (NOT error_tar)

  if (NOT error_configure)
    execute_process (
      WORKING_DIRECTORY ${ROOT_BINARY_DIR}/root
      COMMAND make
      TIMEOUT 1500
      ERROR_VARIABLE error_make
      ERROR_FILE error.log
      )
  endif (NOT error_configure)
  
  ## ------------------------------------------------------------------------------
  ## Install

  if (NOT error_make)
    execute_process (
      WORKING_DIRECTORY ${ROOT_BINARY_DIR}/root
      COMMAND make install
      TIMEOUT 1500
      ERROR_VARIABLE error_make
      ERROR_FILE error.log
      )
  endif (NOT error_make)
  
  ## <-- end build condition ----------------------------------------------------->
  
else (NOT HAVE_ROOT OR ROOT_FORCE_BUILD)
  message (STATUS "Found system-wide installation of ROOT; skipping rebuild!")
endif (NOT HAVE_ROOT OR ROOT_FORCE_BUILD)
