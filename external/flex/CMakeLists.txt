
## ------------------------------------------------------------------------------
#
#  CMake wrapper around the GNU Autotools-based configure/build of the FLEX
#  library.
#
## ------------------------------------------------------------------------------

## Name of the project
project (FLEX)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.5)

## -----------------------------------------------------------------------------
## Only force rebuild of the library from the provided source code, if no system
## installation can be found. In order to handle this properly, we use the CMake
## find script

option (FLEX_FORCE_BUILD "Force rebuild and local installation" 0)

## ------------------------------------------------------------------------------
## Set CMAKE_MODULE_PATH to load custom CMake modules

find_path (USG_ROOT devel_common/cmake/CMakeSettings.cmake
  PATHS 
  ${FLEX_SOURCE_DIR}
  ${FLEX_SOURCE_DIR}/..
  ${FLEX_SOURCE_DIR}/../..
  ${FLEX_SOURCE_DIR}/../../..
  ENV LOFARSOFT
  )

if (USG_ROOT)
  include (${USG_ROOT}/devel_common/cmake/CMakeSettings.cmake)
else (USG_ROOT)
  message (FATAL_ERROR "Unable to locate additional CMake scripts!")
endif (USG_ROOT)

## ------------------------------------------------------------------------------
## Check if the program is available already

find_program (HAVE_FLEX flex PATHS ${bin_locations})

## <-- begin build condition --------------------------------------------------->

if (NOT HAVE_FLEX OR FLEX_FORCE_BUILD)

## ------------------------------------------------------------------------------
## Locate the installation area

find_path (FLEX_prefix release_area.txt
  PATHS
  ${FLEX_SOURCE_DIR}
  PATH_SUFFIXES
  ../release
  ../../release
  NO_DEFAULT_PATH
  )

if (NOT FLEX_prefix)
  message (FATAL_ERROR "Unable to locate the installation area")
endif (NOT FLEX_prefix)

## ------------------------------------------------------------------------------
## Run the configure script provided along with the distribution

## [1] configure

message (STATUS "Running the FLEX configure script...")

execute_process (
  WORKING_DIRECTORY ${FLEX_SOURCE_DIR}
  COMMAND ./configure --prefix=${FLEX_prefix}
  TIMEOUT 300
  ERROR_VARIABLE error_configure
  ERROR_FILE error.log
  )

if (error_configure)
  message (STATUS "There was an error running the FLEX configure script!")
  message (SEND_ERROR "${error_configure}")
endif (error_configure)

## [2] make

message (STATUS "Building FLEX library...")

execute_process (
  WORKING_DIRECTORY ${FLEX_SOURCE_DIR}
  COMMAND make 
  TIMEOUT 3600
  ERROR_VARIABLE error_make
  ERROR_FILE error.log
  )

if (error_make)
  message (STATUS "There was an error building/installing the FLEX library!")
  message (SEND_ERROR "${error_make}")
endif (error_make)

## [3] make install

if (NOT error_make)

message (STATUS "Installing FLEX library...")

execute_process (
  WORKING_DIRECTORY ${FLEX_SOURCE_DIR}
  COMMAND make install
  TIMEOUT 180
  ERROR_VARIABLE error_make_install
  ERROR_FILE error.log
  )

if (error_make_install)
  message (STATUS "There was an error building/installing the FLEX library!")
  message (SEND_ERROR "${error_make_install}")
endif (error_make_install)

endif (NOT error_make)

## <-- end build condition ----------------------------------------------------->

else (NOT HAVE_FLEX OR FLEX_FORCE_BUILD)
  message (STATUS "Found system-wide installation of FLEX; skipping rebuild!")
endif (NOT HAVE_FLEX OR FLEX_FORCE_BUILD)
