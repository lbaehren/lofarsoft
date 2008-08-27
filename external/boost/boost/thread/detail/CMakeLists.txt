
## ------------------------------------------------------------------------------
#
#  [./boost/boost/thread/detail/CMakeLists.txt]
#
## ------------------------------------------------------------------------------

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.5)

## ------------------------------------------------------------------------------
## Installation

file (GLOB boost_boost_thread_detail *.hpp)

install (FILES ${boost_boost_thread_detail}
  DESTINATION include/boost/thread/detail
  )
