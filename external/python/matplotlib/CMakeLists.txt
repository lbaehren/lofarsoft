
## ------------------------------------------------------------------------------
#
#  CMake wrapper around the GNU Autotools-based configure/build of the PYTHON_NUMARRAY
#  library.
#
## ------------------------------------------------------------------------------

project (PYTHON_MATPLOTLIB)

## -----------------------------------------------------------------------------
## Only force rebuild of the library from the provided source code, if no system
## installation can be found. In order to handle this properly, we use the CMake
## find script

option (PYTHON_MATPLOTLIB_FORCE_BUILD "Force rebuild and local installation" 0)

## check where to find the CMake scripts

find_path (PYTHON_MATPLOTLIB_cmake FindBlitz.cmake
  PATHS 
  ${PYTHON_MATPLOTLIB_SOURCE_DIR}
  ${PYTHON_MATPLOTLIB_SOURCE_DIR}/..
  ${PYTHON_MATPLOTLIB_SOURCE_DIR}/../..
  ${PYTHON_MATPLOTLIB_SOURCE_DIR}/../../..
  PATH_SUFFIXES
  devel_common/cmake
  )

if (PYTHON_MATPLOTLIB_cmake)
  include (${PYTHON_MATPLOTLIB_cmake}/FindBlitz.cmake)
endif (PYTHON_MATPLOTLIB_cmake)

## <-- begin build condition --------------------------------------------------->

if (NOT HAVE_PYTHON_MATPLOTLIB OR PYTHON_MATPLOTLIB_FORCE_BUILD)

## ------------------------------------------------------------------------------
## Locate the installation area

find_path (PYTHON_MATPLOTLIB_prefix release_area.txt
  PATHS
  ${PYTHON_MATPLOTLIB_SOURCE_DIR}
  PATH_SUFFIXES
  ../release
  ../../release
  ../../../release
  NO_DEFAULT_PATH
  )

if (NOT PYTHON_MATPLOTLIB_prefix)
  message (FATAL_ERROR "Unable to locate the installation area")
endif (NOT PYTHON_MATPLOTLIB_prefix)

## ------------------------------------------------------------------------------
## Run the configure script provided along with the distribution

message (STATUS "Running the Python setup script for this module ...")

execute_process (
  WORKING_DIRECTORY ${PYTHON_MATPLOTLIB_SOURCE_DIR}
  COMMAND python setup.py build install --home=${PYTHON_MATPLOTLIB_prefix}
  TIMEOUT 1800
  ERROR_VARIABLE error_configure
  ERROR_FILE error.log
  )

if (error_configure)
  message (STATUS "There was an error running the PYTHON_MATPLOTLIB configure script!")
  message (SEND_ERROR "${error_configure}")
endif (error_configure)

## <-- end build condition ----------------------------------------------------->

else (NOT HAVE_PYTHON_MATPLOTLIB OR PYTHON_MATPLOTLIB_FORCE_BUILD)
  message (STATUS "Found system-wide installation of PYTHON_MATPLOTLIB; skipping rebuild!")
endif (NOT HAVE_PYTHON_MATPLOTLIB OR PYTHON_MATPLOTLIB_FORCE_BUILD)
