
## ------------------------------------------------------------------------------
#
#  [./boost/boost/python/CMakeLists.txt]
#
## ------------------------------------------------------------------------------

## Name of the project
project (BOOST_PROGRAM_OPTIONS)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.5)

## ------------------------------------------------------------------------------
## Sub-directories

add_subdirectory (detail)

## ------------------------------------------------------------------------------
## Installation

file (GLOB boost_boost_program_options *.hpp)

install (FILES ${boost_boost_program_options}
  DESTINATION include/boost/program_options
  )
