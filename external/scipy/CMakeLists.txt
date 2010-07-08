
## ------------------------------------------------------------------------------
#
#  CMake wrapper around the GNU Autotools-based configure/build of the SCIPY
#  library.
#
## ------------------------------------------------------------------------------

## Name of the project
project (SCIPY)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.8)

## -----------------------------------------------------------------------------
## Only force rebuild of the library from the provided source code, if no system
## installation can be found. In order to handle this properly, we use the CMake
## find script

option (SCIPY_FORCE_BUILD       "Force rebuild and local installation"      NO  )
option (SCIPY_VERBOSE_CONFIGURE "Be verbose during configuration?"          YES )
option (SCIPY_VERBOSE_BUILD     "Be verbose during build process?"          NO  )

## ------------------------------------------------------------------------------
## Set CMAKE_MODULE_PATH to load custom CMake modules

find_path (USG_ROOT devel_common/cmake/CMakeSettings.cmake
  PATHS 
  ${SCIPY_SOURCE_DIR}
  ${SCIPY_SOURCE_DIR}/..
  ${SCIPY_SOURCE_DIR}/../..
  ${SCIPY_SOURCE_DIR}/../../..
  $ENV{LOFARSOFT}
  )

if (USG_ROOT)
  include (${USG_ROOT}/devel_common/cmake/CMakeSettings.cmake)
else (USG_ROOT)
  message (FATAL_ERROR "[SciPy] Unable to locate additional CMake scripts!")
endif (USG_ROOT)

## ------------------------------------------------------------------------------
## System inspection

if (NOT SCIPY_VERBOSE_CONFIGURE)
  set (LAPACK_FIND_QUIETLY   YES )
  set (SCIPY_FIND_QUIETLY  YES )
  set (PYTHON_FIND_QUIETLY YES )
  set (NUMPY_FIND_QUIETLY YES )
endif (NOT SCIPY_VERBOSE_CONFIGURE)

include (FindNumPy)
include (FindSciPy)
include (FindLAPACK)

if (NOT HAVE_PYTHON)
  message (WARNING "[SciPy] Unable to build project - missing Python!")
endif (NOT HAVE_PYTHON)

if (NOT LAPACK_BLAS_LIBRARY)
  message (WARNING "[SciPy] Unable to build project - missing BLAS library!")
endif (NOT LAPACK_BLAS_LIBRARY)

if (NOT LAPACK_LAPACK_LIBRARY)
  message (WARNING "[SciPy] Unable to build project - missing LAPACK library!")
endif (NOT LAPACK_LAPACK_LIBRARY)

if (NOT HAVE_NUMPY)
  message (WANRING "[SciPy] Unable to build project - missing NumPy!")
endif (NOT HAVE_NUMPY)

## Check version number of SciPy installation

set (SCIPY_REQUIRED_MAJOR_VERSION   0)
set (SCIPY_REQUIRED_MINOR_VERSION   7)

if (SCIPY_API_VERSION)
  if (SCIPY_MAJOR_VERSION LESS ${SCIPY_REQUIRED_MAJOR_VERSION})
    message (STATUS "[SciPy] Installation found - Major version too small!")
    set (SCIPY_FORCE_BUILD TRUE)
  else (SCIPY_MAJOR_VERSION LESS ${SCIPY_REQUIRED_MAJOR_VERSION})
    ## Check for major version
    if (SCIPY_MINOR_VERSION LESS ${SCIPY_REQUIRED_MINOR_VERSION})
      message (STATUS "[SciPy] Installation found - minor version too small!")
      set (SCIPY_FORCE_BUILD TRUE)
    endif (SCIPY_MINOR_VERSION LESS ${SCIPY_REQUIRED_MINOR_VERSION})
  endif (SCIPY_MAJOR_VERSION LESS ${SCIPY_REQUIRED_MAJOR_VERSION})
endif (SCIPY_API_VERSION)

## <-- begin build condition --------------------------------------------------->

if (NOT HAVE_SCIPY OR SCIPY_FORCE_BUILD)
  
  include (ExternalProject)

  set (SCIPY_VERSION     "0.7.2")
  set (SCIPY_DIRECTORY   "scipy-${SCIPY_VERSION}")
  set (SCIPY_TAR_ARCHIVE "${SCIPY_DIRECTORY}.tar.gz")
  
  ## create "site.cfg" used later on

  if (PYTHON_LIBRARIES)
    get_filename_component (python_libdir ${PYTHON_LIBRARIES} PATH)
  endif (PYTHON_LIBRARIES)
  
  ##__________________________________________________________________
  ## Build the project

  find_file (have_scipy_tgz ${SCIPY_TAR_ARCHIVE} ${SCIPY_SOURCE_DIR})

  if (have_scipy_tgz)
    ExternalProject_Add (scipy
      PREFIX ${SCIPY_BINARY_DIR}
      DOWNLOAD_DIR ${SCIPY_BINARY_DIR}
      SOURCE_DIR ${SCIPY_BINARY_DIR}
      URL ${have_scipy_tgz}
      BUILD_IN_SOURCE 1
      CONFIGURE_COMMAND ${PYTHON_EXECUTABLE} ${SCIPY_BINARY_DIR}/setup.py --help
      BUILD_COMMAND ${PYTHON_EXECUTABLE} ${SCIPY_BINARY_DIR}/setup.py build
      INSTALL_COMMAND ${PYTHON_EXECUTABLE} ${SCIPY_BINARY_DIR}/setup.py install --prefix= --home=${USG_INSTALL_PREFIX}
      )
  else (have_scipy_tgz)
    message (WARNING "[SciPy] Unable to build project - missing source!")
  endif (have_scipy_tgz)

  ##__________________________________________________________________
  ## Configuration summary
  
  if (SCIPY_VERBOSE_CONFIGURE)
    message ("-------------------------------------------------------------")
    message (STATUS "[SciPy] Summary of configuration status.")
    message (STATUS "BLAS_LIBRARIES   = ${LAPACK_BLAS_LIBRARY}")
    message (STATUS "PYTHON_LIBRARIES = ${PYTHON_LIBRARIES}")
    message (STATUS "PYTHON_VERSION   = ${PYTHON_VERSION}")
    message (STATUS "NUMPY_VERSION    = ${NUMPY_VERSION}")
    message ("-------------------------------------------------------------")
  endif (SCIPY_VERBOSE_CONFIGURE)
  
  ## <-- end build condition ----------------------------------------------------->
  
else (NOT HAVE_SCIPY OR SCIPY_FORCE_BUILD)
  add_custom_target (scipy
    WORKING_DIRECTORY ${BOOST_BINARY_DIR}
    COMMENT "[SciPy] Found system-wide installation; skipping rebuild!"
    )
endif (NOT HAVE_SCIPY OR SCIPY_FORCE_BUILD)
