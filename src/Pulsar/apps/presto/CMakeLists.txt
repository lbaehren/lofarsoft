##------------------------------------------------------------------------------
## $Id:: CMakeLists.txt 2987 2009-09-04 12:43:48Z baehren                      $
##------------------------------------------------------------------------------

## Name of the project
PROJECT (presto C Fortran)

## Minimum required version of CMake to configure the project
#cmake_minimum_required (VERSION 2.6)

## =============================================================================
##
##  Project sub-directories and header locations
##
## =============================================================================

include_directories (${PULSAR_SOURCE_DIR}/apps/presto/include)

add_subdirectory (src)
add_subdirectory (bin)

if (PULSAR_BUILD_TESTS)
  add_subdirectory (tests)
endif (PULSAR_BUILD_TESTS)

