
## Name of the project
project (PYTHON)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.8)

## -----------------------------------------------------------------------------
## Only force rebuild of the library from the provided source code, if no system
## installation can be found. In order to handle this properly, we use the CMake
## find script

option (PYTHON_FORCE_BUILD       "Force rebuild and local installation?"  NO  )
option (PYTHON_VERBOSE_CONFIGURE "Be verbose during configuration?"       NO  )
option (PYTHON_VERBOSE_BUILD     "Be verbose during build process?"       NO  )

## ------------------------------------------------------------------------------
## Set CMAKE_MODULE_PATH to load custom CMake modules

find_path (USG_ROOT devel_common/cmake/CMakeSettings.cmake
  PATHS 
  ${PYTHON_SOURCE_DIR}
  ${PYTHON_SOURCE_DIR}/..
  ${PYTHON_SOURCE_DIR}/../..
  ${PYTHON_SOURCE_DIR}/../../..
  $ENV{LOFARSOFT}
  )

if (USG_ROOT)
  include (${USG_ROOT}/devel_common/cmake/CMakeSettings.cmake)
else (USG_ROOT)
  message (FATAL_ERROR "[Python] Unable to locate additional CMake scripts!")
endif (USG_ROOT)

## ------------------------------------------------------------------------------
## Check if the program/package/library is already available

if (NOT PYTHON_VERBOSE_CONFIGURE)
  set (PYTHON_FIND_QUIETLY YES )
endif (NOT PYTHON_VERBOSE_CONFIGURE)

include (FindPython)

## <-- begin build condition --------------------------------------------------->

if (NOT HAVE_PYTHON OR PYTHON_FORCE_BUILD)
  
  ##__________________________________________________________________
  ## Additional CMake modules

  include (ExternalProject)

  ##__________________________________________________________________
  ## Configure and build the project

  set (PYTHON_SOURCE_VERSION "2.6.4")
  set (PYTHON_TAR_ARCHIVE "Python-${PYTHON_SOURCE_VERSION}.tgz")

  find_file (HAVE_PYTHON_SOURCE ${PYTHON_TAR_ARCHIVE}
    PATHS ${PYTHON_SOURCE_DIR}
    )
  
  if (HAVE_PYTHON_SOURCE)
    ExternalProject_Add (python
      PREFIX ${PYTHON_BINARY_DIR}
      DOWNLOAD_DIR ${PYTHON_BINARY_DIR}
      SOURCE_DIR ${PYTHON_BINARY_DIR}
      URL ${HAVE_PYTHON_SOURCE}
      BUILD_IN_SOURCE 1
      CONFIGURE_COMMAND ${PYTHON_BINARY_DIR}/configure --prefix=${USG_INSTALL_PREFIX} --enable-framework
      )
  else (HAVE_PYTHON_SOURCE)
    message (STATUS "[Python] Unable to build package - missing source archive!")
  endif (HAVE_PYTHON_SOURCE)
  
  ## <-- end build condition ----------------------------------------------------->
  
else (NOT HAVE_PYTHON OR PYTHON_FORCE_BUILD)
  add_custom_target (python
    WORKING_DIRECTORY ${PYTHON_BINARY_DIR}
    COMMENT "[Python] Found system-wide installation; skipping rebuild!"
    )
  message (STATUS "[Python] Found system-wide installation; skipping rebuild!")
endif (NOT HAVE_PYTHON OR PYTHON_FORCE_BUILD)
