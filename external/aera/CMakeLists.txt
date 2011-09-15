
## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.8)

## Name of the project
project (AERA)

# Project version (major.minor.patch)
set (AERA_VERSION_MAJOR 1 )
set (AERA_VERSION_MINOR 1 )
set (AERA_VERSION_PATCH 0 )
set (AERA_VERSION_ID "")
set (AERA_SOURCE_VERSION "${AERA_VERSION_MAJOR}.${AERA_VERSION_MINOR}.${AERA_VERSION_PATCH}" )

if (AERA_VERSION_ID)
  set (AERA_SOURCE_VERSION "${AERA_SOURCE_VERSION}-${AERA_VERSION_ID}")
endif (AERA_VERSION_ID)

set (AERA_SOURCE_ARCHIVE "AERA_Datareader-${AERA_SOURCE_VERSION}.tar.gz" )
set (AERA_URL         "" )


## -----------------------------------------------------------------------------
## Only force rebuild of the library from the provided source code, if no system
## installation can be found. In order to handle this properly, we use the CMake
## find script

option (AERA_FORCE_BUILD       "Force rebuild and local installation?"    NO  )
option (AERA_VERBOSE_CONFIGURE "Be verbose during configuration?"         NO  )
option (AERA_VERBOSE_BUILD     "Be verbose during build process?"         NO  )

## ------------------------------------------------------------------------------
## Set CMAKE_MODULE_PATH to load custom CMake modules

if (NOT LUS_ROOT)
  find_path (LUS_ROOT devel_common/cmake/CMakeSettings.cmake
    PATHS
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../..
    ${CMAKE_CURRENT_SOURCE_DIR}/../../..
    $ENV{LOFARSOFT}
    )
endif (NOT LUS_ROOT)

if (LUS_ROOT)
  include (${LUS_ROOT}/devel_common/cmake/CMakeSettings.cmake)
else (LUS_ROOT)
  message (FATAL_ERROR "Unable to locate additional CMake scripts!")
endif (LUS_ROOT)

## ------------------------------------------------------------------------------
## Check if the program/package/library is already available

if (NOT AERA_VERBOSE_CONFIGURE)
  set (AERA_FIND_QUIETLY     YES )
endif (NOT AERA_VERBOSE_CONFIGURE)

set (AERA_FOUND FALSE)
include (FindAERA)


## <-- begin build condition --------------------------------------------------->

if (${AERA_FOUND})
  if (${AERA_SOURCE_VERSION} VERSION_GREATER ${AERA_VERSION})
    set (AERA_FORCE_BUILD TRUE)

  endif (${AERA_SOURCE_VERSION} VERSION_GREATER ${AERA_VERSION})
endif (${AERA_FOUND})

message (STATUS "AERA_FORCE_BUILD: ${AERA_FORCE_BUILD}")
if (NOT AERA_FOUND OR AERA_FORCE_BUILD)

  ##__________________________________________________________________
  ## Build the project

  include (ExternalProject)

  find_file (AERA_FOUND_SOURCE ${AERA_SOURCE_ARCHIVE} ${AERA_SOURCE_DIR})

  message (STATUS "AERA_SOURCE_ARCHIVE: ${AERA_SOURCE_ARCHIVE}")
  message (STATUS "AERA_SOURCE_DIR: ${AERA_SOURCE_DIR}")

  if (NOT AERA_FOUND_SOURCE)
    set (AERA_FOUND_SOURCE ${AERA_URL})
  endif (NOT AERA_FOUND_SOURCE)

  message (STATUS "AERA_FOUND_SOURCE: ${AERA_FOUND_SOURCE}")

  ExternalProject_Add (aera
    PREFIX ${PROJECT_BINARY_DIR}
    DOWNLOAD_DIR ${PROJECT_BINARY_DIR}/download
    SOURCE_DIR ${PROJECT_BINARY_DIR}/source
    URL ${AERA_FOUND_SOURCE}
    BUILD_IN_SOURCE 1
    CONFIGURE_COMMAND ${CMAKE_COMMAND} -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
    )

  ## <-- end build condition ----------------------------------------------------->

else (NOT AERA_FOUND OR AERA_FORCE_BUILD)
  add_custom_target (aera
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
    COMMENT "[AERA] Found system-wide installation; skipping rebuild!"
    )
endif (NOT AERA_FOUND OR AERA_FORCE_BUILD)
