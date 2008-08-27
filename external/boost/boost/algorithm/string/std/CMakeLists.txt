
## ------------------------------------------------------------------------------
#
#  [./boost/boost/algorithm/string/std/CMakeLists.txt]
#
## ------------------------------------------------------------------------------

## ------------------------------------------------------------------------------
## Installation

file (GLOB boost_boost_algorithm_string_std *.hpp)

install (FILES ${boost_boost_algorithm_string_std}
  DESTINATION include/boost/algorithm/string/std
  )
