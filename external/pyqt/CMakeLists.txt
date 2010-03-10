
## Name of the project
project (PYQT)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.8)

## -----------------------------------------------------------------------------
## Only force rebuild of the library from the provided source code, if no system
## installation can be found. In order to handle this properly, we use the CMake
## find script

option (PYQT_FORCE_BUILD       "Force rebuild and local installation?"    NO  )
option (PYQT_CMAKE_BUILD       "Use CMake to configure and build IT++?"   YES )
option (PYQT_VERBOSE_CONFIGURE "Be verbose during configuration?"         NO  )
option (PYQT_VERBOSE_BUILD     "Be verbose during build process?"         NO  )

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
  message (FATAL_ERROR "Unable to locate additional CMake scripts!")
endif (USG_ROOT)

## ------------------------------------------------------------------------------
## Check if the program/package/library is already available

if (NOT PYQT_VERBOSE_CONFIGURE)
  set (PYTHON_FIND_QUIETLY YES )
  set (QT_FIND_QUIETLY     YES )
  set (QT4_FIND_QUIETLY     YES )
endif (NOT PYQT_VERBOSE_CONFIGURE)

include (FindPython)
include (FindQt4)

## <-- begin build condition --------------------------------------------------->

if (NOT HAVE_PYQT OR PYQT_FORCE_BUILD)
  
  set (PYQT_VERSION "4.7")
  
  if (APPLE)
    set (PYQT_TAR_ARCHIVE "PyQt-mac-gpl-${PYQT_VERSION}.tar.gz")
  else (APPLE)
    set (PYQT_TAR_ARCHIVE "PyQt-x11-gpl-${PYQT_VERSION}.tar.gz")
  endif (APPLE)
  
  set (PYQT_URL "http://www.riverbankcomputing.co.uk/static/Downloads/PyQt4/${PYQT_TAR_ARCHIVE}")

  ##__________________________________________________________________
  ## Build the project
  
  include (ExternalProject)
  
  find_file (have_pyqt_tgz ${PYQT_TAR_ARCHIVE}  ${PYQT_SOURCE_DIR})
  
  if (have_pyqt_tgz)
    ExternalProject_Add (pyqt
      PREFIX ${PYQT_BINARY_DIR}
      DOWNLOAD_DIR ${PYQT_BINARY_DIR}
      SOURCE_DIR ${PYQT_BINARY_DIR}
      URL ${have_pyqt_tgz}
      BUILD_IN_SOURCE 1
      CONFIGURE_COMMAND ${PYTHON_EXECUTABLE} ${PYQT_BINARY_DIR}/configure.py -d ${CMAKE_INSTALL_PREFIX}/lib/python
      )
  else (have_pyqt_tgz)
    ExternalProject_Add (pyqt
      PREFIX ${PYQT_BINARY_DIR}
      DOWNLOAD_DIR ${PYQT_BINARY_DIR}
      SOURCE_DIR ${PYQT_BINARY_DIR}
      URL ${PYQT_URL}
      BUILD_IN_SOURCE 1
      CONFIGURE_COMMAND ${PYTHON_EXECUTABLE} ${PYQT_BINARY_DIR}/configure.py -d ${CMAKE_INSTALL_PREFIX}/lib/python
      )
  endif (have_pyqt_tgz)
  
  ## <-- end build condition ----------------------------------------------------->
  
else (NOT HAVE_PYQT OR PYQT_FORCE_BUILD)
  message (STATUS "[PyQt] Found system-wide installation; skipping rebuild!")
endif (NOT HAVE_PYQT OR PYQT_FORCE_BUILD)

## ------------------------------------------------------------------------------
## Configuration summary

if (PYQT_VERBOSE_CONFIGURE) 

  message ("--------------------------------------------------")
  message ("   [PyQt] Configuration summary.")
  message (" ")
  message (" PyQt version = ${PYQT_VERSION}")
  message ("--------------------------------------------------")

endif (PYQT_VERBOSE_CONFIGURE)
