##------------------------------------------------------------------------------
## $Id::                                                                       $
##------------------------------------------------------------------------------

## Project name
project (CMake C CXX)

## Required CMake version
cmake_minimum_required (VERSION 2.4)

include (./CMakeSettings.cmake)

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
message (STATUS " CMAKE_SYSTEM ............. : ${CMAKE_SYSTEM}")
message (STATUS " CMAKE_SYSTEM_VERSION ..... : ${CMAKE_SYSTEM_VERSION}")
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
  
  make_package_version (./../../src/CR-Tools/implement/Data FALSE FALSE)
  message ("------------------------------------------------------------")
  make_package_version (./../../src/CR-Tools/implement/IO FALSE FALSE)
  
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
  message (STATUS "PYTHONHOME = $ENV{PYTHONHOME}")
  message (STATUS "ROOTSYS    = $ENV{ROOTSYS}")
  
endif (TEST_ENV)

## ==============================================================================

message ("======================================================================")
