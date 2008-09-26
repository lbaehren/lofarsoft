
## ------------------------------------------------------------------------------
##
##  CMake wrapper around the GNU Autotools-based configure/build of the
##  PYTHON_NUMARRAY library.
## 
##  Numarray, replacement for Numeric (a.k.a. numpy)
##
## ------------------------------------------------------------------------------

project (PYTHON_NUMARRAY)

## -----------------------------------------------------------------------------
## Only force rebuild of the library from the provided source code, if no system
## installation can be found. In order to handle this properly, we use the CMake
## find script

option (PYTHON_NUMARRAY_FORCE_BUILD "Force rebuild and local installation" 0)

## check where to find the CMake scripts

find_path (PYTHON_NUMARRAY_cmake FindPython.cmake
  PATHS 
  ${PYTHON_NUMARRAY_SOURCE_DIR}
  ${PYTHON_NUMARRAY_SOURCE_DIR}/..
  ${PYTHON_NUMARRAY_SOURCE_DIR}/../..
  ${PYTHON_NUMARRAY_SOURCE_DIR}/../../..
  PATH_SUFFIXES
  devel_common/cmake
  )

if (PYTHON_NUMARRAY_cmake)
  include (${PYTHON_NUMARRAY_cmake}/FindPython.cmake)
endif (PYTHON_NUMARRAY_cmake)

## <-- begin build condition --------------------------------------------------->

if (NOT HAVE_PYTHON_NUMARRAY OR PYTHON_NUMARRAY_FORCE_BUILD)
  
  ## ----------------------------------------------------------------------------
  ## Locate the installation area
  
  find_path (PYTHON_NUMARRAY_prefix release_area.txt
    PATHS
    ${PYTHON_NUMARRAY_SOURCE_DIR}
    PATH_SUFFIXES
    ../release
    ../../release
    ../../../release
    NO_DEFAULT_PATH
    )
  
  if (NOT PYTHON_NUMARRAY_prefix)
    message (FATAL_ERROR "Unable to locate the installation area")
  endif (NOT PYTHON_NUMARRAY_prefix)
  
  ## ----------------------------------------------------------------------------
  ## Run the configure script provided along with the distribution
  ##
  ## python setup.py install --gencode
  ##                         --install-lib=<dir>
  ##                         --install-headers=<dir>/numarray
  ##
  
  message (STATUS "Running the Python setup script for this module ...")
  
  if(UNIX)
    
    execute_process (
      WORKING_DIRECTORY ${PYTHON_NUMARRAY_SOURCE_DIR}
      COMMAND ${PYTHON_EXECUTABLE} setup.py build install --home=${PYTHON_NUMARRAY_prefix}
      TIMEOUT 1800
      ERROR_VARIABLE error_configure
      ERROR_FILE error.log
      )
    
    if (error_configure)
      message (STATUS "There was an error running the Numarray configure script!")
      message (SEND_ERROR "${error_configure}")
    endif (error_configure)
    
  endif(UNIX)
  
  ## <-- end build condition ----------------------------------------------------->
  
else (NOT HAVE_PYTHON_NUMARRAY OR PYTHON_NUMARRAY_FORCE_BUILD)
  message (STATUS "Found system-wide installation of PYTHON_NUMARRAY; skipping rebuild!")
endif (NOT HAVE_PYTHON_NUMARRAY OR PYTHON_NUMARRAY_FORCE_BUILD)
