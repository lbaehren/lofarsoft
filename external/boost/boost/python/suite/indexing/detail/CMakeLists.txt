
## ------------------------------------------------------------------------------
#
#  [./boost/boost/python/suite/indexing/detail/CMakeLists.txt]
#
## ------------------------------------------------------------------------------

## ------------------------------------------------------------------------------
## Installation

file (GLOB boost_boost_python_suite_indexing_detail *.hpp)

install (FILES ${boost_boost_python_suite_indexing_detail}
  DESTINATION include/boost/python/suite/indexing/detail
  )
