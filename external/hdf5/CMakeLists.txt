
## ------------------------------------------------------------------------------
#
#  CMake wrapper around the GNU Autotools-based configure/build of the HDF5
#  library.
#
## ------------------------------------------------------------------------------

project (HDF5)

cmake_minimum_required(VERSION 2.4)

## -----------------------------------------------------------------------------
## Only force rebuild of the library from the provided source code, if no system
## installation can be found. In order to handle this properly, we use the CMake
## find script

option (HDF5_FORCE_BUILD "Force rebuild and local installation" 0)

## check where to find the CMake scripts

find_path (HDF5_cmake CMakeSettings.cmake
  PATHS 
  ${HDF5_SOURCE_DIR}
  ${HDF5_SOURCE_DIR}/..
  ${HDF5_SOURCE_DIR}/../..
  ${HDF5_SOURCE_DIR}/../../..
  PATH_SUFFIXES
  devel_common/cmake
  )

if (HDF5_cmake)
  list (APPEND CMAKE_MODULE_PATH ${HDF5_cmake})
else (HDF5_cmake)
  message (FATAL_ERROR "Unable to locate additional CMake scripts!")
endif (HDF5_cmake)

## load CMake modules to search for external components

set (HDF5_FIND_QUIETLY 1)
include (FindHDF5)

## <-- begin build condition --------------------------------------------------->

if (NOT HAVE_HDF5 OR HDF5_FORCE_BUILD)

## ------------------------------------------------------------------------------
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

## ------------------------------------------------------------------------------
## Run the configure script provided along with the distribution

## [1] configure

message (STATUS "Running the HDF5 configure script...")

execute_process (
  WORKING_DIRECTORY ${HDF5_SOURCE_DIR}
  COMMAND ./configure --prefix=${HDF5_prefix} --includedir=${HDF5_prefix}/include/hdf5
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
