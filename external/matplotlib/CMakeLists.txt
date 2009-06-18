
## Name of the project
project (MATPLOTLIB)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.5)

## -----------------------------------------------------------------------------
## Only force rebuild of the library from the provided source code, if no system
## installation can be found. In order to handle this properly, we use the CMake
## find script

option (MATPLOTLIB_FORCE_BUILD       "Force rebuild and local installation?"    NO  )
option (MATPLOTLIB_CMAKE_BUILD       "Use CMake to configure and build MATPLOTLIB?"  YES )
option (MATPLOTLIB_VERBOSE_CONFIGURE "Be verbose during configuration?"         NO  )
option (MATPLOTLIB_VERBOSE_BUILD     "Be verbose during build process?"         NO  )

## ------------------------------------------------------------------------------
## Set CMAKE_MODULE_PATH to load custom CMake modules

find_path (USG_ROOT devel_common/cmake/CMakeSettings.cmake
  PATHS 
  ${MATPLOTLIB_SOURCE_DIR}
  ${MATPLOTLIB_SOURCE_DIR}/..
  ${MATPLOTLIB_SOURCE_DIR}/../..
  ${MATPLOTLIB_SOURCE_DIR}/../../..
  $ENV{LOFARSOFT}
  )

if (USG_ROOT)
  include (${USG_ROOT}/devel_common/cmake/CMakeSettings.cmake)
else (USG_ROOT)
  message (FATAL_ERROR "Unable to locate additional CMake scripts!")
endif (USG_ROOT)

## ------------------------------------------------------------------------------
## Check if the program/package/library is already available

if (NOT MATPLOTLIB_VERBOSE_CONFIGURE)
  set (MATPLOTLIB_FIND_QUIETLY  YES )
  set (NUMPY_FIND_QUIETLY  YES )
  set (PYTHON_FIND_QUIETLY YES )
endif (NOT MATPLOTLIB_VERBOSE_CONFIGURE)

include (FindNumPy)
include (FindMatplotlib)

## <-- begin build condition --------------------------------------------------->

if (NOT HAVE_MATPLOTLIB OR MATPLOTLIB_FORCE_BUILD)
  
  ## ------------------------------------------------------------------------------
  ## Locate the installation area
  
  find_path (MATPLOTLIB_prefix release_area.txt
    PATHS
    ${MATPLOTLIB_SOURCE_DIR}
    PATH_SUFFIXES
    ../release
    ../../release
    ../../../release
    NO_DEFAULT_PATH
    )
  
  if (NOT MATPLOTLIB_prefix)
    message (FATAL_ERROR "Unable to locate the installation area")
  endif (NOT MATPLOTLIB_prefix)
  
  ## ------------------------------------------------------------------------------
  ## 

  set (MATPLOTLIB_VERSION "0.98.5.3")
  set (MATPLOTLIB_DIRECTORY "matplotlib-${MATPLOTLIB_VERSION}")
  set (MATPLOTLIB_TAR_ARCHIVE "${MATPLOTLIB_DIRECTORY}.tar.gz")
  
  find_file (have_matplotlib_tgz ${MATPLOTLIB_TAR_ARCHIVE} ${MATPLOTLIB_SOURCE_DIR})
  find_program (tar_executable tar ${bin_locations})

  if (have_matplotlib_tgz AND tar_executable)
    
    ## expand the tar-archive
    add_custom_target (matplotlib_expand
      COMMAND ${tar_executable} -xvzf ${have_matplotlib_tgz}
      WORKING_DIRECTORY ${MATPLOTLIB_BINARY_DIR}
      COMMENT "[Matplotlib] Expanding tar-archive ..."
      )
    
    ## build the package
    add_custom_target (matplotlib_build
      COMMAND ${PYTHON_EXECUTABLE} setup.py build
      WORKING_DIRECTORY ${MATPLOTLIB_BINARY_DIR}/${MATPLOTLIB_DIRECTORY}
      COMMENT "[Matplotlib] Starting build ..."
      )
    add_dependencies (matplotlib_build matplotlib_expand)
    
    ## build and install the package
    add_custom_target (install
      COMMAND ${PYTHON_EXECUTABLE} setup.py install --home=${USG_INSTALL_PREFIX}
      COMMAND rm -rf ${MATPLOTLIB_BINARY_DIR}/*
      WORKING_DIRECTORY ${MATPLOTLIB_BINARY_DIR}/${MATPLOTLIB_DIRECTORY}
      COMMENT "[Matplotlib] Installing package ..."
      )
    add_dependencies (install matplotlib_build)
    
  else (have_matplotlib_tgz AND tar_executable)
    message (STATUS "[Matplotlib] Unable to install - missing tar-archive!")
  endif (have_matplotlib_tgz AND tar_executable)
  
  ## <-- end build condition ----------------------------------------------------->
  
else (NOT HAVE_MATPLOTLIB OR MATPLOTLIB_FORCE_BUILD)
  message (STATUS "Found system-wide installation of MATPLOTLIB; skipping rebuild!")
endif (NOT HAVE_MATPLOTLIB OR MATPLOTLIB_FORCE_BUILD)
