
## Name of the project
project (PELICAN)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.8)

## -----------------------------------------------------------------------------
## Only force rebuild of the library from the provided source code, if no system
## installation can be found. In order to handle this properly, we use the CMake
## find script

option (PELICAN_FORCE_BUILD       "Force rebuild and local installation?"    NO  )
option (PELICAN_VERBOSE_CONFIGURE "Be verbose during configuration?"         NO  )
option (PELICAN_VERBOSE_BUILD     "Be verbose during build process?"         NO  )

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

if (NOT PELICAN_VERBOSE_CONFIGURE)
  set (GIT_FIND_QUIETLY         YES )
  set (PELICAN_FIND_QUIETLY     YES )
endif (NOT PELICAN_VERBOSE_CONFIGURE)

include (FindGit)
include (FindPELICAN)

## Check if we have a recent enough version of Git

set (GIT_VERSION_REQUIRED "1.6.5")

if (GIT_EXECUTABLE AND GIT_VERSION)
  if (${GIT_VERSION} STRLESS ${GIT_VERSION_REQUIRED})
    set (GIT_FOUND_REQUIRED FALSE)
  else (${GIT_VERSION} STRLESS ${GIT_VERSION_REQUIRED})
    set (GIT_FOUND_REQUIRED TRUE)
  endif (${GIT_VERSION} STRLESS ${GIT_VERSION_REQUIRED})
endif (GIT_EXECUTABLE AND GIT_VERSION)

## <-- begin build condition --------------------------------------------------->

if (NOT HAVE_PELICAN OR PELICAN_FORCE_BUILD)
  
  set (PELICAN_VERSION        "" )
  set (PELICAN_SOURCE_ARCHIVE "" )
  set (PELICAN_URL            "" )
  set (PELICAN_GIT            "http://github.com/pelican/pelican.git" )
  
  ##__________________________________________________________
  ## Required packages
  
  find_path (CPPUNIT_INCLUDE_DIR cppunit/TestAssert.h
    PATHS /sw /usr /usr/local ${CMAKE_INSTALL_PREFIX}
    PATH_SUFFIXES include
    )
  
  find_path (CPPUNIT_LIBRARIES libcppunit
    PATHS /sw /usr /usr/local ${CMAKE_INSTALL_PREFIX}
    PATH_SUFFIXES lib
    )
  
  if (CPPUNIT_INCLUDE_DIR AND CPPUNIT_LIBRARIES)
    set (CPPUNIT_FOUND TRUE)
  endif (CPPUNIT_INCLUDE_DIR AND CPPUNIT_LIBRARIES)
  
  ##__________________________________________________________________
  ## Build the project
  
  include (ExternalProject)
  
  if (GIT_FOUND_REQUIRED)
    
    ExternalProject_Add (pelican
      PREFIX ${PROJECT_BINARY_DIR}
      DOWNLOAD_DIR download
      SOURCE_DIR source
      GIT_REPOSITORY ${PELICAN_GIT}
      CONFIGURE_COMMAND ${CMAKE_COMMAND} -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX} -DCPPUNIT_INCLUDE_DIR=${CPPUNIT_INCLUDE_DIR} -DDCPPUNIT_LIBRARIES=${CPPUNIT_LIBRARIES} -DCMAKE_BUILD_TYPE=Release ${PROJECT_BINARY_DIR}/source/pelican
      INSTALL_COMMAND make install
      )
    
  else (GIT_FOUND_REQUIRED)
    
    add_custom_target (pelican
      WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
      COMMENT "[PELICAN] Git missing or too old - unable to retrieve source code!"
      )
    
  endif (GIT_FOUND_REQUIRED)
  
  ## <-- end build condition ----------------------------------------------------->
  
else (NOT HAVE_PELICAN OR PELICAN_FORCE_BUILD)
  add_custom_target (pelican
    WORKING_DIRECTORY ${PELICAN_BINARY_DIR}
    COMMENT "[PELICAN] Found system-wide installation; skipping rebuild!"
    )
endif (NOT HAVE_PELICAN OR PELICAN_FORCE_BUILD)
