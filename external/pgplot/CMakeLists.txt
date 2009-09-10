
## Name of the project
project (PGPLOT)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.5)

## -----------------------------------------------------------------------------
## Only force rebuild of the library from the provided source code, if no system
## installation can be found. In order to handle this properly, we use the CMake
## find script

option (PGPLOT_FORCE_BUILD       "Force rebuild and local installation?"    NO  )
option (PGPLOT_VERBOSE_CONFIGURE "Be verbose during configuration?"         NO  )
option (PGPLOT_VERBOSE_BUILD     "Be verbose during build process?"         NO  )

## ------------------------------------------------------------------------------
## Set CMAKE_MODULE_PATH to load custom CMake modules

find_path (USG_ROOT devel_common/cmake/CMakeSettings.cmake
  PATHS 
  ${PGPLOT_SOURCE_DIR}
  ${PGPLOT_SOURCE_DIR}/..
  ${PGPLOT_SOURCE_DIR}/../..
  ${PGPLOT_SOURCE_DIR}/../../..
  ENV LOFARSOFT
  )

if (USG_ROOT)
  include (${USG_ROOT}/devel_common/cmake/CMakeSettings.cmake)
else (USG_ROOT)
  message (FATAL_ERROR "Unable to locate additional CMake scripts!")
endif (USG_ROOT)

## ------------------------------------------------------------------------------
## Check if the program/package/library is already available

include (FindPGPLOT)

## <-- begin build condition --------------------------------------------------->

if (NOT HAVE_PGPLOT OR PGPLOT_FORCE_BUILD)
  
  ## ------------------------------------------------------------------------------
  ## Locate the installation area
  
  find_path (PGPLOT_prefix release_area.txt
    PATHS
    ${PGPLOT_SOURCE_DIR}
    PATH_SUFFIXES
    ../release
    ../../release
    ../../../release
    NO_DEFAULT_PATH
    )
  
  if (NOT PGPLOT_prefix)
    message (FATAL_ERROR "Unable to locate the installation area")
  endif (NOT PGPLOT_prefix)
  
  ## ------------------------------------------------------------------------------
  ## Check for required tools
  
  find_program (HAVE_WGET  wget  ${bin_locations} )
  find_program (HAVE_CURL  curl  ${bin_locations} )
  find_program (HAVE_TAR   tar   ${bin_locations} )
  
  ##__________________________________________________________________
  ## Download the tar-archive from the USG server

  set (PGPLOT_VERSION "5.2")
  set (PGPLOT_DIRECTORY "pgplot")
  set (PGPLOT_TAR_ARCHIVE "pgplot${PGPLOT_VERSION}.tar.gz")
  set (PGPLOT_URL "ftp://ftp.astro.caltech.edu/pub/pgplot/${PGPLOT_TAR_ARCHIVE}")

  if (HAVE_WGET)
    add_custom_target (download_archive ALL
      COMMAND wget -c ${PGPLOT_URL}
      WORKING_DIRECTORY ${PGPLOT_BINARY_DIR}
      )
  else (HAVE_WGET)
    if (HAVE_CURL)
      add_custom_target (download_archive ALL
	COMMAND curl ${PGPLOT_URL} -o ${PGPLOT_TAR_ARCHIVE} -C -
	WORKING_DIRECTORY ${PGPLOT_BINARY_DIR}
	)
    else (HAVE_CURL)
      message (FATAL_ERROR "[PGPLOT] No tool found to download tar-archive!")
    endif (HAVE_CURL)
  endif (HAVE_WGET)
  
  ##__________________________________________________________________
  ## Expand the downloaded tar-archive
  
  add_custom_target (expand_archive ALL
    COMMAND tar -xvzf ${PGPLOT_TAR_ARCHIVE}
    WORKING_DIRECTORY ${PGPLOT_BINARY_DIR}
    )
  add_dependencies (expand_archive download_archive)

  ##__________________________________________________________________
  ## Configure
  
  add_custom_target (run_configure ALL
    COMMAND ./configure --prefix=${PGPLOT_prefix}
    WORKING_DIRECTORY ${PGPLOT_BINARY_DIR}/${PGPLOT_BUILD_DIR}
    )
  add_dependencies (run_configure expand_archive)
  
  ##__________________________________________________________________
  ## Build
  
  add_custom_target (run_make ALL
    COMMAND make
    WORKING_DIRECTORY ${PGPLOT_BINARY_DIR}/${PGPLOT_BUILD_DIR}
    )
  add_dependencies (run_make run_configure)

  ##__________________________________________________________________
  ## Installation

  add_custom_target (install ALL
    COMMAND make install
    WORKING_DIRECTORY ${PGPLOT_BINARY_DIR}/${PGPLOT_BUILD_DIR}
    )
  add_dependencies (install run_make)

  ##__________________________________________________________________
  ## Installation

  add_custom_target (cleanup_build ALL
    COMMAND rm -rf qt-*
    WORKING_DIRECTORY ${PGPLOT_BINARY_DIR}
    )
  add_dependencies (install run_make)

  ## <-- end build condition ----------------------------------------------------->
  
else (NOT HAVE_PGPLOT OR PGPLOT_FORCE_BUILD)
  message (STATUS "Found system-wide installation of PGPLOT; skipping rebuild!")
endif (NOT HAVE_PGPLOT OR PGPLOT_FORCE_BUILD)
