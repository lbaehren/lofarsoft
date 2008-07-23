
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

## Name of the project
project (NUMPY)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.5)

## -----------------------------------------------------------------------------
## Options

option (NUMPY_FORCE_BUILD    "Force rebuild and local installation?"   NO )
option (NUMPY_CMAKE_BUILD    "Use CMake to configure and build NumPy?" NO )
option (NUMPY_ENABLE_TESTING "Build and enable execution of tests?"    NO )

## -----------------------------------------------------------------------------
## Set CMAKE_MODULE_PATH to load custom CMake modules

find_path (USG_ROOT devel_common/cmake/CMakeSettings.cmake
  PATHS 
  ${NUMPY_SOURCE_DIR}
  ${NUMPY_SOURCE_DIR}/..
  ${NUMPY_SOURCE_DIR}/../..
  ${NUMPY_SOURCE_DIR}/../../..
  ENV LOFARSOFT
  )

if (USG_ROOT)
  ## load common CMake definitions
  include (${USG_ROOT}/devel_common/cmake/CMakeSettings.cmake)
  ## try locating required external packages
  set (PYTHON_FIND_QUIETLY  YES)
  include (FindPython)
else (USG_ROOT)
  message (STATUS "Unable to locate additional CMake scripts!")
endif (USG_ROOT)

## <-- begin build condition --------------------------------------------------->

if (NOT HAVE_NUMPY OR NUMPY_FORCE_BUILD)
  
  ## ----------------------------------------------------------------------------
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

  ## <-- method used to configure and build the package ------------------------>

  if (NUMPY_CMAKE_BUILD)

    add_subdirectory (numpy)

  else (NUMPY_CMAKE_BUILD)
    
    ## --------------------------------------------------------------------------
    ## Run the configure script provided along with the distribution
    ##
    ## python setup.py install --gencode
    ##                         --install-lib=<dir>
    ##                         --install-headers=<dir>/numpy
    ##
    
    message (STATUS "Running the Python setup script for this module ...")
    
    if(UNIX)
      
      execute_process (
	WORKING_DIRECTORY ${NUMPY_SOURCE_DIR}
	COMMAND python setup.py install --prefix=${NUMPY_prefix}
	TIMEOUT 1800
	ERROR_VARIABLE error_configure
	ERROR_FILE error.log
	)
      
      if (error_configure)
	message (STATUS "There was an error running the Numpy configure script!")
	message (SEND_ERROR "${error_configure}")
      endif (error_configure)
      
    endif(UNIX)
    
  endif (NUMPY_CMAKE_BUILD)
  
  ## <-- end build condition --------------------------------------------------->
  
else (NOT HAVE_NUMPY OR NUMPY_FORCE_BUILD)
  message (STATUS "Found system-wide installation of NUMPY; skipping rebuild!")
endif (NOT HAVE_NUMPY OR NUMPY_FORCE_BUILD)
