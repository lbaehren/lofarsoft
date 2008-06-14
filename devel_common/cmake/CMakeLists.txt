##------------------------------------------------------------------------------
## $Id::                                                                       $
##------------------------------------------------------------------------------

## Project name
project (CMake)

## Required CMake version
cmake_minimum_required (VERSION 2.4)

include (./CMakeSettings.cmake)

## ==============================================================================
##
##  Test the CMake scripts to find external packages and libraries
##
## ==============================================================================

## Assemble the list of scripts to test

file (GLOB cmake_scripts Find*.cmake)

## Test file for the various CMake find scripts.

message (STATUS "Testing CMake find scripts ... ")

message (${CMake_SOURCE_DIR})

foreach (script ${cmake_scripts})
  string (REGEX REPLACE "${CMake_SOURCE_DIR}/" "" name ${script})
  message ("------------------------------------------------------------")
  message (STATUS "Testing ${name} ...")
  include (${script})
endforeach (script)

message ("------------------------------------------------------------")
message (STATUS "Testing completed")
message ("------------------------------------------------------------")

## ==============================================================================
##
##  Test macros
##
## ==============================================================================

include (MakePackageVersion)

make_package_version (./../../src/CR-Tools/implement/Data TRUE TRUE)
make_package_version (./../../src/CR-Tools/implement/IO FALSE FALSE)
