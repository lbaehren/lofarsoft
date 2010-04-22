
## Name of the project
project (PFILE)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.8)

## -----------------------------------------------------------------------------
## Only force rebuild of the library from the provided source code, if no system
## installation can be found. In order to handle this properly, we use the CMake
## find script

option (PFILE_FORCE_BUILD       "Force rebuild and local installation?"  NO )
option (PFILE_VERBOSE_CONFIGURE "Be verbose during configuration?"       NO )
option (PFILE_VERBOSE_BUILD     "Be verbose during build process?"       NO )

## ------------------------------------------------------------------------------
## Set CMAKE_MODULE_PATH to load custom CMake modules

find_path (USG_ROOT devel_common/cmake/CMakeSettings.cmake
  PATHS 
  ${PFILE_SOURCE_DIR}
  ${PFILE_SOURCE_DIR}/..
  ${PFILE_SOURCE_DIR}/../..
  ${PFILE_SOURCE_DIR}/../../..
  $ENV{LOFARSOFT}
  )

if (USG_ROOT)
  include (${USG_ROOT}/devel_common/cmake/CMakeSettings.cmake)
else (USG_ROOT)
  message (FATAL_ERROR "[pfile] Unable to locate additional CMake scripts!")
endif (USG_ROOT)

## ------------------------------------------------------------------------------
## Check if the program/package/library is already available

## <-- begin build condition --------------------------------------------------->

if (NOT HAVE_PFILE OR PFILE_FORCE_BUILD)
  
  include (ExternalProject)

  find_file (have_pfile_tgz pfile.tar.gz ${PFILE_SOURCE_DIR})
  
  if (have_pfile_tgz)
    ExternalProject_Add (pfile
      PREFIX ${PFILE_BINARY_DIR}
      DOWNLOAD_DIR ${PFILE_BINARY_DIR}
      SOURCE_DIR ${PFILE_BINARY_DIR}
      URL ${have_pfile_tgz}
      BUILD_IN_SOURCE 1
      CONFIGURE_COMMAND ${PFILE_BINARY_DIR}/configure --prefix=${USG_INSTALL_PREFIX}
      )
  else (have_pfile_tgz)
    status (WARNING "[pfile] Unable to build project - no sources found!")
  endif (have_pfile_tgz)
  
## <-- end build condition ----------------------------------------------------->
  
else (NOT HAVE_PFILE OR PFILE_FORCE_BUILD)
  add_custom_target (pfile
    WORKING_DIRECTORY ${PFILE_BINARY_DIR}
    COMMENT "[pfile] Found system-wide installation; skipping rebuild!"
    )
endif (NOT HAVE_PFILE OR PFILE_FORCE_BUILD)
