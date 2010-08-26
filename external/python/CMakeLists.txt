
## Name of the project
project (Python)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.8)

## -----------------------------------------------------------------------------
## Only force rebuild of the library from the provided source code, if no system
## installation can be found. In order to handle this properly, we use the CMake
## find script

option (Python_FORCE_BUILD       "Force rebuild and local installation?"  NO  )
option (Python_VERBOSE_CONFIGURE "Be verbose during configuration?"       NO  )
option (Python_VERBOSE_BUILD     "Be verbose during build process?"       NO  )

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

if (NOT Python_VERBOSE_CONFIGURE)
  set (Python_FIND_QUIETLY     YES )
endif (NOT Python_VERBOSE_CONFIGURE)

include (FindPython)

## <-- begin build condition --------------------------------------------------->

if (NOT HAVE_Python OR Python_FORCE_BUILD)
  
  set (Python_VERSION        "2.6.5")
  set (Python_SOURCE_ARCHIVE "Python-${Python_VERSION}.tgz")
  set (Python_URL            "http://www.python.org/ftp/python/${Python_VERSION}/${Python_SOURCE_ARCHIVE}")

  ##__________________________________________________________________
  ## Build the project
  
  include (ExternalProject)
  
  find_file (Python_SOURCE ${Python_SOURCE_ARCHIVE} ${PROJECT_SOURCE_DIR})
  
  if (NOT Python_SOURCE)
    set (Python_SOURCE ${Python_URL})
  endif (NOT Python_SOURCE)
  
  ExternalProject_Add (python
    PREFIX ${PROJECT_BINARY_DIR}
    DOWNLOAD_DIR ${PROJECT_BINARY_DIR}
    SOURCE_DIR ${PROJECT_BINARY_DIR}
    URL ${Python_SOURCE}
    BUILD_IN_SOURCE 1
    CONFIGURE_COMMAND ${PROJECT_BINARY_DIR}/configure --prefix=${CMAKE_INSTALL_PREFIX}
    )
  
  ## <-- end build condition ----------------------------------------------------->
  
else (NOT HAVE_Python OR Python_FORCE_BUILD)
  add_custom_target (python
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
    COMMENT "[Python] Found system-wide installation; skipping rebuild!"
    )
endif (NOT HAVE_Python OR Python_FORCE_BUILD)
