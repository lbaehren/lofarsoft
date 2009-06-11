
## Name of the project
project (NUMPY)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.5)

## -----------------------------------------------------------------------------
## Only force rebuild of the library from the provided source code, if no system
## installation can be found. In order to handle this properly, we use the CMake
## find script

option (NUMPY_FORCE_BUILD       "Force rebuild and local installation?"    NO  )
option (NUMPY_CMAKE_BUILD       "Use CMake to configure and build NUMPY?"  YES )
option (NUMPY_VERBOSE_CONFIGURE "Be verbose during configuration?"         NO  )
option (NUMPY_VERBOSE_BUILD     "Be verbose during build process?"         NO  )

## ------------------------------------------------------------------------------
## Set CMAKE_MODULE_PATH to load custom CMake modules

find_path (USG_ROOT devel_common/cmake/CMakeSettings.cmake
  PATHS 
  ${NUMPY_SOURCE_DIR}
  ${NUMPY_SOURCE_DIR}/..
  ${NUMPY_SOURCE_DIR}/../..
  ${NUMPY_SOURCE_DIR}/../../..
  $ENV{LOFARSOFT}
  )

if (USG_ROOT)
  include (${USG_ROOT}/devel_common/cmake/CMakeSettings.cmake)
else (USG_ROOT)
  message (FATAL_ERROR "Unable to locate additional CMake scripts!")
endif (USG_ROOT)

## ------------------------------------------------------------------------------
## Check if the program/package/library is already available

if (NOT NUMPY_VERBOSE_CONFIGURE)
  set (NUMPY_FIND_QUIETLY  YES )
  set (PYTHON_FIND_QUIETLY YES )
endif (NOT NUMPY_VERBOSE_CONFIGURE)

include (FindNumPy)

message (STATUS "-- NumPy library   = ${NUMPY_LIBRARIES}")

## <-- begin build condition --------------------------------------------------->

if (NOT HAVE_NUMPY OR NUMPY_FORCE_BUILD)
  
  ## ------------------------------------------------------------------------------
  ## Locate the installation area
  
  find_path (NUMPY_prefix release_area.txt
    PATHS
    ${NUMPY_SOURCE_DIR}
    PATH_SUFFIXES
    ../release
    ../../release
    ../../../release
    NO_DEFAULT_PATH
    )
  
  if (NOT NUMPY_prefix)
    message (FATAL_ERROR "Unable to locate the installation area")
  endif (NOT NUMPY_prefix)
  
  ## ------------------------------------------------------------------------------
  ## 

  set (NUMPY_VERSION "1.3.0")
  set (NUMPY_DIRECTORY "numpy-${NUMPY_VERSION}")
  set (NUMPY_TAR_ARCHIVE "${NUMPY_DIRECTORY}.tar.gz")
  
  find_file (have_numpy_tgz ${NUMPY_TAR_ARCHIVE} ${NUMPY_SOURCE_DIR})
  find_program (tar_executable tar ${bin_locations})

  if (have_numpy_tgz AND tar_executable)
    
    ## expand the tar-archive
    add_custom_target (numpy_expand
      COMMAND ${tar_executable} -xvzf ${have_numpy_tgz}
      WORKING_DIRECTORY ${NUMPY_BINARY_DIR}
      COMMENT "[NumPy] Expanding tar-archive ..."
      )
    
    ## build the package
    add_custom_target (numpy_build
      COMMAND ${PYTHON_EXECUTABLE} setup.py build
      WORKING_DIRECTORY ${NUMPY_BINARY_DIR}/${NUMPY_DIRECTORY}
      COMMENT "[NumPy] Starting build ..."
      )
    add_dependencies (numpy_build numpy_expand)
    
    ## build and install the package
    add_custom_target (install
      COMMAND ${PYTHON_EXECUTABLE} setup.py install --home=${USG_INSTALL_PREFIX}
      COMMAND rm -rf ${NUMPY_BINARY_DIR}/*
      WORKING_DIRECTORY ${NUMPY_BINARY_DIR}/${NUMPY_DIRECTORY}
      COMMENT "[NumPy] Installing package ..."
      )
    add_dependencies (install numpy_build)
    
  else (have_numpy_tgz AND tar_executable)
    message (STATUS "[NumPy] Unable to install - missing tar-archive!")
  endif (have_numpy_tgz AND tar_executable)
  
  ## <-- end build condition ----------------------------------------------------->
  
else (NOT HAVE_NUMPY OR NUMPY_FORCE_BUILD)
  message (STATUS "Found system-wide installation of NUMPY; skipping rebuild!")
endif (NOT HAVE_NUMPY OR NUMPY_FORCE_BUILD)
