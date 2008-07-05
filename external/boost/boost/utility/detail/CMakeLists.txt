
## ------------------------------------------------------------------------------
#
#  [./boost/boost/utility/detail/CMakeLists.txt]
#
## ------------------------------------------------------------------------------

## ------------------------------------------------------------------------------
## Installation

file (GLOB boost_boost_utility_detail *.hpp)

install (FILES ${boost_boost_utility_detail}
  DESTINATION include/boost/utility/detail
  )
