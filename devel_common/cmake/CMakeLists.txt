##------------------------------------------------------------------------------
## $Id::                                                                       $
##------------------------------------------------------------------------------

## Project name
project (CMake C CXX)

## Required CMake version
cmake_minimum_required (VERSION 2.5)

## ------------------------------------------------------------------------------
## Set CMAKE_MODULE_PATH to load custom CMake modules

if (NOT USG_ROOT)
  find_path (USG_ROOT devel_common/cmake/CMakeSettings.cmake
    PATHS
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/..
    ${PROJECT_SOURCE_DIR}/../..
    ENV LOFARSOFT
    )
  
  if (USG_ROOT)
    include (${USG_ROOT}/devel_common/cmake/CMakeSettings.cmake)
  else (USG_ROOT)
    message (FATAL_ERROR "Unable to locate additional CMake scripts!")
  endif (USG_ROOT)
endif (NOT USG_ROOT)

## ==============================================================================
##
##  Options
##
## ==============================================================================

option (TEST_FIND   "Test find scripts to locate external packages?" YES )
option (TEST_MACROS "Test custom defined macros?"                    YES )
option (TEST_ENV    "Test working with environment variables?"       YES )

## ==============================================================================
##
##  Summary of CMake settings
##
## ==============================================================================

message ("======================================================================")
message (STATUS "Summary of system and CMake settings ...")
message ("======================================================================")

message (STATUS " CMake version ............ : ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}")
message (STATUS " CMAKE_ROOT ............... : ${CMAKE_ROOT}")
message (STATUS " CMAKE_SYSTEM_NAME ........ : ${CMAKE_SYSTEM_NAME}")
message (STATUS " CMAKE_SYSTEM_VERSION ..... : ${CMAKE_SYSTEM_VERSION}")
message (STATUS " CMAKE_SYSTEM ............. : ${CMAKE_SYSTEM}")
message (STATUS " CMAKE_SYSTEM_PROCESSOR ... : ${CMAKE_SYSTEM_PROCESSOR}")
message (STATUS " CMAKE_SYSTEM_64BIT ........: ${CMAKE_SYSTEM_64BIT}")
message (STATUS " CMAKE_SYSTEM_BIG_ENDIAN .. : ${CMAKE_SYSTEM_BIG_ENDIAN}")
message (STATUS " CMAKE_MODULE_PATH ........ : ${CMAKE_MODULE_PATH}")

## ==============================================================================
##
##  Test the CMake scripts to find external packages and libraries
##
## ==============================================================================

if (TEST_FIND)
  
  message ("======================================================================")
  message (STATUS "Testing CMake FindXXX scripts ...")
  message ("======================================================================")
  
  ## Assemble the list of scripts to test
  
  file (GLOB cmake_scripts Find*.cmake)
  
  ## Test file for the various CMake find scripts.
  
  foreach (script ${cmake_scripts})
    string (REGEX REPLACE "${CMake_SOURCE_DIR}/" "" name ${script})
    message ("------------------------------------------------------------")
    message (STATUS "Testing ${name} ...")
    include (${script})
  endforeach (script)
  
endif (TEST_FIND)
  
## ==============================================================================
##
##  Test macros
##
## ==============================================================================

if (TEST_MACROS)
  
  message ("======================================================================")
  message (STATUS "Testing macros definitions ...")
  message ("======================================================================")

  include (MakePackageVersion)
  
  make_package_version (${USG_ROOT}/src/CR-Tools/implement/Data FALSE FALSE)
  message ("------------------------------------------------------------")
  make_package_version (${USG_ROOT}/src/CR-Tools/implement/IO FALSE FALSE)
  message ("------------------------------------------------------------")
  make_package_version (${USG_ROOT}/src/CR-Tools/implement/Imaging FALSE FALSE)
  
endif (TEST_MACROS)

## ==============================================================================
##
##  Test working with environment variables
##
## ==============================================================================

if (TEST_ENV)
  
  message ("======================================================================")
  message (STATUS "Testing access to environment variables ...")
  message ("======================================================================")
  
  message (STATUS "LOFARSOFT  = $ENV{LOFARSOFT}")
  message (STATUS "PYTHONPATH = $ENV{PYTHONPATH}")
  message (STATUS "ROOTSYS    = $ENV{ROOTSYS}")
  
endif (TEST_ENV)

## ==============================================================================
##
##  Test modules/macros provided by CMake itself
##
## ==============================================================================

message ("======================================================================")
message (STATUS "Testing modules/macros provided by CMake itself ...")
message ("======================================================================")

include (CheckLibraryExists)

## [1] libz --------------------------------------

find_library (libz
  NAMES z
  PATHS /usr/lib /usr/local/lib
  )

if (libz)
  message (STATUS "[1] Checking libz for symbols ...")

  foreach (libsymbol compress_block base_dist scan_tree inflate_fast)
    check_library_exists (
      ${libz}
      ${libsymbol}
      ""
      libz_has_${libsymbol}
      )
  endforeach (libsymbol)  
endif (libz)

## [2] libcfitsio --------------------------------

find_library (libcfitsio
  NAMES cfitsio
  PATHS ${CMake_SOURCE_DIR}/../../release/lib /usr/lib /usr/local/lib
  )

if (libcfitsio)
  message (STATUS "[2] Checking libcfitsio for symbols ...")  

  foreach (libsymbol file_open ftp_open file_openfile ffopen ffopentest ffreopen)    
    check_library_exists (
      ${libcfitsio}
      ${libsymbol}
      ""
      libcfitsio_has_${libsymbol}
      )
  endforeach (libsymbol)
else (libcfitsio)
  message (STATUS "[2] Unable to check libcfitsio for symbols; library not found!")  
endif (libcfitsio)

## ==============================================================================

message ("======================================================================")
