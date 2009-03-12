
## Name of the project
project (BLITZ)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.6)

## -----------------------------------------------------------------------------
## Only force rebuild of the library from the provided source code, if no system
## installation can be found. In order to handle this properly, we use the CMake
## find script

option (BLITZ_FORCE_BUILD       "Force rebuild and local installation?"  NO  )
option (BLITZ_VERBOSE_CONFIGURE "Be verbose during configuration?"       YES )
option (BLITZ_VERBOSE_BUILD     "Be verbose during build process?"       NO  )

## ------------------------------------------------------------------------------
## Set CMAKE_MODULE_PATH to load custom CMake modules

find_path (USG_ROOT devel_common/cmake/CMakeSettings.cmake
  PATHS 
  ${BLITZ_SOURCE_DIR}
  ${BLITZ_SOURCE_DIR}/..
  ${BLITZ_SOURCE_DIR}/../..
  ${BLITZ_SOURCE_DIR}/../../..
  $ENV{LOFARSOFT}
  )

if (USG_ROOT)
  include (${USG_ROOT}/devel_common/cmake/CMakeSettings.cmake)
else (USG_ROOT)
  message (FATAL_ERROR "[Blitz++] Unable to locate additional CMake scripts!")
endif (USG_ROOT)

## ------------------------------------------------------------------------------
## Check if the program/package/library is already available

include (FindBlitz)

## <-- begin build condition --------------------------------------------------->

if (NOT HAVE_BLITZ OR BLITZ_FORCE_BUILD)

  ##__________________________________________________________________
  ## Locate the installation area
  
  find_path (BLITZ_prefix release_area.txt
    PATHS
    ${BLITZ_SOURCE_DIR}
    PATH_SUFFIXES
    ../release
    ../../release
    ../../../release
    NO_DEFAULT_PATH
    )
  
  if (NOT BLITZ_prefix)
    message (FATAL_ERROR "[Blitz++] Unable to locate the installation area")
  endif (NOT BLITZ_prefix)  

  ##__________________________________________________________________
  ## Tools and definitions

  find_program (HAVE_WGET  wget  ${bin_locations} )
  find_program (HAVE_CURL  curl  ${bin_locations} )
  find_program (HAVE_TAR   tar   ${bin_locations} )

  set (BLITZ_VERSION "0.9")
  set (BLITZ_DIRECTORY "blitz-${BLITZ_VERSION}")
  set (BLITZ_TAR_ARCHIVE "${BLITZ_DIRECTORY}.tar.gz")
  set (BLITZ_URL "${USG_DOWNLOAD}/software/${BLITZ_TAR_ARCHIVE}")
  
  ##__________________________________________________________________
  ## Check if a tar-archive with the source code is available already;
  ## if this is not the case we need to fetch it from the USg server.
  
  find_file (have_blitz_tgz ${BLITZ_TAR_ARCHIVE} ${BLITZ_SOURCE_DIR})
  
  if (have_blitz_tgz)
    add_custom_target (download_archive
      COMMAND cp ${have_blitz_tgz} .
      WORKING_DIRECTORY ${BLITZ_BINARY_DIR}
      COMMENT "Copying tar-archive from ${USG_ROOT}/external/blitz ..."
      )
  else (have_blitz_tgz)
    if (HAVE_WGET)
      add_custom_target (download_archive
	COMMAND wget -c ${BLITZ_URL}
	WORKING_DIRECTORY ${BLITZ_BINARY_DIR}
	)
    else (HAVE_WGET)
      if (HAVE_CURL)
	add_custom_target (download_archive
	  COMMAND curl ${BLITZ_URL} -o ${BLITZ_TAR_ARCHIVE} -C -
	  WORKING_DIRECTORY ${BLITZ_BINARY_DIR}
	  )
      else (HAVE_CURL)
	message (FATAL_ERROR "[Blitz++] No tool found to download tar-archive!")
      endif (HAVE_CURL)
    endif (HAVE_WGET)
  endif (have_blitz_tgz)
  
  ##__________________________________________________________________
  ## Expand the tar-archive
  
  add_custom_target (expand_archive
    COMMAND tar -xvzf ${BLITZ_TAR_ARCHIVE}
    WORKING_DIRECTORY ${BLITZ_BINARY_DIR}
    COMMENT "[Blitz++] Expanding tar-archive ..."
    )
  add_dependencies (expand_archive download_archive)
  
  ##__________________________________________________________________
  ## Configure and build the project

  ## run "./configure"

  add_custom_target (run_configure ALL
    COMMAND ./configure --prefix=${BLITZ_prefix}
    WORKING_DIRECTORY ${BLITZ_BINARY_DIR}/${BLITZ_DIRECTORY}
    COMMENT "[Blitz++] Running configure ..."
    )
  add_dependencies (run_configure expand_archive)
  
  ## run "make"

  add_custom_target (run_make ALL
    COMMAND make
    WORKING_DIRECTORY ${BLITZ_BINARY_DIR}/${BLITZ_DIRECTORY}
    COMMENT "[Blitz++] Starting build ..."
    )
  add_dependencies (run_make run_configure)
  
  ## run "make install"

  add_custom_target (install ALL
    COMMAND make install
    COMMAND rm -rf ${BLITZ_BINARY_DIR}/${BLITZ_TAR_ARCHIVE}
    COMMAND rm -rf ${BLITZ_BINARY_DIR}/${BLITZ_DIRECTORY}
    WORKING_DIRECTORY ${BLITZ_BINARY_DIR}/${BLITZ_DIRECTORY}
    COMMENT "[Blitz++] Installing software and cleaning up ..."
    )
  add_dependencies (install run_make)
  
  ## <-- end build condition --------------------------------------------------->
  
else (NOT HAVE_BLITZ OR BLITZ_FORCE_BUILD)
  message (STATUS "Found system-wide installation of BLITZ; skipping rebuild!")
endif (NOT HAVE_BLITZ OR BLITZ_FORCE_BUILD)
