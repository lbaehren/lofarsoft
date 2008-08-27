
## ------------------------------------------------------------------------------
#
#  [./boost/boost/python/CMakeLists.txt]
#
## ------------------------------------------------------------------------------

## Name of the project
project (BOOST_WAVE)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.5)

## ------------------------------------------------------------------------------
## Sub-directories

add_subdirectory (cpplexer)
add_subdirectory (grammars)
add_subdirectory (util)

## ------------------------------------------------------------------------------
## Installation

file (GLOB boost_boost_wave *.hpp)

install (FILES ${boost_boost_wave}
  DESTINATION include/boost/wave
  )
