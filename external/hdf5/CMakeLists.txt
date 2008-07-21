
## ------------------------------------------------------------------------------
#
#  CMake wrapper around the GNU Autotools-based configure/build of the HDF5
#  library.
#
## ------------------------------------------------------------------------------

## Name of the project
project (HDF5)

## Required version of CMake
cmake_minimum_required(VERSION 2.5)

## -----------------------------------------------------------------------------
## Only force rebuild of the library from the provided source code, if no system
## installation can be found. In order to handle this properly, we use the CMake
## find script

option (HDF5_FORCE_BUILD "Force rebuild and local installation" 0)

## ------------------------------------------------------------------------------
## Set CMAKE_MODULE_PATH to load custom CMake modules

find_path (USG_ROOT devel_common/cmake/CMakeSettings.cmake
  PATHS 
  ${HDF5_SOURCE_DIR}
  ${HDF5_SOURCE_DIR}/..
  ${HDF5_SOURCE_DIR}/../..
  ${HDF5_SOURCE_DIR}/../../..
  ENV LOFARSOFT
  )

if (USG_ROOT)
  include (${USG_ROOT}/devel_common/cmake/CMakeSettings.cmake)
else (USG_ROOT)
  message (FATAL_ERROR "Unable to locate additional CMake scripts!")
endif (USG_ROOT)

## ------------------------------------------------------------------------------
## Check for system-wide HDF5 library installation

set (HDF5_FIND_QUIETLY 1)
include (FindHDF5)

## if the there is a system-wide installation, it is the correct version of the
## library? If not we need to force a build from the provided source tree.

if (HDF5_MAJOR_VERSION LESS 1 OR HDF5_MINOR_VERSION LESS 8)
  message (STATUS "System-wide installation of HDF5 library found - but too old!")
  set (HDF5_FORCE_BUILD TRUE)
endif (HDF5_MAJOR_VERSION LESS 1 OR HDF5_MINOR_VERSION LESS 8)

## <-- begin build condition --------------------------------------------------->

if (NOT HAVE_HDF5 OR HDF5_FORCE_BUILD)
  
  ## ----------------------------------------------------------------------------
  ## Locate the installation area
  
  find_path (HDF5_prefix release_area.txt
    PATHS
    ${HDF5_SOURCE_DIR}
    PATH_SUFFIXES
    ../release
    ../../release
    NO_DEFAULT_PATH
    )
  
  if (NOT HDF5_prefix)
    message (FATAL_ERROR "Unable to locate the installation area")
  endif (NOT HDF5_prefix)
  
  ## ----------------------------------------------------------------------------
  ## Compression/deflation libraries

  include (FindSZIP)
  include (FindZLIB)

  ## ----------------------------------------------------------------------------
  ## Run the configure script provided along with the distribution
  
  ## [1] configure
  
  message (STATUS "Running the HDF5 configure script...")
  
  execute_process (
    WORKING_DIRECTORY ${HDF5_SOURCE_DIR}
    COMMAND ./configure --prefix=${HDF5_prefix} --includedir=${HDF5_prefix}/include/hdf5 --enable-shared-=no
    TIMEOUT 300
    ERROR_VARIABLE error_configure
    ERROR_FILE error.log
    )
  
  if (error_configure)
    message (STATUS "There was an error running the HDF5 configure script!")
    message (SEND_ERROR "${error_configure}")
  endif (error_configure)
  
  ## [2] make
  
  message (STATUS "Building HDF5 library...")
  
  execute_process (
    WORKING_DIRECTORY ${HDF5_SOURCE_DIR}
    COMMAND make 
    TIMEOUT 3600
    ERROR_VARIABLE error_make
    ERROR_FILE error.log
    )
  
  if (error_make)
    message (STATUS "There was an error building/installing the HDF5 library!")
    message (SEND_ERROR "${error_make}")
  endif (error_make)
  
  ## [3] make install
  
  if (NOT error_make)
    
    message (STATUS "Installing HDF5 library...")
    
    execute_process (
      WORKING_DIRECTORY ${HDF5_SOURCE_DIR}
      COMMAND make install
      TIMEOUT 180
      ERROR_VARIABLE error_make_install
      ERROR_FILE error.log
      )
    
    if (error_make_install)
      message (STATUS "There was an error building/installing the HDF5 library!")
      message (SEND_ERROR "${error_make_install}")
    endif (error_make_install)
    
  endif (NOT error_make)
  
  ## <-- end build condition ----------------------------------------------------->
  
else (NOT HAVE_HDF5 OR HDF5_FORCE_BUILD)
  message (STATUS "Found system-wide installation of HDF5; skipping rebuild!")
endif (NOT HAVE_HDF5 OR HDF5_FORCE_BUILD)
