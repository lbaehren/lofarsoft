
## Name of the project
project (MATPLOTLIB)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.8)

## -----------------------------------------------------------------------------
## Only force rebuild of the library from the provided source code, if no system
## installation can be found. In order to handle this properly, we use the CMake
## find script

option (MATPLOTLIB_FORCE_BUILD       "Force rebuild and local installation?"    NO  )
option (MATPLOTLIB_CMAKE_BUILD       "Use CMake to configure and build MATPLOTLIB?"  YES )
option (MATPLOTLIB_VERBOSE_CONFIGURE "Be verbose during configuration?"         NO  )
option (MATPLOTLIB_VERBOSE_BUILD     "Be verbose during build process?"         NO  )

## ------------------------------------------------------------------------------
## Set CMAKE_MODULE_PATH to load custom CMake modules

find_path (USG_ROOT devel_common/cmake/CMakeSettings.cmake
  PATHS 
  ${MATPLOTLIB_SOURCE_DIR}
  ${MATPLOTLIB_SOURCE_DIR}/..
  ${MATPLOTLIB_SOURCE_DIR}/../..
  ${MATPLOTLIB_SOURCE_DIR}/../../..
  $ENV{LOFARSOFT}
  )

if (USG_ROOT)
  include (${USG_ROOT}/devel_common/cmake/CMakeSettings.cmake)
else (USG_ROOT)
  message (FATAL_ERROR "Unable to locate additional CMake scripts!")
endif (USG_ROOT)

## ------------------------------------------------------------------------------
## Check if the program/package/library is already available

if (NOT MATPLOTLIB_VERBOSE_CONFIGURE)
  set (MATPLOTLIB_FIND_QUIETLY  YES )
  set (NUMPY_FIND_QUIETLY       YES )
  set (PYTHON_FIND_QUIETLY      YES )
endif (NOT MATPLOTLIB_VERBOSE_CONFIGURE)

include (FindPython)
include (FindNumPy)
include (FindMatplotlib)

message (STATUS "Python = ${PYTHON_EXECUTABLE}")

## <-- begin build condition --------------------------------------------------->

if (NOT HAVE_MATPLOTLIB OR MATPLOTLIB_FORCE_BUILD)

  include (ExternalProject)
  
  set (matplotlib_version "0.99.1.2")
  set (matplotlib_tar_archive "matplotlib-${matplotlib_version}.tar.gz")

  find_file (have_matplotlib_tgz ${matplotlib_tar_archive} ${MATPLOTLIB_SOURCE_DIR})
  find_file (have_setupext_py setupext.py.in ${MATPLOTLIB_SOURCE_DIR})

  ## Build the project

  if (have_matplotlib_tgz)
    ExternalProject_Add (matplotlib
      PREFIX ${MATPLOTLIB_BINARY_DIR}
      DOWNLOAD_DIR ${MATPLOTLIB_BINARY_DIR}
      SOURCE_DIR ${MATPLOTLIB_BINARY_DIR}
      URL ${have_matplotlib_tgz}
      BUILD_IN_SOURCE 1
      CONFIGURE_COMMAND ${PYTHON_EXECUTABLE} setup.py --help
      BUILD_COMMAND ${PYTHON_EXECUTABLE} ${MATPLOTLIB_BINARY_DIR}/setup.py build
      INSTALL_COMMAND ${PYTHON_EXECUTABLE} ${MATPLOTLIB_BINARY_DIR}/setup.py install --prefix=  --home=${USG_INSTALL_PREFIX}
      )
  else (have_matplotlib_tgz)
    status (WARNING "[Matplotlib] Unable to build project - no sources found!")
  endif (have_matplotlib_tgz)

  ## Patch the setup script to check for external packages

  if (have_setupext_py)
    ## create patched version of the configuration script
    configure_file (${have_setupext_py} ${MATPLOTLIB_SOURCE_DIR}/setupext.py)
    ## patch the original script provided by Matplotlib
    ExternalProject_Add_Step (matplotlib matplotlib_setupext
      COMMAND mv ${MATPLOTLIB_SOURCE_DIR}/setupext.py setupext.py
      COMMENT "Replacing original setupext.py ..."
      DEPENDEES patch
      DEPENDERS configure
      WORKING_DIRECTORY ${MATPLOTLIB_BINARY_DIR}
      )
  endif (have_setupext_py)
  
  ## <-- end build condition ----------------------------------------------------->
  
else (NOT HAVE_MATPLOTLIB OR MATPLOTLIB_FORCE_BUILD)
  add_custom_target (matplotlib
    WORKING_DIRECTORY ${MATPLOTLIB_BINARY_DIR}
    COMMENT "[Matplotlib] Found system-wide installation; skipping rebuild!"
    )
  message (STATUS "[Matplotlib] Found system-wide installation; skipping rebuild!")
endif (NOT HAVE_MATPLOTLIB OR MATPLOTLIB_FORCE_BUILD)
