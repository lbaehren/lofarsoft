
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

find_path (USG_ROOT devel_common/cmake/CMakeSettings.cmake
  PATHS 
  ${PLPLOT_SOURCE_DIR}
  ${PLPLOT_SOURCE_DIR}/..
  ${PLPLOT_SOURCE_DIR}/../..
  ${PLPLOT_SOURCE_DIR}/../../..
  $ENV{LOFARSOFT}
  )

if (USG_ROOT)
  include (${USG_ROOT}/devel_common/cmake/CMakeSettings.cmake)
else (USG_ROOT)
  message (FATAL_ERROR "[Plplot] Unable to locate additional CMake scripts!")
endif (USG_ROOT)

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

  find_file (have_plplot_tgz plplot-5.9.2.tar.gz ${PLPLOT_SOURCE_DIR})
  
  configure_file (
    ${PLPLOT_SOURCE_DIR}/plplot.cmake.in
    ${PLPLOT_BINARY_DIR}/plplot.cmake
    )

  if (have_plplot_tgz)
    ExternalProject_Add (plplot
      PREFIX ${PLPLOT_BINARY_DIR}
      DOWNLOAD_DIR ${PLPLOT_BINARY_DIR}/plplot
      SOURCE_DIR ${PLPLOT_BINARY_DIR}/plplot
      URL ${have_plplot_tgz}
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
  message (STATUS "[Plplot] Found system-wide installation; skipping rebuild!")
endif (NOT HAVE_PLPLOT OR PLPLOT_FORCE_BUILD)
