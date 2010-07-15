
## Name of the project
project (CPPUNIT)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.8)

include (ExternalProject)

## -----------------------------------------------------------------------------
## Only force rebuild of the library from the provided source code, if no system
## installation can be found. In order to handle this properly, we use the CMake
## find script

option (CPPUNIT_FORCE_BUILD       "Force rebuild and local installation?"  NO )
option (CPPUNIT_VERBOSE_CONFIGURE "Be verbose during configuration?"       NO )
option (CPPUNIT_VERBOSE_BUILD     "Be verbose during build process?"       NO )

## ------------------------------------------------------------------------------
## Set CMAKE_MODULE_PATH to load custom CMake modules

find_path (USG_ROOT devel_common/cmake/CMakeSettings.cmake
  PATHS 
  ${CPPUNIT_SOURCE_DIR}
  ${CPPUNIT_SOURCE_DIR}/..
  ${CPPUNIT_SOURCE_DIR}/../..
  ${CPPUNIT_SOURCE_DIR}/../../..
  $ENV{LOFARSOFT}
  )

if (USG_ROOT)
  include (${USG_ROOT}/devel_common/cmake/CMakeSettings.cmake)
else (USG_ROOT)
  message (FATAL_ERROR "[CPPUNIT] Unable to locate additional CMake scripts!")
endif (USG_ROOT)

## ------------------------------------------------------------------------------
## Check if the program/package/library is already available

## <-- begin build condition --------------------------------------------------->

if (NOT HAVE_CPPUNIT OR CPPUNIT_FORCE_BUILD)
  
  set (CPPUNIT_SVN "https://cppunit.svn.sourceforge.net/svnroot/cppunit/trunk/cppunit" )
  
  ExternalProject_Add (cppunit
    PREFIX ${CPPUNIT_BINARY_DIR}
    DOWNLOAD_DIR ${CPPUNIT_BINARY_DIR}/download
    SOURCE_DIR ${CPPUNIT_BINARY_DIR}/source
    SVN_REPOSITORY ${CPPUNIT_SVN}
    BUILD_IN_SOURCE 1
    PATCH_COMMAND ${CPPUNIT_BINARY_DIR}/source/autogen.sh
    CONFIGURE_COMMAND ${CPPUNIT_BINARY_DIR}/source/configure --prefix=${CMAKE_INSTALL_PREFIX}
  INSTALL_COMMAND make install
    )
  
  ## <-- end build condition ----------------------------------------------------->
  
else (NOT HAVE_CPPUNIT OR CPPUNIT_FORCE_BUILD)
  add_custom_target (cppunit
    WORKING_DIRECTORY ${CPPUNIT_BINARY_DIR}
    COMMENT "[CPPUNIT] Found system-wide installation; skipping rebuild!"
    )
endif (NOT HAVE_CPPUNIT OR CPPUNIT_FORCE_BUILD)
