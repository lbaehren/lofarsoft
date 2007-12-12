
## ------------------------------------------------------------------------------
## $Id::                                                                        $
## ------------------------------------------------------------------------------
##
##  CMake wrapper around the GNU Autotools-based configure/build of the
##  PYTHON_NUMPY library.
## 
##  NumPy - replacement of Numeric Python that adds the features of numarray.
##
## ------------------------------------------------------------------------------

project (PYTHON_NUMPY)

## -----------------------------------------------------------------------------
## Only force rebuild of the library from the provided source code, if no system
## installation can be found. In order to handle this properly, we use the CMake
## find script

option (PYTHON_NUMPY_FORCE_BUILD "Force rebuild and local installation" 0)

## check where to find the CMake scripts

find_path (PYTHON_NUMPY_cmake FindPython.cmake
  PATHS 
  ${PYTHON_NUMPY_SOURCE_DIR}
  ${PYTHON_NUMPY_SOURCE_DIR}/..
  ${PYTHON_NUMPY_SOURCE_DIR}/../..
  ${PYTHON_NUMPY_SOURCE_DIR}/../../..
  PATH_SUFFIXES
  devel_common/cmake
  )

if (PYTHON_NUMPY_cmake)
  include (${PYTHON_NUMPY_cmake}/FindPython.cmake)
endif (PYTHON_NUMPY_cmake)

## <-- begin build condition --------------------------------------------------->

if (NOT HAVE_PYTHON_NUMPY OR PYTHON_NUMPY_FORCE_BUILD)

## ------------------------------------------------------------------------------
## Locate the installation area

find_path (PYTHON_NUMPY_prefix release_area.txt
  PATHS
  ${PYTHON_NUMPY_SOURCE_DIR}
  PATH_SUFFIXES
  ../release
  ../../release
  ../../../release
  NO_DEFAULT_PATH
  )

if (NOT PYTHON_NUMPY_prefix)
  message (FATAL_ERROR "Unable to locate the installation area")
endif (NOT PYTHON_NUMPY_prefix)

## ------------------------------------------------------------------------------
## Run the configure script provided along with the distribution
##
## python setup.py install --gencode
##                         --install-lib=<dir>
##                         --install-headers=<dir>/numpy
##

message (STATUS "Running the Python setup script for this module ...")

if(UNIX)
  
  execute_process (
    WORKING_DIRECTORY ${PYTHON_NUMPY_SOURCE_DIR}
    COMMAND python setup.py install --prefix=${PYTHON_NUMPY_prefix}
    TIMEOUT 1800
    ERROR_VARIABLE error_configure
    ERROR_FILE error.log
    )
  
  if (error_configure)
    message (STATUS "There was an error running the Numpy configure script!")
    message (SEND_ERROR "${error_configure}")
  endif (error_configure)

endif(UNIX)
  
## <-- end build condition ----------------------------------------------------->

else (NOT HAVE_PYTHON_NUMPY OR PYTHON_NUMPY_FORCE_BUILD)
  message (STATUS "Found system-wide installation of PYTHON_NUMPY; skipping rebuild!")
endif (NOT HAVE_PYTHON_NUMPY OR PYTHON_NUMPY_FORCE_BUILD)
