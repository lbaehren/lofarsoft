
## ------------------------------------------------------------------------------
#
#  [./boost/boost/python/suite/indexing/CMakeLists.txt]
#
## ------------------------------------------------------------------------------

## ------------------------------------------------------------------------------
## Sub-directories

add_subdirectory (detail)

## ------------------------------------------------------------------------------
## Installation

file (GLOB boost_boost_python_suite_indexing *.hpp)

install (FILES ${boost_boost_python_suite_indexing}
  DESTINATION include/boost/python/suite/indexing
  )
