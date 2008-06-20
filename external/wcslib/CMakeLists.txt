##------------------------------------------------------------------------------
## $Id:: CMakeLists.txt 489 2007-08-01 11:40:38Z baehren                       $
##------------------------------------------------------------------------------

## -----------------------------------------------------------------------------
## Project name and version number number

## Name of the project
project (WCSLIB)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.5)

set (WCSLIB_VERSION_MAJOR 4)
set (WCSLIB_VERSION_MINOR 2)
set (WCSLIB_VERSION "${WCSLIB_VERSION_MAJOR}.${WCSLIB_VERSION_MINOR}")

## -----------------------------------------------------------------------------
## User defined build/install options

option (WCSLIB_PYTHON_WRAPPER "Generate Python wrapper for WCSLIB?" 0)
option (WCSLIB_VERBOSE_CONFIGURE "Verbose output during configuration?" 0)
option (WCSLIB_FORCE_BUILD "Force rebuild and local installation" 0)

## ------------------------------------------------------------------------------
## Set CMAKE_MODULE_PATH to load custom CMake modules

if (NOT USG_ROOT)
  find_path (USG_ROOT devel_common/cmake/CMakeSettings.cmake
    PATHS 
    ${WCSLIB_SOURCE_DIR}
    ${WCSLIB_SOURCE_DIR}/..
    ${WCSLIB_SOURCE_DIR}/../..
    ${WCSLIB_SOURCE_DIR}/../../..
    ENV LOFARSOFT
    )
  
  if (USG_ROOT)
    include (${USG_ROOT}/devel_common/cmake/CMakeSettings.cmake)
  else (USG_ROOT)
    message (FATAL_ERROR "Unable to locate additional CMake scripts!")
  endif (USG_ROOT)
endif (NOT USG_ROOT)

if (NOT WCSLIB_VERBOSE_CONFIGURE)
  set (CFITSIO_FIND_QUIETLY 1)
  set (BOOST_FIND_QUIETLY 1)
  set (PYTHON_FIND_QUIETLY 1)
  set (WCSLIB_FIND_QUIETLY 1)
endif (NOT WCSLIB_VERBOSE_CONFIGURE)

include (FindCFITSIO)
include (FindBoost)
include (FindPython)
include (FindWCS)

## <-- begin build condition --------------------------------------------------->

if (NOT HAVE_WCSLIB OR WCSLIB_FORCE_BUILD)
  
  ## ----------------------------------------------------------------------------
  ## Check for additional tools which are required
  
  include (CTest)
  
  find_library (libm m)
  find_program (flex_bin flex)
  
  ## ----------------------------------------------------------------------------
  ## Subdirectories
  
  add_subdirectory (${WCSLIB_SOURCE_DIR}/C)
  add_subdirectory (${WCSLIB_SOURCE_DIR}/FORTRAN)
  add_subdirectory (${WCSLIB_SOURCE_DIR}/pgsbox)
  
  if (WCSLIB_PYTHON_WRAPPER AND HAVE_PYTHON)
    add_subdirectory (${WCSLIB_SOURCE_DIR}/Python)
  endif (WCSLIB_PYTHON_WRAPPER AND HAVE_PYTHON)
  
  ## ----------------------------------------------------------------------------
  ## Installation
  
  find_path (WCSLIB_INSTALL_PREFIX release_area.txt
    PATHS
    ${WCSLIB_SOURCE_DIR}
    PATH_SUFFIXES
    ../release
    ../../release
    NO_DEFAULT_PATH
    )
  
  if (WCSLIB_INSTALL_PREFIX)
    message (STATUS "Installation area located for package WCSLIB.")
    set (CMAKE_INSTALL_PREFIX ${WCSLIB_INSTALL_PREFIX}) 
  endif (WCSLIB_INSTALL_PREFIX)

## -----------------------------------------------------------------------------
## Configuration summary

message (STATUS " ")
message (STATUS "============================================================")
message (STATUS " ")
message (STATUS "[WCSLIB] Configuration summary")
message (STATUS "PYTHON_EXECUTABLE = ${PYTHON_EXECUTABLE} ")
message (STATUS "PYTHON_INCLUDES   = ${PYTHON_INCLUDES} ")
message (STATUS "PYTHON_LIBRARIES  = ${PYTHON_LIBRARIES} ")
message (STATUS "HAVE_PYCONFIG_H   = ${HAVE_PYCONFIG_H}")
message (STATUS "WCSLIB_SOURCE_DIR = ${WCSLIB_SOURCE_DIR}")
message (STATUS "WCSLIB_BINARY_DIR = ${WCSLIB_BINARY_DIR}")
message (STATUS " ")
message (STATUS "============================================================")
message (STATUS " ")

## <-- end build condition ----------------------------------------------------->

else (NOT HAVE_WCSLIB OR WCSLIB_FORCE_BUILD)
  message (STATUS "Found system-wide installation of WCSLIB; skipping rebuild!")
endif (NOT HAVE_WCSLIB OR WCSLIB_FORCE_BUILD)
