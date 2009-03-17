
## Name of the project
project (PLPLOT)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.6)

## -----------------------------------------------------------------------------
## Only force rebuild of the library from the provided source code, if no system
## installation can be found. In order to handle this properly, we use the CMake
## find script

option (PLPLOT_FORCE_BUILD       "Force rebuild and local installation?"  NO  )
option (PLPLOT_VERBOSE_CONFIGURE "Be verbose during configuration?"       YES )
option (PLPLOT_VERBOSE_BUILD     "Be verbose during build process?"       NO  )

## ------------------------------------------------------------------------------
## Set CMAKE_MODULE_PATH to load custom CMake modules

find_path (USG_ROOT devel_common/cmake/CMakeSettings.cmake
  PATHS 
  ${PLPLOT_SOURCE_DIR}
  ${PLPLOT_SOURCE_DIR}/..
  ${PLPLOT_SOURCE_DIR}/../..
  ${PLPLOT_SOURCE_DIR}/../../..
  $ENV{LOFARSOFT}
  )

if (USG_ROOT)
  include (${USG_ROOT}/devel_common/cmake/CMakeSettings.cmake)
else (USG_ROOT)
  message (FATAL_ERROR "[PLPLOT] Unable to locate additional CMake scripts!")
endif (USG_ROOT)

## ------------------------------------------------------------------------------
## Check if the program/package/library is already available

set (PLPLOT_FIND_QUIETLY YES )

include (FindPLplot)

## <-- begin build condition --------------------------------------------------->

if (NOT HAVE_PLPLOT OR PLPLOT_FORCE_BUILD)
  
  ##__________________________________________________________________
  ## Locate the installation area
  
  find_path (PLPLOT_prefix release_area.txt
    PATHS
    ${PLPLOT_SOURCE_DIR}
    PATH_SUFFIXES
    ../release
    ../../release
    ../../../release
    NO_DEFAULT_PATH
    )
  
  if (NOT PLPLOT_prefix)
    message (FATAL_ERROR "[Plplot] Unable to locate the installation area")
  endif (NOT PLPLOT_prefix)
  
  ##__________________________________________________________________
  ## Tools and definitions

  find_program (HAVE_WGET  wget  ${bin_locations} )
  find_program (HAVE_CURL  curl  ${bin_locations} )
  find_program (HAVE_TAR   tar   ${bin_locations} )

  set (PLPLOT_VERSION "5.9.2")
  set (PLPLOT_DIRECTORY "plplot-${PLPLOT_VERSION}")
  set (PLPLOT_TAR_ARCHIVE "${PLPLOT_DIRECTORY}.tar.gz")
  set (PLPLOT_URL "${USG_DOWNLOAD}/software/${PLPLOT_TAR_ARCHIVE}")

  ##__________________________________________________________________
  ## Python.Numeric header files

  include (FindNumeric)

  if (NUMERIC_INCLUDES)
    include_directories (${NUMERIC_INCLUDES})
  endif (NUMERIC_INCLUDES)
  
  ##__________________________________________________________________
  ## Check if a tar-archive with the source code is available already;
  ## if this is not the case we need to fetch it from the USg server.
  
  find_file (have_plplot_tgz ${PLPLOT_TAR_ARCHIVE} ${PLPLOT_SOURCE_DIR})
  
  if (have_plplot_tgz)
    add_custom_target (download_archive
      COMMAND cp ${have_plplot_tgz} .
      WORKING_DIRECTORY ${PLPLOT_BINARY_DIR}
      COMMENT "Copying tar-archive from ${USG_ROOT}/external/plplot ..."
      )
  else (have_plplot_tgz)
    if (HAVE_WGET)
      add_custom_target (download_archive
	COMMAND wget -c ${PLPLOT_URL}
	WORKING_DIRECTORY ${PLPLOT_BINARY_DIR}
	)
    else (HAVE_WGET)
      if (HAVE_CURL)
	add_custom_target (download_archive ALL
	  COMMAND curl ${PLPLOT_URL} -o ${PLPLOT_TAR_ARCHIVE} -C -
	  WORKING_DIRECTORY ${PLPLOT_BINARY_DIR}
	  )
      else (HAVE_CURL)
	message (FATAL_ERROR "[PLPLOT] No tool found to download tar-archive!")
      endif (HAVE_CURL)
    endif (HAVE_WGET)
  endif (have_plplot_tgz)
  
  ##__________________________________________________________________
  ## Expand the tar-archive
  
  add_custom_target (expand_archive
    COMMAND tar -xvzf ${PLPLOT_TAR_ARCHIVE}
    WORKING_DIRECTORY ${PLPLOT_BINARY_DIR}
    COMMENT "Expanding tar-archive ..."
    )
  add_dependencies (expand_archive download_archive)

  ##__________________________________________________________________
  ## Configure and build the project

  ## Create configuration files from templates

  configure_file (
    ${PLPLOT_SOURCE_DIR}/plplot.cmake.in
    ${PLPLOT_BINARY_DIR}/plplot.cmake
    )

  ## run "cmake"

  add_custom_target (run_cmake ALL
    COMMAND cmake -C ${PLPLOT_BINARY_DIR}/plplot.cmake .
    COMMAND cmake .
    WORKING_DIRECTORY ${PLPLOT_BINARY_DIR}/${PLPLOT_DIRECTORY}
    COMMENT "[PLPLOT] Running configure ..."
    )
  add_dependencies (run_cmake expand_archive)
  
  ## run "make"

  add_custom_target (run_make ALL
    COMMAND make
    WORKING_DIRECTORY ${PLPLOT_BINARY_DIR}/${PLPLOT_DIRECTORY}
    COMMENT "[PLPLOT] Starting build ..."
    )
  add_dependencies (run_make run_cmake)
  
  ## run "make install"

  add_custom_target (install ALL
    COMMAND make install
    COMMAND rm -rf ${PLPLOT_BINARY_DIR}/${PLPLOT_TAR_ARCHIVE}
    COMMAND rm -rf ${PLPLOT_BINARY_DIR}/${PLPLOT_DIRECTORY}
    WORKING_DIRECTORY ${PLPLOT_BINARY_DIR}/${PLPLOT_DIRECTORY}
    COMMENT "[PLPLOT] Installing software and cleaning up ..."
    )
  add_dependencies (install run_make)

  ## <-- end build condition ----------------------------------------------------->
  
else (NOT HAVE_PLPLOT OR PLPLOT_FORCE_BUILD)
  message (STATUS "Found system-wide installation of Plplot; skipping rebuild!")
endif (NOT HAVE_PLPLOT OR PLPLOT_FORCE_BUILD)
