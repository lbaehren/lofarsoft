
## Name of the project
project (NUMERIC)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.5)

## -----------------------------------------------------------------------------
## Only force rebuild of the library from the provided source code, if no system
## installation can be found. In order to handle this properly, we use the CMake
## find script

option (NUMERIC_FORCE_BUILD       "Force rebuild and local installation?"      NO  )
option (NUMERIC_CMAKE_BUILD       "Use CMake to configure and build NUMERIC?"  YES )
option (NUMERIC_VERBOSE_CONFIGURE "Be verbose during configuration?"           NO  )
option (NUMERIC_VERBOSE_BUILD     "Be verbose during build process?"           NO  )

## ------------------------------------------------------------------------------
## Set CMAKE_MODULE_PATH to load custom CMake modules

find_path (USG_ROOT devel_common/cmake/CMakeSettings.cmake
  PATHS 
  ${NUMERIC_SOURCE_DIR}
  ${NUMERIC_SOURCE_DIR}/..
  ${NUMERIC_SOURCE_DIR}/../..
  ${NUMERIC_SOURCE_DIR}/../../..
  $ENV{LOFARSOFT}
  )

if (USG_ROOT)
  include (${USG_ROOT}/devel_common/cmake/CMakeSettings.cmake)
else (USG_ROOT)
  message (FATAL_ERROR "Unable to locate additional CMake scripts!")
endif (USG_ROOT)

## ------------------------------------------------------------------------------
## Check if the program/package/library is already available

if (NOT NUMERIC_VERBOSE_CONFIGURE)
  set (NUMERIC_FIND_QUIETLY YES )
  set (PYTHON_FIND_QUIETLY YES )
endif (NOT NUMERIC_VERBOSE_CONFIGURE)

include (FindNumeric)

## <-- begin build condition --------------------------------------------------->

if (NOT HAVE_NUMERIC OR NUMERIC_FORCE_BUILD)
  
  include (ExternalProject)

  find_file (have_numeric_tgz Numeric-24.2.tar.gz ${NUMERIC_SOURCE_DIR})

  if (have_numeric_tgz)
    ExternalProject_Add (numeric
      PREFIX ${NUMERIC_BINARY_DIR}
      DOWNLOAD_DIR ${NUMERIC_BINARY_DIR}
      SOURCE_DIR ${NUMERIC_BINARY_DIR}
      URL ${have_numeric_tgz}
      BUILD_IN_SOURCE 1
      CONFIGURE_COMMAND ${PYTHON_EXECUTABLE} ${NUMERIC_BINARY_DIR}/setup.py build
      BUILD_COMMAND ${PYTHON_EXECUTABLE} ${NUMERIC_BINARY_DIR}/setup.py install --home=${USG_INSTALL_PREFIX}
      )
  else (have_numeric_tgz)
    status (WARNING "[Numeric] Unable to build project - no sources found!")
  endif (have_numeric_tgz)
  
  ## <-- end build condition ----------------------------------------------------->
  
else (NOT HAVE_NUMERIC OR NUMERIC_FORCE_BUILD)
  add_custom_target (numeric
    WORKING_DIRECTORY ${NUMERIC_BINARY_DIR}
    COMMENT "[Numeric] Found system-wide installation; skipping rebuild!"
    )
endif (NOT HAVE_NUMERIC OR NUMERIC_FORCE_BUILD)
