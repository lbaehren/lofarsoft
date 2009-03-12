
## Name of the project
project (SWIG)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.6)

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
  ## Check if a tar-archive with the source code is available already;
  ## if this is not the case we need to fetch it from the USg server.
  
  find_file (have_swig_tgz ${SWIG_TAR_ARCHIVE} ${SWIG_SOURCE_DIR})
  
  if (have_swig_tgz)
    add_custom_target (download_archive
      COMMAND cp ${have_swig_tgz} .
      WORKING_DIRECTORY ${SWIG_BINARY_DIR}
      COMMENT "Copying tar-archive from ${USG_ROOT}/external/swig ..."
      )
  else (have_swig_tgz)
    if (HAVE_WGET)
      add_custom_target (download_archive
	COMMAND wget -c ${SWIG_URL}
	WORKING_DIRECTORY ${SWIG_BINARY_DIR}
	)
    else (HAVE_WGET)
      if (HAVE_CURL)
	add_custom_target (download_archive ALL
	  COMMAND curl ${SWIG_URL} -o ${SWIG_TAR_ARCHIVE} -C -
	  WORKING_DIRECTORY ${SWIG_BINARY_DIR}
	  )
      else (HAVE_CURL)
	message (FATAL_ERROR "[SWIG] No tool found to download tar-archive!")
      endif (HAVE_CURL)
    endif (HAVE_WGET)
  endif (have_swig_tgz)
  
  ##__________________________________________________________________
  ## Expand the tar-archive
  
  add_custom_target (expand_archive
    COMMAND tar -xvzf ${SWIG_TAR_ARCHIVE}
    WORKING_DIRECTORY ${SWIG_BINARY_DIR}
    COMMENT "Expanding tar-archive ..."
    )
  add_dependencies (expand_archive download_archive)

  ##__________________________________________________________________
  ## Configure and build the project

  #  X features:
  #    --x-includes=DIR    X include files are in DIR
  #    --x-libraries=DIR   X library files are in DIR
  #
  #  System types:
  #    --build=BUILD     configure for building on BUILD [guessed]
  #    --host=HOST       cross-compile to build programs to run on HOST [BUILD]

  ## run "./configure"

  add_custom_target (run_configure ALL
    COMMAND ./configure --prefix=${SWIG_prefix}
    WORKING_DIRECTORY ${SWIG_BINARY_DIR}/${SWIG_DIRECTORY}
    COMMENT "[SWIG] Running configure ..."
    )
  add_dependencies (run_configure expand_archive)
  
  ## run "make"

  add_custom_target (run_make ALL
    COMMAND make
    WORKING_DIRECTORY ${SWIG_BINARY_DIR}/${SWIG_DIRECTORY}
    COMMENT "[SWIG] Starting build ..."
    )
  add_dependencies (run_make run_configure)
  
  ## run "make install"

  add_custom_target (install ALL
    COMMAND make install
    COMMAND rm -rf ${SWIG_BINARY_DIR}/${SWIG_TAR_ARCHIVE}
    COMMAND rm -rf ${SWIG_BINARY_DIR}/${SWIG_DIRECTORY}
    WORKING_DIRECTORY ${SWIG_BINARY_DIR}/${SWIG_DIRECTORY}
    COMMENT "[SWIG] Installing software and cleaning up ..."
    )
  add_dependencies (install run_make)

  ## <-- end build condition ----------------------------------------------------->
  
else (NOT HAVE_SWIG OR SWIG_FORCE_BUILD)
  message (STATUS "Found system-wide installation of Swig; skipping rebuild!")
endif (NOT HAVE_SWIG OR SWIG_FORCE_BUILD)
