
## ------------------------------------------------------------------------------
#
#  CMake wrapper around the GNU Autotools-based configure/build of the SPHINX
#  library.
#
## ------------------------------------------------------------------------------

## Name of the project
project (SPHINX)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.8)

## -----------------------------------------------------------------------------
## Only force rebuild of the binary from the provided source code, if no system
## installation can be found. In order to handle this properly, we use the CMake
## find script

option (SPHINX_FORCE_BUILD       "Force rebuild and local installation"      NO  )
option (SPHINX_VERBOSE_CONFIGURE "Be verbose during configuration?"          NO  )
option (SPHINX_VERBOSE_BUILD     "Be verbose during build process?"          NO  )

## ------------------------------------------------------------------------------
## Set CMAKE_MODULE_PATH to load custom CMake modules

find_path (LUS_ROOT devel_common/cmake/CMakeSettings.cmake
  PATHS
  ${SPHINX_SOURCE_DIR}
  ${SPHINX_SOURCE_DIR}/..
  ${SPHINX_SOURCE_DIR}/../..
  ${SPHINX_SOURCE_DIR}/../../..
  $ENV{LOFARSOFT}
  )

if (LUS_ROOT)
  include (${LUS_ROOT}/devel_common/cmake/CMakeSettings.cmake)
else (LUS_ROOT)
  message (FATAL_ERROR "[Sphinx] Unable to locate additional CMake scripts!")
endif (LUS_ROOT)

## ------------------------------------------------------------------------------
## System inspection

if (NOT SPHINX_VERBOSE_CONFIGURE)
  set (SPHINX_FIND_QUIETLY  YES )
  set (PYTHON_FIND_QUIETLY YES )
endif (NOT SPHINX_VERBOSE_CONFIGURE)

include (FindPython)
include (FindSphinx)

## Check version number of Sphinx installation
set (SPHINX_REQUIRED_MAJOR_VERSION   1)
set (SPHINX_REQUIRED_MINOR_VERSION   0)

if (SPHINX_API_VERSION)
  if (SPHINX_MAJOR_VERSION LESS ${SPHINX_REQUIRED_MAJOR_VERSION})
    message (STATUS "[Sphinx] Installation found - Major version too small!")
    set (SPHINX_FORCE_BUILD TRUE)
  else (SPHINX_MAJOR_VERSION LESS ${SPHINX_REQUIRED_MAJOR_VERSION})
    ## Check for major version
    if (SPHINX_MINOR_VERSION LESS ${SPHINX_REQUIRED_MINOR_VERSION})
      message (STATUS "[Sphinx] Installation found - minor version too small!")
      set (SPHINX_FORCE_BUILD TRUE)
    endif (SPHINX_MINOR_VERSION LESS ${SPHINX_REQUIRED_MINOR_VERSION})
  endif (SPHINX_MAJOR_VERSION LESS ${SPHINX_REQUIRED_MAJOR_VERSION})
endif (SPHINX_API_VERSION)

## <-- begin build condition --------------------------------------------------->

if (NOT HAVE_SPHINX OR SPHINX_FORCE_BUILD)

  include (ExternalProject)

  set (SPHINX_VERSION     "1.0.7")
  set (SPHINX_DIRECTORY   "Sphinx-${SPHINX_VERSION}")
  set (SPHINX_TAR_ARCHIVE "${SPHINX_DIRECTORY}.tar.gz")

  ##__________________________________________________________________
  ## Build the project

  find_file (have_sphinx_tgz ${SPHINX_TAR_ARCHIVE} ${SPHINX_SOURCE_DIR})

  if (have_sphinx_tgz)
    ExternalProject_Add (sphinx
      PREFIX ${SPHINX_BINARY_DIR}
      DOWNLOAD_DIR ${SPHINX_BINARY_DIR}/download
      SOURCE_DIR ${SPHINX_BINARY_DIR}/source
      BUILD_IN_SOURCE 1
      URL ${have_sphinx_tgz}
#      URL_MD5 d4067e42e75525ecf108ece471ce97b
      URL_MD5 42c722d48e52d4888193965dd473adb5
      CONFIGURE_COMMAND echo "No configuration needed"
      BUILD_COMMAND echo "Not building anything"
      INSTALL_COMMAND ${PYTHON_EXECUTABLE} ${SPHINX_BINARY_DIR}/source/setup.py install --home=${LUS_INSTALL_PREFIX}
      )
  else (have_sphinx_tgz)
    message (WARNING "[Sphinx] Unable to build project - missing source!")
  endif (have_sphinx_tgz)

  ##__________________________________________________________________
  ## Configuration summary

  if (SPHINX_VERBOSE_CONFIGURE)
    message ("-------------------------------------------------------------")
    message (STATUS "[Sphinx] Summary of configuration status.")
    message (STATUS "PYTHON_VERSION   = ${PYTHON_VERSION}")
    message ("-------------------------------------------------------------")
  endif (SPHINX_VERBOSE_CONFIGURE)

  ## <-- end build condition ----------------------------------------------------->

else (NOT HAVE_SPHINX OR SPHINX_FORCE_BUILD)
  add_custom_target (sphinx
    WORKING_DIRECTORY ${BOOST_BINARY_DIR}
    COMMENT "[Sphinx] Found system-wide installation; skipping rebuild!"
    )
endif (NOT HAVE_SPHINX OR SPHINX_FORCE_BUILD)

