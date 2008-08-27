
## ------------------------------------------------------------------------------
#
#  [./boost/boost/thread/pthread/CMakeLists.txt]
#
## ------------------------------------------------------------------------------

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.5)

## ------------------------------------------------------------------------------
## Installation

file (GLOB boost_boost_thread_pthread *.hpp)

install (FILES ${boost_boost_thread_pthread}
  DESTINATION include/boost/thread/pthread
  )
