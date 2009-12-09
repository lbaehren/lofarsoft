
## Name of the project
project (VALGRIND)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.8)

## -----------------------------------------------------------------------------
## Only force rebuild of the library from the provided source code, if no system
## installation can be found. In order to handle this properly, we use the CMake
## find script

option (VALGRIND_FORCE_BUILD       "Force rebuild and local installation?"  NO  )
option (VALGRIND_VERBOSE_CONFIGURE "Be verbose during configuration?"       YES )
option (VALGRIND_VERBOSE_BUILD     "Be verbose during build process?"       NO  )

## ------------------------------------------------------------------------------
## Set CMAKE_MODULE_PATH to load custom CMake modules

find_path (USG_ROOT devel_common/cmake/CMakeSettings.cmake
  PATHS 
  ${VALGRIND_SOURCE_DIR}
  ${VALGRIND_SOURCE_DIR}/..
  ${VALGRIND_SOURCE_DIR}/../..
  ${VALGRIND_SOURCE_DIR}/../../..
  $ENV{LOFARSOFT}
  )

if (USG_ROOT)
  include (${USG_ROOT}/devel_common/cmake/CMakeSettings.cmake)
else (USG_ROOT)
  message (FATAL_ERROR "[Valgrind] Unable to locate additional CMake scripts!")
endif (USG_ROOT)

## ------------------------------------------------------------------------------
## Check if the program/package/library is already available

set (VALGRIND_FIND_QUIETLY YES )

#include (FindValgrind)

## <-- begin build condition --------------------------------------------------->

if (NOT HAVE_VALGRIND OR VALGRIND_FORCE_BUILD)
  
  ##__________________________________________________________________
  ## Additional CMake modules

  include (ExternalProject)

  ##__________________________________________________________________
  ## Configure and build the project
    
  ExternalProject_Add (valgrind
    PREFIX ${VALGRIND_BINARY_DIR}
    DOWNLOAD_DIR ${VALGRIND_BINARY_DIR}
    SOURCE_DIR ${VALGRIND_BINARY_DIR}
    SVN_REPOSITORY svn://svn.valgrind.org/valgrind/trunk
    PATCH_COMMAND ${VALGRIND_BINARY_DIR}/autogen.sh
    BUILD_IN_SOURCE 1
    CONFIGURE_COMMAND ${VALGRIND_BINARY_DIR}/configure --prefix=${USG_INSTALL_PREFIX}
    INSTALL_COMMAND sudo make install
    )
  
  ## <-- end build condition ----------------------------------------------------->
  
else (NOT HAVE_VALGRIND OR VALGRIND_FORCE_BUILD)
  add_custom_target (valgrind
    WORKING_DIRECTORY ${VALGRIND_BINARY_DIR}
    COMMENT "[Valgrind] Found system-wide installation; skipping rebuild!"
    )
  message (STATUS "[Valgrind] Found system-wide installation; skipping rebuild!")
endif (NOT HAVE_VALGRIND OR VALGRIND_FORCE_BUILD)
