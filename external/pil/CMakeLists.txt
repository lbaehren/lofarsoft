
## Name of the project
project (PIL)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.8)

## -----------------------------------------------------------------------------
## Only force rebuild of the library from the provided source code, if no system
## installation can be found. In order to handle this properly, we use the CMake
## find script

option (PIL_FORCE_BUILD       "Force rebuild and local installation?"  NO )
option (PIL_VERBOSE_CONFIGURE "Be verbose during configuration?"       NO )
option (PIL_VERBOSE_BUILD     "Be verbose during build process?"       NO )

## ------------------------------------------------------------------------------
## Set CMAKE_MODULE_PATH to load custom CMake modules

find_path (USG_ROOT devel_common/cmake/CMakeSettings.cmake
  PATHS 
  ${PIL_SOURCE_DIR}
  ${PIL_SOURCE_DIR}/..
  ${PIL_SOURCE_DIR}/../..
  ${PIL_SOURCE_DIR}/../../..
  $ENV{LOFARSOFT}
  )

if (USG_ROOT)
  include (${USG_ROOT}/devel_common/cmake/CMakeSettings.cmake)
else (USG_ROOT)
  message (FATAL_ERROR "[pil] Unable to locate additional CMake scripts!")
endif (USG_ROOT)

## ------------------------------------------------------------------------------
## Check if the program/package/library is already available

## <-- begin build condition --------------------------------------------------->

if (NOT HAVE_PIL OR PIL_FORCE_BUILD)
  
  include (ExternalProject)

  find_file (have_pil_tgz pil.tgz ${PIL_SOURCE_DIR})
  
  if (have_pil_tgz)
    ExternalProject_Add (pil
      PREFIX ${PIL_BINARY_DIR}
      DOWNLOAD_DIR ${PIL_BINARY_DIR}
      SOURCE_DIR ${PIL_BINARY_DIR}
      URL ${have_pil_tgz}
      CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${USG_INSTALL_PREFIX}
      )
  else (have_pil_tgz)
    message (WARNING "[pil] Unable to build project - no sources found!")
  endif (have_pil_tgz)
  
  ## <-- end build condition ----------------------------------------------------->
  
else (NOT HAVE_PIL OR PIL_FORCE_BUILD)
  add_custom_target (pil
    WORKING_DIRECTORY ${PIL_BINARY_DIR}
    COMMENT "[pil] Found system-wide installation; skipping rebuild!"
    )
endif (NOT HAVE_PIL OR PIL_FORCE_BUILD)
