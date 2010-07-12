
## ==============================================================================
##
##  CMake project settings
##
## ==============================================================================

## Name of the project handled by CMake
project (BLAS)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.8)

## Enforced CMake policy 
cmake_policy (VERSION 2.8)

## ------------------------------------------------------------------------------
## Set CMAKE_MODULE_PATH to load custom CMake modules

find_path (USG_ROOT devel_common/cmake/CMakeSettings.cmake
  PATHS 
  ${BLAS_SOURCE_DIR}
  ${BLAS_SOURCE_DIR}/..
  ${BLAS_SOURCE_DIR}/../..
  ${BLAS_SOURCE_DIR}/../../..
  ENV LOFARSOFT
  )

if (USG_ROOT)
  include (${USG_ROOT}/devel_common/cmake/CMakeSettings.cmake)
else (USG_ROOT)
  message (FATAL_ERROR "Unable to locate additional CMake scripts!")
endif (USG_ROOT)

## check for location installation location

find_path (BLAS_INSTALL_PREFIX release_area.txt
  PATHS
  ${BLAS_SOURCE_DIR}
  PATH_SUFFIXES
  ../release
  ../../release
  NO_DEFAULT_PATH
  )

if (BLAS_INSTALL_PREFIX)
  message (STATUS "Installation area located for package BLAS.")
  set (CMAKE_INSTALL_PREFIX ${BLAS_INSTALL_PREFIX}) 
endif (BLAS_INSTALL_PREFIX)

## ==============================================================================
##
##  Package information
##
## ==============================================================================

set (BLAS_VERSION        "" )
set (BLAS_SOURCE_ARCHIVE "blas.tgz" )
set (BLAS_URL            "http://www.netlib.org/blas/${BLAS_SOURCE_ARCHIVE}" )
set (BLAS_SVN            "" )

set (CBLAS_VERSION        "" )
set (CBLAS_SOURCE_ARCHIVE "cblas.tgz" )
set (CBLAS_URL            "http://www.netlib.org/blas/blast-forum/${CBLAS_SOURCE_ARCHIVE}" )
set (CBLAS_SVN            "" )

## ==============================================================================
##
##  Build instructions
##
## ==============================================================================

##____________________________________________________________________
##                                                                BLAS

## Source location

find_file (HAVE_BLAS_SOURCE ${BLAS_SOURCE_ARCHIVE}
  PATHS ${BLAS_SOURCE_DIR}
  )

if (NOT HAVE_BLAS_SOURCE)
  set (HAVE_BLAS_SOURCE ${BLAS_URL})
endif (NOT HAVE_BLAS_SOURCE)

## Build and install package

ExternalProject_Add (blas
  PREFIX ${BLAS_BINARY_DIR}
  DOWNLOAD_DIR ${BLAS_BINARY_DIR}/download
  SOURCE_DIR ${BLAS_BINARY_DIR}/blas
  URL ${HAVE_BLAS_SOURCE}
  CMAKE_ARGS -D CMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
  INSTALL_COMMAND make install
  )

ExternalProject_Add_Step (blas blas_cmake
  COMMAND cp ${BLAS_SOURCE_DIR}/blas.cmake ${BLAS_BINARY_DIR}/blas/CMakeLists.txt
  COMMENT "Moving CMakeLists.txt into place ..."
  DEPENDEES patch
  DEPENDERS configure
  WORKING_DIRECTORY ${BLAS_BINARY_DIR}/blas
  )

##____________________________________________________________________
##                                                               CBLAS

## Source location

find_file (HAVE_CBLAS_SOURCE ${CBLAS_SOURCE_ARCHIVE}
  PATHS ${BLAS_SOURCE_DIR}
  )

if (NOT HAVE_CBLAS_SOURCE)
  set (HAVE_CBLAS_SOURCE ${CBLAS_URL})
endif (NOT HAVE_CBLAS_SOURCE)

## Build and install package

ExternalProject_Add (cblas
  PREFIX ${BLAS_BINARY_DIR}
  DOWNLOAD_DIR ${BLAS_BINARY_DIR}/download
  SOURCE_DIR ${BLAS_BINARY_DIR}/cblas
  URL ${HAVE_CBLAS_SOURCE}
  CMAKE_ARGS -D CMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
  INSTALL_COMMAND make install
  )

ExternalProject_Add_Step (cblas cblas_cmake
  COMMAND cp ${BLAS_SOURCE_DIR}/cblas.cmake ${BLAS_BINARY_DIR}/cblas/CMakeLists.txt
  COMMENT "Moving CMakeLists.txt into place ..."
  DEPENDEES patch
  DEPENDERS configure
  WORKING_DIRECTORY ${BLAS_BINARY_DIR}/cblas
  )

## Package dependencies

add_dependencies (cblas blas)

