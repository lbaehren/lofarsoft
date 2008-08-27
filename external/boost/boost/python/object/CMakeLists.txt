
## ------------------------------------------------------------------------------
#
#  [./boost/boost/python/object/CMakeLists.txt]
#
## ------------------------------------------------------------------------------

## ------------------------------------------------------------------------------
## Installation

file (GLOB boost_boost_python_object *.hpp)

install (FILES ${boost_boost_python_object}
  DESTINATION include/boost/python/object
  )
