
## ------------------------------------------------------------------------------
#
#  [./boost/boost/python/detail/CMakeLists.txt]
#
## ------------------------------------------------------------------------------

## ------------------------------------------------------------------------------
## Installation

file (GLOB boost_boost_python_detail *.hpp)

install (FILES ${boost_boost_python_detail}
  DESTINATION include/boost/python/detail
  )
