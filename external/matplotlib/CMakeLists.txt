
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
  set (NUMPY_FIND_QUIETLY  YES )
  set (PYTHON_FIND_QUIETLY YES )
endif (NOT MATPLOTLIB_VERBOSE_CONFIGURE)

include (FindNumPy)
include (FindMatplotlib)

## <-- begin build condition --------------------------------------------------->

if (NOT HAVE_MATPLOTLIB OR MATPLOTLIB_FORCE_BUILD)

  include (ExternalProject)
  
  find_file (have_matplotlib_tgz matplotlib-0.98.5.3.tar.gz ${MATPLOTLIB_SOURCE_DIR})
  
  if (have_matplotlib_tgz)
    ExternalProject_Add (matplotlib
      PREFIX ${MATPLOTLIB_BINARY_DIR}
      DOWNLOAD_DIR ${MATPLOTLIB_BINARY_DIR}
      SOURCE_DIR ${MATPLOTLIB_BINARY_DIR}
      URL ${have_matplotlib_tgz}
      BUILD_IN_SOURCE 1
      CONFIGURE_COMMAND ${PYTHON_EXECUTABLE} ${MATPLOTLIB_BINARY_DIR}/setup.py build
      BUILD_COMMAND ${PYTHON_EXECUTABLE} ${MATPLOTLIB_BINARY_DIR}/setup.py install --prefix=  --home=${USG_INSTALL_PREFIX}
      )
  else (have_matplotlib_tgz)
    status (WARNING "[Matplotlib] Unable to build project - no sources found!")
  endif (have_matplotlib_tgz)
  
  ## <-- end build condition ----------------------------------------------------->
  
else (NOT HAVE_MATPLOTLIB OR MATPLOTLIB_FORCE_BUILD)
  add_custom_target (matplotlib
    WORKING_DIRECTORY ${MATPLOTLIB_BINARY_DIR}
    COMMENT "[Matplotlib] Found system-wide installation; skipping rebuild!"
    )
  message (STATUS "[Matplotlib] Found system-wide installation; skipping rebuild!")
endif (NOT HAVE_MATPLOTLIB OR MATPLOTLIB_FORCE_BUILD)
