
## ------------------------------------------------------------------------------
#
#  [./boost/boost/algorithm/string/detail/CMakeLists.txt]
#
## ------------------------------------------------------------------------------

## ------------------------------------------------------------------------------
## Installation

file (GLOB boost_boost_algorithm_string_detail *.hpp)

install (FILES ${boost_boost_algorithm_string_detail}
  DESTINATION include/boost/algorithm/string/detail
  )
