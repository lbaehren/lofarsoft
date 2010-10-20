
## Name of the project
project (ARMADILLO)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.8)

## -----------------------------------------------------------------------------
## Only force rebuild of the library from the provided source code, if no system
## installation can be found. In order to handle this properly, we use the CMake
## find script

option (ARMADILLO_FORCE_BUILD       "Force rebuild and local installation?"  NO  )
option (ARMADILLO_VERBOSE_CONFIGURE "Be verbose during configuration?"       NO  )
option (ARMADILLO_VERBOSE_BUILD     "Be verbose during build process?"       NO  )

## ------------------------------------------------------------------------------
## Set CMAKE_MODULE_PATH to load custom CMake modules

find_path (LUS_ROOT devel_common/cmake/CMakeSettings.cmake
  PATHS 
  ${ARMADILLO_SOURCE_DIR}
  ${ARMADILLO_SOURCE_DIR}/..
  ${ARMADILLO_SOURCE_DIR}/../..
  ${ARMADILLO_SOURCE_DIR}/../../..
  $ENV{LOFARSOFT}
  )

if (LUS_ROOT)
  include (${LUS_ROOT}/devel_common/cmake/CMakeSettings.cmake)
else (LUS_ROOT)
  message (FATAL_ERROR "Unable to locate additional CMake scripts!")
endif (LUS_ROOT)

## ------------------------------------------------------------------------------
## Check if the program/package/library is already available

if (NOT ARMADILLO_VERBOSE_CONFIGURE)
  set (ARMADILLO_FIND_QUIETLY YES )
  set (BOOST_FIND_QUIETLY YES )
endif (NOT ARMADILLO_VERBOSE_CONFIGURE)

include (FindArmadillo)
include (FindBoost)

## <-- begin build condition --------------------------------------------------->

if (NOT HAVE_ARMADILLO OR ARMADILLO_FORCE_BUILD)
  
  set (ARMADILLO_VERSION "0.9.90")
  set (ARMADILLO_TAR_ARCHIVE "armadillo-${ARMADILLO_VERSION}.tar.gz")
  set (ARMADILLO_URL "http://downloads.sourceforge.net/project/arma/${ARMADILLO_TAR_ARCHIVE}")

  ##__________________________________________________________________
  ## Build the project
  
  include (ExternalProject)
  
  find_file (have_armadillo_tgz ${ARMADILLO_TAR_ARCHIVE} ${ARMADILLO_SOURCE_DIR})
  
  if (NOT have_armadillo_tgz)
    set (have_armadillo_tgz ${ARMADILLO_URL})
  endif (NOT have_armadillo_tgz)
  
  ExternalProject_Add (armadillo
    PREFIX ${ARMADILLO_BINARY_DIR}
    DOWNLOAD_DIR ${ARMADILLO_BINARY_DIR}/download
    SOURCE_DIR ${ARMADILLO_BINARY_DIR}/source
    URL ${have_armadillo_tgz}
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${USG_INSTALL_PREFIX}
    )
  
  ## <-- end build condition ----------------------------------------------------->
  
else (NOT HAVE_ARMADILLO OR ARMADILLO_FORCE_BUILD)
  add_custom_target (armadillo
    WORKING_DIRECTORY ${ARMADILLO_BINARY_DIR}
    COMMENT "[Armadillo] Found system-wide installation; skipping rebuild!"
    )
endif (NOT HAVE_ARMADILLO OR ARMADILLO_FORCE_BUILD)
