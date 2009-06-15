
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
  
  ## ------------------------------------------------------------------------------
  ## Locate the installation area
  
  find_path (NUMERIC_prefix release_area.txt
    PATHS
    ${NUMERIC_SOURCE_DIR}
    PATH_SUFFIXES
    ../release
    ../../release
    ../../../release
    NO_DEFAULT_PATH
    )
  
  if (NOT NUMERIC_prefix)
    message (FATAL_ERROR "Unable to locate the installation area")
  endif (NOT NUMERIC_prefix)
  
  ## ------------------------------------------------------------------------------
  ## 

  set (NUMERIC_VERSION "24.2")
  set (NUMERIC_DIRECTORY "Numeric-${NUMERIC_VERSION}")
  set (NUMERIC_TAR_ARCHIVE "${NUMERIC_DIRECTORY}.tar.gz")
  
  find_file (have_numeric_tgz ${NUMERIC_TAR_ARCHIVE} ${NUMERIC_SOURCE_DIR})
  find_program (tar_executable tar ${bin_locations})

  if (have_numeric_tgz AND tar_executable)
    
    ## expand the tar-archive
    add_custom_target (numeric_expand
      COMMAND ${tar_executable} -xvzf ${have_numeric_tgz}
      WORKING_DIRECTORY ${NUMERIC_BINARY_DIR}
      COMMENT "[Numeric] Expanding tar-archive ..."
      )
    
    ## build the package
    add_custom_target (numeric_build
      COMMAND ${PYTHON_EXECUTABLE} setup.py build
      WORKING_DIRECTORY ${NUMERIC_BINARY_DIR}/${NUMERIC_DIRECTORY}
      COMMENT "[Numeric] Starting build ..."
      )
    add_dependencies (numeric_build numeric_expand)
    
    ## build and install the package
    add_custom_target (install
      COMMAND ${PYTHON_EXECUTABLE} setup.py install --home=${USG_INSTALL_PREFIX}
      COMMAND rm -rf ${NUMERIC_BINARY_DIR}/${NUMERIC_DIRECTORY}
      COMMAND rm -rf ${NUMERIC_BINARY_DIR}/CMake*
      WORKING_DIRECTORY ${NUMERIC_BINARY_DIR}/${NUMERIC_DIRECTORY}
      COMMENT "[Numeric] Installing package ..."
      )
    add_dependencies (install numeric_build)
    
  else (have_numeric_tgz AND tar_executable)
    message (STATUS "[Numeric] Unable to install - missing tar-archive!")
  endif (have_numeric_tgz AND tar_executable)
  
  ## <-- end build condition ----------------------------------------------------->
  
else (NOT HAVE_NUMERIC OR NUMERIC_FORCE_BUILD)
  message (STATUS "Found system-wide installation of NUMERIC; skipping rebuild!")
endif (NOT HAVE_NUMERIC OR NUMERIC_FORCE_BUILD)
