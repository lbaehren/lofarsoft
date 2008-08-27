
## ------------------------------------------------------------------------------
#
#  [./boost/boost/python/CMakeLists.txt]
#
## ------------------------------------------------------------------------------

## Name of the project
project (BOOST_PYTHON)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.5)

## ------------------------------------------------------------------------------
## Sub-directories

add_subdirectory (converter)
add_subdirectory (detail)
add_subdirectory (object)
add_subdirectory (suite)

## ------------------------------------------------------------------------------
## Installation

file (GLOB boost_boost_python *.hpp)

install (FILES ${boost_boost_python}
  DESTINATION include/boost/python
  )
