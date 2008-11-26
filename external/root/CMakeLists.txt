
## Name of the project
project (ROOT)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.5)

## -----------------------------------------------------------------------------
## Only force rebuild of the library from the provided source code, if no system
## installation can be found. In order to handle this properly, we use the CMake
## find script

option (ROOT_FORCE_BUILD       "Force rebuild and local installation?"   NO  )
option (ROOT_CMAKE_BUILD       "Use CMake to configure and build ROOT?"  YES )
option (ROOT_VERBOSE_CONFIGURE "Be verbose during configuration?"        NO  )
option (ROOT_VERBOSE_BUILD     "Be verbose during build process?"        NO  )

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

  ##__________________________________________________________________
  ## Download the tar-archive from the USG server

  set (ROOT_VERSION "v5.20.00")
  set (ROOT_TAR_ARCHIVE "root_${ROOT_VERSION}.source.tar.gz")
  set (ROOT_URL "${USG_DOWNLOAD}/software/${ROOT_TAR_ARCHIVE}")
  
  if (HAVE_WGET)
    add_custom_target (download_archive ALL
      COMMAND wget -c ${ROOT_URL}
      WORKING_DIRECTORY ${ROOT_BINARY_DIR}
      )
  else (HAVE_WGET)
    if (HAVE_CURL)
      add_custom_target (download_archive ALL
	COMMAND curl ${ROOT_URL} -o ${ROOT_TAR_ARCHIVE} -C
	WORKING_DIRECTORY ${ROOT_BINARY_DIR}
	)
    else (HAVE_CURL)
      message (FATAL_ERROR "[ROOT] No tool found to download tar-archive!")
    endif (HAVE_CURL)
  endif (HAVE_WGET)
  
  ##__________________________________________________________________
  ## Expand the downloaded tar-archive

  if (HAVE_TAR)
    add_custom_target (expand_archive ALL
      COMMAND tar -xvzf ${ROOT_TAR_ARCHIVE}
      WORKING_DIRECTORY ${ROOT_BINARY_DIR}
      )
    add_dependencies (expand_archive download_archive)
  else (HAVE_TAR)
    message (FATAL_ERROR "[ROOT] Missing tar to expand downloaded archive!")
  endif (HAVE_TAR)
  
  ##__________________________________________________________________
  ## Configure, build and install
  
  add_custom_target (run_configure ALL
    COMMAND ./configure --prefix=${ROOT_prefix}
    WORKING_DIRECTORY ${ROOT_BINARY_DIR}/root
    )
  add_dependencies (run_configure expand_archive)
  
  add_custom_target (run_make ALL
    COMMAND make
    WORKING_DIRECTORY ${ROOT_BINARY_DIR}/root
    )
  add_dependencies (run_make run_configure)

  add_custom_target (install ALL
    COMMAND make install
    WORKING_DIRECTORY ${ROOT_BINARY_DIR}/root
    )
  add_dependencies (install run_make)
  
  ##__________________________________________________________________
  ## Clean up the build directory

  add_custom_target (cleanup_build ALL
    COMMAND rm -f ${ROOT_TAR_ARCHIVE}
    COMMAND rm -rf root
    WORKING_DIRECTORY ${ROOT_BINARY_DIR}
    )
  
  ## <-- end build condition ----------------------------------------------------->
  
else (NOT HAVE_ROOT OR ROOT_FORCE_BUILD)
  message (STATUS "Found system-wide installation of ROOT; skipping rebuild!")
endif (NOT HAVE_ROOT OR ROOT_FORCE_BUILD)
