PROJECT (CMAKE_TEST)

file (GLOB cmake_scripts Find*.cmake)

## -----------------------------------------------------------------------------
## Test file for the various CMake find scripts.

message (STATUS "Testing CMake find scripts... ++++++++++++++++++++++++++++++++")

message (${CMAKE_TEST_SOURCE_DIR})

foreach (script ${cmake_scripts})
  string (REGEX REPLACE "${CMAKE_TEST_SOURCE_DIR}/" "" name ${script})
  message ("++++++ Testing ${name} ...")
  include (${script})
endforeach (script)

message (STATUS "Testing completed ++++++++++++++++++++++++++++++++++++++++++++")
