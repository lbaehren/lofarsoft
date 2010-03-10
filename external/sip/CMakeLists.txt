
## Name of the project
project (SIP)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.8)

## -----------------------------------------------------------------------------
## Only force rebuild of the library from the provided source code, if no system
## installation can be found. In order to handle this properly, we use the CMake
## find script

option (SIP_FORCE_BUILD       "Force rebuild and local installation?"  NO  )
option (SIP_VERBOSE_CONFIGURE "Be verbose during configuration?"       YES )
option (SIP_VERBOSE_BUILD     "Be verbose during build process?"       NO  )
option (SIP_CMAKE_BUILD       "Use CMake to configure and build?"      NO  )

## ------------------------------------------------------------------------------
## Set CMAKE_MODULE_PATH to load custom CMake modules

find_path (USG_ROOT devel_common/cmake/CMakeSettings.cmake
  PATHS 
  ${SIP_SOURCE_DIR}
  ${SIP_SOURCE_DIR}/..
  ${SIP_SOURCE_DIR}/../..
  ${SIP_SOURCE_DIR}/../../..
  $ENV{LOFARSOFT}
  )

if (USG_ROOT)
  include (${USG_ROOT}/devel_common/cmake/CMakeSettings.cmake)
else (USG_ROOT)
  message (FATAL_ERROR "[Sip] Unable to locate additional CMake scripts!")
endif (USG_ROOT)

## ------------------------------------------------------------------------------
## Check if the program/package/library is already available

if (NOT SIP_VERBOSE_CONFIGURE)
  set (PYTHON_FIND_QUIETLY YES )
  set (SIP_FIND_QUIETLY    YES )
endif (NOT SIP_VERBOSE_CONFIGURE)

include (FindPython)
#include (FindSIP)

## <-- begin build condition --------------------------------------------------->

if (NOT HAVE_SIP OR SIP_FORCE_BUILD)
  
  ##__________________________________________________________________
  ## Additional CMake modules

  include (ExternalProject)

  ##__________________________________________________________________
  ## Configure and build the project

  set (SIP_VERSION "4.10")
  set (SIP_DIRECTORY "sip-${SIP_VERSION}")
  set (SIP_TAR_ARCHIVE "${SIP_DIRECTORY}.tar.gz")
  set (SIP_URL "http://www.riverbankcomputing.co.uk/static/Downloads/sip4/${SIP_TAR_ARCHIVE}")

  ##__________________________________________________________________
  ## Check if a tar-archive with the source code is available already;
  ## if this is not the case we need to fetch it from the server.
  
  find_file (have_sip_tgz ${SIP_TAR_ARCHIVE} ${SIP_SOURCE_DIR})

  if (have_sip_tgz) 
    ExternalProject_Add (sip
      PREFIX ${SIP_BINARY_DIR}
      DOWNLOAD_DIR ${SIP_BINARY_DIR}
      SOURCE_DIR ${SIP_BINARY_DIR}
      BINARY_DIR ${SIP_BINARY_DIR}/build
      URL ${have_sip_tgz}
      CONFIGURE_COMMAND ${PYTHON_EXECUTABLE} ${SIP_BINARY_DIR}/configure.py -b ${CMAKE_INSTALL_PREFIX}/bin -d ${CMAKE_INSTALL_PREFIX}/lib/python -e ${CMAKE_INSTALL_PREFIX}/include/sip -v ${CMAKE_INSTALL_PREFIX}/share/sip
      )
  else (have_sip_tgz)
    ExternalProject_Add (sip
      PREFIX ${SIP_BINARY_DIR}
      DOWNLOAD_DIR ${SIP_BINARY_DIR}
      SOURCE_DIR ${SIP_BINARY_DIR}
      BINARY_DIR ${SIP_BINARY_DIR}/build
      URL ${SIP_URL}
      CONFIGURE_COMMAND ${PYTHON_EXECUTABLE} ${SIP_BINARY_DIR}/configure.py -b ${CMAKE_INSTALL_PREFIX}/bin -d ${CMAKE_INSTALL_PREFIX}/lib/python -e ${CMAKE_INSTALL_PREFIX}/include/sip -v ${CMAKE_INSTALL_PREFIX}/share/sip
      )
  endif (have_sip_tgz)

  ## <-- end build condition ----------------------------------------------------->
  
else (NOT HAVE_SIP OR SIP_FORCE_BUILD)
  add_custom_target (sip
    WORKING_DIRECTORY ${SIP_BINARY_DIR}
    COMMENT "[Sip] Found system-wide installation; skipping rebuild!"
    )
endif (NOT HAVE_SIP OR SIP_FORCE_BUILD)
