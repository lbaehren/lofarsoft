
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
##                                                          Configuration options
##
## ==============================================================================

set (DSPSR_VERSION          "1.0")
set (DSPSR_SOURCE_DIRECTORY "dspsr-${DSPSR_VERSION}" )
set (DSPSR_SOURCE_ARCHIVE   "${DSPSR_SOURCE_DIRECTORY}.tgz")

## ==============================================================================
##
##                                                              System inspection
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
##                                              Build & installation instructions
##
##  We need to define a custom DOWNLOAD_COMMAND, as the project's sources are
##  not provided as a tar-archive, but as a fully expanded code tree; in order
##  to avoid writing into the original source directories, we need to create an
##  archive first, which afterwards is being dropped into the build directory.
##
## ==============================================================================

ExternalProject_Add (dspsr
  PREFIX ${PROJECT_BINARY_DIR}
  DOWNLOAD_DIR download
  SOURCE_DIR source
  BUILD_IN_SOURCE 1
  URL ${PROJECT_BINARY_DIR}/${DSPSR_SOURCE_ARCHIVE}
  PATCH_COMMAND ${PROJECT_BINARY_DIR}/source/bootstrap PSRHOME=${PROJECT_BINARY_DIR}/source LOGIN_ARCH=${DSPSR_LOGIN_ARCH} --with-psrchive-dir=${CMAKE_INSTALL_PREFIX} --with-psrdada-dir=${CMAKE_INSTALL_PREFIX}/psrdada
  CONFIGURE_COMMAND ${PROJECT_BINARY_DIR}/source/configure PSRHOME=${PROJECT_BINARY_DIR}/source LOGIN_ARCH=${DSPSR_LOGIN_ARCH} --with-psrchive-dir=${CMAKE_INSTALL_PREFIX} --with-psrdada-dir=${CMAKE_INSTALL_PREFIX}/psrdada LIBS=-ldl
  INSTALL_COMMAND ${CMAKE_BUILD_TOOL} install prefix=${CMAKE_INSTALL_PREFIX} -I${CMAKE_INSTALL_PREFIX}/psrdada/include   
  )

## Create tar-archive

ExternalProject_Add_Step(dspsr create_archive
  COMMAND ${TAR_EXECUTABLE} -czf ${PROJECT_BINARY_DIR}/${DSPSR_SOURCE_ARCHIVE} ${DSPSR_SOURCE_DIRECTORY}
  COMMENT "Creating tar-archive ${DSPSR_SOURCE_ARCHIVE} ..."
  DEPENDERS download
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
  )
