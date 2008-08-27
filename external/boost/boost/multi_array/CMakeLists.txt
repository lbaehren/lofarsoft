
## ------------------------------------------------------------------------------
#
#  [./boost/boost/multi_array/CMakeLists.txt]
#
## ------------------------------------------------------------------------------

## ------------------------------------------------------------------------------
## Installation

file (GLOB boost_boost_multi_array *.hpp)

install (FILES ${boost_boost_multi_array}
  DESTINATION include/boost/multi_array
  )
