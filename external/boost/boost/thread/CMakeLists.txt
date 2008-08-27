
## ------------------------------------------------------------------------------
#
#  [./boost/boost/thread/CMakeLists.txt]
#
## ------------------------------------------------------------------------------

## Name of the project
project (BOOST_THREAD)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.5)

## ------------------------------------------------------------------------------
## Sub-directories

add_subdirectory (detail)

## ------------------------------------------------------------------------------
## Installation

file (GLOB boost_boost_thread *.hpp)

install (FILES ${boost_boost_thread}
  DESTINATION include/boost/thread
  )
