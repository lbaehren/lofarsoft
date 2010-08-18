
## Name of the project
project (PLPLOT)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.8)

## -----------------------------------------------------------------------------
## Only force rebuild of the library from the provided source code, if no system
## installation can be found. In order to handle this properly, we use the CMake
## find script

option (PLPLOT_FORCE_BUILD       "Force rebuild and local installation?"  NO  )
option (PLPLOT_VERBOSE_CONFIGURE "Be verbose during configuration?"       YES )
option (PLPLOT_VERBOSE_BUILD     "Be verbose during build process?"       NO  )

## ------------------------------------------------------------------------------
## Set CMAKE_MODULE_PATH to load custom CMake modules

find_path (LUS_ROOT devel_common/cmake/CMakeSettings.cmake
  PATHS 
  ${PLPLOT_SOURCE_DIR}
  ${PLPLOT_SOURCE_DIR}/..
  ${PLPLOT_SOURCE_DIR}/../..
  ${PLPLOT_SOURCE_DIR}/../../..
  $ENV{LOFARSOFT}
  )

if (LUS_ROOT)
  include (${LUS_ROOT}/devel_common/cmake/CMakeSettings.cmake)
else (LUS_ROOT)
  message (FATAL_ERROR "[Plplot] Unable to locate additional CMake scripts!")
endif (LUS_ROOT)

## ------------------------------------------------------------------------------
## Check if the program/package/library is already available

set (PLPLOT_FIND_QUIETLY YES )

include (FindPLplot)

## <-- begin build condition --------------------------------------------------->

if (NOT HAVE_PLPLOT OR PLPLOT_FORCE_BUILD)
  
  ##__________________________________________________________________
  ## Additional CMake modules

  include (ExternalProject)
  include (FindNumPy)

  if (NOT HAVE_NUMPY)
    include (FindNumeric)
  endif (NOT HAVE_NUMPY)

  ##__________________________________________________________________
  ## Configure and build the project

  set (PLPLOT_VERSION "5.9.2" )
  set (PLPLOT_SOURCE  "plplot-${PLPLOT_VERSION}.tar.gz" )

  find_file (have_plplot_tgz ${PLPLOT_SOURCE} ${PLPLOT_SOURCE_DIR})
  
  configure_file (
    ${PLPLOT_SOURCE_DIR}/plplot.cmake.in
    ${PLPLOT_BINARY_DIR}/plplot.cmake
    )

  if (have_plplot_tgz)
    ExternalProject_Add (plplot
      PREFIX ${PLPLOT_BINARY_DIR}
      DOWNLOAD_DIR ${PLPLOT_BINARY_DIR}/download
      SOURCE_DIR ${PLPLOT_BINARY_DIR}/source
      URL ${have_plplot_tgz}
      URL_MD5 2a14753112083badab87f1806cc12a46
      CMAKE_ARGS -C ${PLPLOT_BINARY_DIR}/plplot.cmake
      )
  else (have_plplot_tgz)
    message (WARNING "[Plplot] Unable to build project - no sources found!")
  endif (have_plplot_tgz)
  
  ## <-- end build condition ----------------------------------------------------->
  
else (NOT HAVE_PLPLOT OR PLPLOT_FORCE_BUILD)
  add_custom_target (plplot
    WORKING_DIRECTORY ${PLPLOT_BINARY_DIR}
    COMMENT "[Plplot] Found system-wide installation; skipping rebuild!"
    )
endif (NOT HAVE_PLPLOT OR PLPLOT_FORCE_BUILD)
