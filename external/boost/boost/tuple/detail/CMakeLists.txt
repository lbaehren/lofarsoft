
## ------------------------------------------------------------------------------
#
#  [./boost/boost/tuple/detail/CMakeLists.txt]
#
## ------------------------------------------------------------------------------

## ------------------------------------------------------------------------------
## Installation

file (GLOB boost_boost_tuple_detail *.hpp)

install (FILES ${boost_boost_tuple_detail}
  DESTINATION include/boost/tuple/detail
  )
