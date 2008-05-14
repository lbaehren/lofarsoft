##------------------------------------------------------------------------------
## $Id::                                                                       $
##------------------------------------------------------------------------------

## Project name
project (CMAKE_TEST)

## Required CMake version
cmake_minimum_required (VERSION 2.4)

include (./CMakeSettings.cmake)

## -----------------------------------------------------------------------------

## Assemble the list of scripts to test

file (GLOB cmake_scripts Find*.cmake)

## Test file for the various CMake find scripts.

message (STATUS "Testing CMake find scripts ... ")

message (${CMAKE_TEST_SOURCE_DIR})

foreach (script ${cmake_scripts})
  string (REGEX REPLACE "${CMAKE_TEST_SOURCE_DIR}/" "" name ${script})
  message ("------------------------------------------------------------")
  message (STATUS "Testing ${name} ...")
  include (${script})
endforeach (script)

message ("------------------------------------------------------------")
message (STATUS "Testing completed")
message ("------------------------------------------------------------")
