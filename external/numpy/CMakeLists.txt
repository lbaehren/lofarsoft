
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
## Check for system-wide NUMPY library installation

find_program (HAVE_NUMPY numpy PATHS ${bin_locations})

## <-- begin build condition --------------------------------------------------->

if (NOT HAVE_NUMPY OR NUMPY_FORCE_BUILD)
  
  include (ExternalProject)

  set (NUMPY_VERSION     "1.4.0")
  set (NUMPY_DIRECTORY   "numpy-${NUMPY_VERSION}")
  set (NUMPY_TAR_ARCHIVE "${NUMPY_DIRECTORY}.tar.gz")
  
  ## create "site.cfg" used later on
  
  file (WRITE ${NUMPY_BINARY_DIR}/site.cfg "[DEFAULT] \n")
  file (APPEND ${NUMPY_BINARY_DIR}/site.cfg "library_dirs = /usr/lib, /usr/local/lib, /usr/lib64, /usr/local/lib64, /sw/lib \n")

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
      CONFIGURE_COMMAND ${PYTHON_EXECUTABLE} setup.py --help
      BUILD_COMMAND ${PYTHON_EXECUTABLE} setup.py build
      INSTALL_COMMAND ${PYTHON_EXECUTABLE} setup.py install --prefix= --home=${USG_INSTALL_PREFIX}
      )
  else (have_numpy_tgz)
    message (WARNING "[Numpy] Unable to build project - missing source!")
  endif (have_numpy_tgz)
  
  ## <-- end build condition ----------------------------------------------------->
  
else (NOT HAVE_NUMPY OR NUMPY_FORCE_BUILD)
  add_custom_target (numpy
    WORKING_DIRECTORY ${BOOST_BINARY_DIR}
    COMMENT "[Numpy] Found system-wide installation; skipping rebuild!"
    )
  message (STATUS "[Numpy] Found system-wide installation; skipping rebuild!")
endif (NOT HAVE_NUMPY OR NUMPY_FORCE_BUILD)
