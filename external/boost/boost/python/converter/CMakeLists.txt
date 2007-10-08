
## ------------------------------------------------------------------------------
#
#  [./boost/boost/python/converter/CMakeLists.txt]
#
## ------------------------------------------------------------------------------

## ------------------------------------------------------------------------------
## Installation

file (GLOB boost_boost_python_converter *.hpp)

install (FILES ${boost_boost_python_converter}
  DESTINATION include/boost/python/converter
  )
