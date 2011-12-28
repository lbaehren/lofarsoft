
## ==============================================================================
##
##  CMake project settings
##
## ==============================================================================

cmake_minimum_required (VERSION 2.8.3)
cmake_policy (VERSION 2.8)
include (ExternalProject)

##__________________________________________________________
## Name of the project handled by CMake

project (DSPSR)

## ==============================================================================
##
##  Configuration options
##
## ==============================================================================

set (DSPSR_VERSION          "1.0")
set (DSPSR_SOURCE_DIRECTORY "dspsr-${DSPSR_VERSION}" )

## ==============================================================================
##
##  System inspection
##
## ==============================================================================

## Determine which value to assign to the architecture flag

if (UNIX)
  if (APPLE)
    set (DSPSR_LOGIN_ARCH "darwin")
  else (APPLE)
    set (DSPSR_LOGIN_ARCH "linux")
  endif (APPLE)
endif (UNIX)

## ==============================================================================
##
##  Build & installation instructions
##
## ==============================================================================

ExternalProject_Add (dspsr
  PREFIX ${PROJECT_BINARY_DIR}
  DOWNLOAD_DIR download
  SOURCE_DIR ${PROJECT_SOURCE_DIR}/${DSPSR_SOURCE_DIRECTORY}
  PATCH_COMMAND ${PROJECT_SOURCE_DIR}/${DSPSR_SOURCE_DIRECTORY}/bootstrap PSRHOME=${PROJECT_SOURCE_DIR}/${DSPSR_SOURCE_DIRECTORY} LOGIN_ARCH=${DSPSR_LOGIN_ARCH} --with-psrchive-dir=${CMAKE_INSTALL_PREFIX} --with-psrdada-dir=${CMAKE_INSTALL_PREFIX}/psrdada
  CONFIGURE_COMMAND ${PROJECT_SOURCE_DIR}/${DSPSR_SOURCE_DIRECTORY}/configure PSRHOME=${PROJECT_SOURCE_DIR}/${DSPSR_SOURCE_DIRECTORY} LOGIN_ARCH=${DSPSR_LOGIN_ARCH} --with-psrchive-dir=${CMAKE_INSTALL_PREFIX} --with-psrdada-dir=${CMAKE_INSTALL_PREFIX}/psrdada LIBS=-ldl
      )
