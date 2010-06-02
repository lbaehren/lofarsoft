
## ------------------------------------------------------------------------------
#
#  CMake wrapper around the GNU Autotools-based configure/build of the NUMPY
#  library.
#
## ------------------------------------------------------------------------------

## Name of the project
project (NUMPY)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.8)

## -----------------------------------------------------------------------------
## Only force rebuild of the library from the provided source code, if no system
## installation can be found. In order to handle this properly, we use the CMake
## find script

option (NUMPY_FORCE_BUILD       "Force rebuild and local installation"      NO  )
option (NUMPY_VERBOSE_CONFIGURE "Be verbose during configuration?"          NO  )
option (NUMPY_VERBOSE_BUILD     "Be verbose during build process?"          NO  )

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
  message (FATAL_ERROR "[Numpy] Unable to locate additional CMake scripts!")
endif (USG_ROOT)

## ------------------------------------------------------------------------------
## System inspection

if (NOT NUMPY_VERBOSE_CONFIGURE)
  set (LAPACK_FIND_QUIETLY   YES )
  set (NUMPY_FIND_QUIETLY  YES )
  set (PYTHON_FIND_QUIETLY YES )
endif (NOT NUMPY_VERBOSE_CONFIGURE)

include (FindNumPy)
include (FindLAPACK)

if (NOT HAVE_PYTHON)
  message (SEND_ERROR "[NumPy] Unable to build project - missing Python!")
endif (NOT HAVE_PYTHON)

if (NOT LAPACK_BLAS_LIBRARY)
  message (SEND_ERROR "[NumPy] Unable to build project - missing BLAS library!")
endif (NOT LAPACK_BLAS_LIBRARY)

if (NOT LAPACK_LAPACK_LIBRARY)
  message (SEND_ERROR "[NumPy] Unable to build project - missing LAPACK library!")
endif (NOT LAPACK_LAPACK_LIBRARY)

## Check version number of NumPy installation

set (NUMPY_REQUIRED_MAJOR_VERSION   1)
set (NUMPY_REQUIRED_MINOR_VERSION   1)

if (NUMPY_API_VERSION)
  if (NUMPY_MAJOR_VERSION LESS ${NUMPY_REQUIRED_MAJOR_VERSION})
    message (STATUS "[NumPy] Installation found - Major version too small!")
    set (NUMPY_FORCE_BUILD TRUE)
  else (NUMPY_MAJOR_VERSION LESS ${NUMPY_REQUIRED_MAJOR_VERSION})
    ## Check for major version
    if (NUMPY_MINOR_VERSION LESS ${NUMPY_REQUIRED_MINOR_VERSION})
      message (STATUS "[NumPy] Installation found - minor version too small!")
      set (NUMPY_FORCE_BUILD TRUE)
    endif (NUMPY_MINOR_VERSION LESS ${NUMPY_REQUIRED_MINOR_VERSION})
  endif (NUMPY_MAJOR_VERSION LESS ${NUMPY_REQUIRED_MAJOR_VERSION})
endif (NUMPY_API_VERSION)

## <-- begin build condition --------------------------------------------------->

if (NOT HAVE_NUMPY OR NUMPY_FORCE_BUILD)
  
  include (ExternalProject)

  set (NUMPY_VERSION     "1.3.0")
  set (NUMPY_DIRECTORY   "numpy-${NUMPY_VERSION}")
  set (NUMPY_TAR_ARCHIVE "${NUMPY_DIRECTORY}.tar.gz")
  
  ## create "site.cfg" used later on

  if (PYTHON_LIBRARIES)
    get_filename_component (python_libdir ${PYTHON_LIBRARIES} PATH)
  endif (PYTHON_LIBRARIES)
  
  file (WRITE ${NUMPY_BINARY_DIR}/site.cfg "[DEFAULT] \n")
  file (APPEND ${NUMPY_BINARY_DIR}/site.cfg "library_dirs = /usr/lib:/usr/local/lib:/usr/lib64:/usr/local/lib64:/sw/lib:${python_libdir} \n")

  ##__________________________________________________________________
  ## Build the project

  find_file (have_numpy_tgz ${NUMPY_TAR_ARCHIVE} ${NUMPY_SOURCE_DIR})

  if (have_numpy_tgz)
    ExternalProject_Add (numpy
      PREFIX ${NUMPY_BINARY_DIR}
      DOWNLOAD_DIR ${NUMPY_BINARY_DIR}
      SOURCE_DIR ${NUMPY_BINARY_DIR}
      URL ${have_numpy_tgz}
      BUILD_IN_SOURCE 1
      CONFIGURE_COMMAND ${PYTHON_EXECUTABLE} ${NUMPY_BINARY_DIR}/setup.py --help
      BUILD_COMMAND ${PYTHON_EXECUTABLE} ${NUMPY_BINARY_DIR}/setup.py build
      INSTALL_COMMAND ${PYTHON_EXECUTABLE} ${NUMPY_BINARY_DIR}/setup.py install --prefix= --home=${USG_INSTALL_PREFIX}
      )
  else (have_numpy_tgz)
    message (WARNING "[Numpy] Unable to build project - missing source!")
  endif (have_numpy_tgz)

  ##__________________________________________________________________
  ## Configuration summary
  
  if (NUMPY_VERBOSE_CONFIGURE)
    message ("-------------------------------------------------------------")
    message (STATUS "[NumPy] Summary of configuration status.")
    message (STATUS "BLAS_LIBRARIES   = ${BLAS_LIBRARIES}")
    message (STATUS "PYTHON_LIBRARIES = ${PYTHON_LIBRARIES}")
    message (STATUS "PYTHON_VERSION   = ${PYTHON_VERSION}")
    message ("-------------------------------------------------------------")
  endif (NUMPY_VERBOSE_CONFIGURE)
  
  ## <-- end build condition ----------------------------------------------------->
  
else (NOT HAVE_NUMPY OR NUMPY_FORCE_BUILD)
  add_custom_target (numpy
    WORKING_DIRECTORY ${BOOST_BINARY_DIR}
    COMMENT "[Numpy] Found system-wide installation; skipping rebuild!"
    )
endif (NOT HAVE_NUMPY OR NUMPY_FORCE_BUILD)
