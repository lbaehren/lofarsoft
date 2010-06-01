
## ------------------------------------------------------------------------------
#
#  CMake wrapper around the GNU Autotools-based configure/build of the IPYTHON
#  library.
#
## ------------------------------------------------------------------------------

## Name of the project
project (IPYTHON)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.8)

## -----------------------------------------------------------------------------
## Only force rebuild of the binary from the provided source code, if no system
## installation can be found. In order to handle this properly, we use the CMake
## find script

option (IPYTHON_FORCE_BUILD       "Force rebuild and local installation"      NO  )
option (IPYTHON_VERBOSE_CONFIGURE "Be verbose during configuration?"          NO  )
option (IPYTHON_VERBOSE_BUILD     "Be verbose during build process?"          NO  )

## ------------------------------------------------------------------------------
## Set CMAKE_MODULE_PATH to load custom CMake modules

find_path (USG_ROOT devel_common/cmake/CMakeSettings.cmake
  PATHS 
  ${IPYTHON_SOURCE_DIR}
  ${IPYTHON_SOURCE_DIR}/..
  ${IPYTHON_SOURCE_DIR}/../..
  ${IPYTHON_SOURCE_DIR}/../../..
  $ENV{LOFARSOFT}
  )

if (USG_ROOT)
  include (${USG_ROOT}/devel_common/cmake/CMakeSettings.cmake)
else (USG_ROOT)
  message (FATAL_ERROR "[IPython] Unable to locate additional CMake scripts!")
endif (USG_ROOT)

## ------------------------------------------------------------------------------
## System inspection

if (NOT IPYTHON_VERBOSE_CONFIGURE)
  set (IPYTHON_FIND_QUIETLY  YES )
  set (PYTHON_FIND_QUIETLY YES )
endif (NOT IPYTHON_VERBOSE_CONFIGURE)

include (FindPython)
include (FindIPython)

## Check version number of IPython installation
set (IPYTHON_REQUIRED_MAJOR_VERSION   0)
set (IPYTHON_REQUIRED_MINOR_VERSION   11)

if (IPYTHON_API_VERSION)
  if (IPYTHON_MAJOR_VERSION LESS ${IPYTHON_REQUIRED_MAJOR_VERSION})
    message (STATUS "[IPython] Installation found - Major version too small!")
    set (IPYTHON_FORCE_BUILD TRUE)
  else (IPYTHON_MAJOR_VERSION LESS ${IPYTHON_REQUIRED_MAJOR_VERSION})
    ## Check for major version
    if (IPYTHON_MINOR_VERSION LESS ${IPYTHON_REQUIRED_MINOR_VERSION})
      message (STATUS "[IPython] Installation found - minor version too small!")
      set (IPYTHON_FORCE_BUILD TRUE)
    endif (IPYTHON_MINOR_VERSION LESS ${IPYTHON_REQUIRED_MINOR_VERSION})
  endif (IPYTHON_MAJOR_VERSION LESS ${IPYTHON_REQUIRED_MAJOR_VERSION})
endif (IPYTHON_API_VERSION)

## <-- begin build condition --------------------------------------------------->

if (NOT HAVE_IPYTHON OR IPYTHON_FORCE_BUILD)
  
  include (ExternalProject)

  set (IPYTHON_VERSION     "0.11")
  set (IPYTHON_DIRECTORY   "ipython-${IPYTHON_VERSION}")
  set (IPYTHON_TAR_ARCHIVE "${IPYTHON_DIRECTORY}.tar.gz")
  
  ## create "site.cfg" used later on

  if (PYTHON_LIBRARIES)
    get_filename_component (python_libdir ${PYTHON_LIBRARIES} PATH)
  endif (PYTHON_LIBRARIES)
  
  file (WRITE ${IPYTHON_BINARY_DIR}/site.cfg "[DEFAULT] \n")
  file (APPEND ${IPYTHON_BINARY_DIR}/site.cfg "library_dirs = /usr/lib, /usr/local/lib, /usr/lib64, /usr/local/lib64, /sw/lib, ${python_libdir} \n")

  ##__________________________________________________________________
  ## Build the project

  find_file (have_ipython_tgz ${IPYTHON_TAR_ARCHIVE} ${IPYTHON_SOURCE_DIR})

  if (have_ipython_tgz)
    ExternalProject_Add (ipython
      PREFIX ${IPYTHON_BINARY_DIR}
      DOWNLOAD_DIR ${IPYTHON_BINARY_DIR}
      SOURCE_DIR ${IPYTHON_BINARY_DIR}
      URL ${have_ipython_tgz}
      BUILD_IN_SOURCE 1
      CONFIGURE_COMMAND ${PYTHON_EXECUTABLE} ${IPYTHON_BINARY_DIR}/setup.py --help
      BUILD_COMMAND ${PYTHON_EXECUTABLE} ${IPYTHON_BINARY_DIR}/setup.py build
      INSTALL_COMMAND ${PYTHON_EXECUTABLE} ${IPYTHON_BINARY_DIR}/setup.py install --prefix= --home=${USG_INSTALL_PREFIX}
      )
  else (have_ipython_tgz)
    message (WARNING "[IPython] Unable to build project - missing source!")
  endif (have_ipython_tgz)

  ##__________________________________________________________________
  ## Configuration summary
  
  if (IPYTHON_VERBOSE_CONFIGURE)
    message ("-------------------------------------------------------------")
    message (STATUS "[IPython] Summary of configuration status.")
    message (STATUS "PYTHON_VERSION   = ${PYTHON_VERSION}")
    message ("-------------------------------------------------------------")
  endif (IPYTHON_VERBOSE_CONFIGURE)
  
  ## <-- end build condition ----------------------------------------------------->
  
else (NOT HAVE_IPYTHON OR IPYTHON_FORCE_BUILD)
  add_custom_target (ipython
    WORKING_DIRECTORY ${BOOST_BINARY_DIR}
    COMMENT "[IPython] Found system-wide installation; skipping rebuild!"
    )
endif (NOT HAVE_IPYTHON OR IPYTHON_FORCE_BUILD)

