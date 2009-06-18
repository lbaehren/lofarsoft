
## Name of the project
project (PYQT)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.6)

## -----------------------------------------------------------------------------
## Only force rebuild of the library from the provided source code, if no system
## installation can be found. In order to handle this properly, we use the CMake
## find script

option (PYQT_FORCE_BUILD       "Force rebuild and local installation?"  NO  )
option (PYQT_VERBOSE_CONFIGURE "Be verbose during configuration?"       YES )
option (PYQT_VERBOSE_BUILD     "Be verbose during build process?"       NO  )

## ------------------------------------------------------------------------------
## Set CMAKE_MODULE_PATH to load custom CMake modules

find_path (USG_ROOT devel_common/cmake/CMakeSettings.cmake
  PATHS 
  ${PYQT_SOURCE_DIR}
  ${PYQT_SOURCE_DIR}/..
  ${PYQT_SOURCE_DIR}/../..
  ${PYQT_SOURCE_DIR}/../../..
  $ENV{LOFARSOFT}
  )

if (USG_ROOT)
  include (${USG_ROOT}/devel_common/cmake/CMakeSettings.cmake)
else (USG_ROOT)
  message (FATAL_ERROR "[PYQT] Unable to locate additional CMake scripts!")
endif (USG_ROOT)

## ------------------------------------------------------------------------------
## Check if the program/package/library is already available

#include (FindPYQT)

if (PYQT_FOUND)
  ## report found PYQT installation
  if (PYQT_VERBOSE_CONFIGURE)
    message (STATUS "PYQT_DIR        = ${PYQT_DIR}")
    message (STATUS "PYQT_EXECUTABLE = ${PYQT_EXECUTABLE}")
    message (STATUS "PYQT_VERSION    = ${PYQT_VERSION}")
  endif (PYQT_VERBOSE_CONFIGURE)
  
  string (COMPARE GREATER ${PYQT_VERSION} "1.3.31" PYQT_VERSION_OK)

  if (PYQT_VERSION_OK)
    set (HAVE_PYQT TRUE)
  else (PYQT_VERSION_OK)
    message (STATUS "PYQT version too old - forcing build of newer version!")
    set (HAVE_PYQT FALSE)
  endif (PYQT_VERSION_OK)
else (PYQT_FOUND)
    set (HAVE_PYQT FALSE)
endif (PYQT_FOUND)

## <-- begin build condition --------------------------------------------------->

if (NOT HAVE_PYQT OR PYQT_FORCE_BUILD)
  
  ##__________________________________________________________________
  ## Locate the installation area
  
  find_path (PYQT_prefix release_area.txt
    PATHS
    ${PYQT_SOURCE_DIR}
    PATH_SUFFIXES
    ../release
    ../../release
    ../../../release
    NO_DEFAULT_PATH
    )
  
  if (NOT PYQT_prefix)
    message (FATAL_ERROR "[Pyqt] Unable to locate the installation area")
  endif (NOT PYQT_prefix)
  
  ##__________________________________________________________________
  ## Tools and definitions

  find_program (HAVE_WGET  wget  ${bin_locations} )
  find_program (HAVE_CURL  curl  ${bin_locations} )
  find_program (HAVE_TAR   tar   ${bin_locations} )

#  http://www.riverbankcomputing.co.uk/static/Downloads/PyQt4/PyQt-mac-gpl-4.5.1.tar.gz

  set (PYQT_VERSION "4.4.4")
  if (APPLE)
    set (PYQT_DIRECTORY "PyQt-mac-gpl-${PYQT_VERSION}")
  else (APPLE)
    set (PYQT_DIRECTORY "PyQt-x11-gpl-${PYQT_VERSION}")
  endif (APPLE)
  set (PYQT_TAR_ARCHIVE "${PYQT_DIRECTORY}.tar.gz")
  set (PYQT_URL "http://www.riverbankcomputing.co.uk/static/Downloads/PyQt4/${PYQT_TAR_ARCHIVE}")
  
  ##__________________________________________________________________
  ## Check if a tar-archive with the source code is available already;
  ## if this is not the case we need to fetch it from the USg server.
  
  find_file (have_pyqt_tgz ${PYQT_TAR_ARCHIVE} ${PYQT_SOURCE_DIR})
  
  if (have_pyqt_tgz)
    add_custom_target (download_archive
      COMMAND cp ${have_pyqt_tgz} .
      WORKING_DIRECTORY ${PYQT_BINARY_DIR}
      COMMENT "Copying tar-archive from ${USG_ROOT}/external/pyqt ..."
      )
  else (have_pyqt_tgz)
    if (HAVE_WGET)
      add_custom_target (download_archive
	COMMAND wget -c ${PYQT_URL}
	WORKING_DIRECTORY ${PYQT_BINARY_DIR}
	)
    else (HAVE_WGET)
      if (HAVE_CURL)
	add_custom_target (download_archive ALL
	  COMMAND curl ${PYQT_URL} -o ${PYQT_TAR_ARCHIVE} -C -
	  WORKING_DIRECTORY ${PYQT_BINARY_DIR}
	  )
      else (HAVE_CURL)
	message (FATAL_ERROR "[PYQT] No tool found to download tar-archive!")
      endif (HAVE_CURL)
    endif (HAVE_WGET)
  endif (have_pyqt_tgz)
  
  ##__________________________________________________________________
  ## Expand the tar-archive
  
  add_custom_target (expand_archive
    COMMAND tar -xvzf ${PYQT_TAR_ARCHIVE}
    WORKING_DIRECTORY ${PYQT_BINARY_DIR}
    COMMENT "Expanding tar-archive ..."
    )
  add_dependencies (expand_archive download_archive)

  ##__________________________________________________________________
  ## Configure and build the project

  ## run "./configure"

  add_custom_target (run_configure ALL
    COMMAND python configure.py
    WORKING_DIRECTORY ${PYQT_BINARY_DIR}/${PYQT_DIRECTORY}
    COMMENT "[PYQT] Running configure ..."
    )
  add_dependencies (run_configure expand_archive)
  
  ## run "make"

  add_custom_target (run_make ALL
    COMMAND make
    WORKING_DIRECTORY ${PYQT_BINARY_DIR}/${PYQT_DIRECTORY}
    COMMENT "[PYQT] Starting build ..."
    )
  add_dependencies (run_make run_configure)
  
  ## run "make install"

  add_custom_target (install ALL
    COMMAND make install
    COMMAND rm -rf ${PYQT_BINARY_DIR}/${PYQT_TAR_ARCHIVE}
    COMMAND rm -rf ${PYQT_BINARY_DIR}/${PYQT_DIRECTORY}
    WORKING_DIRECTORY ${PYQT_BINARY_DIR}/${PYQT_DIRECTORY}
    COMMENT "[PYQT] Installing software and cleaning up ..."
    )
  add_dependencies (install run_make)

  ## <-- end build condition ----------------------------------------------------->
  
else (NOT HAVE_PYQT OR PYQT_FORCE_BUILD)
  message (STATUS "Found system-wide installation of Pyqt; skipping rebuild!")
endif (NOT HAVE_PYQT OR PYQT_FORCE_BUILD)
