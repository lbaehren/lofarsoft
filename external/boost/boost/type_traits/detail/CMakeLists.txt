
## ------------------------------------------------------------------------------
#
#  [./boost/boost/type_traits/detail/CMakeLists.txt]
#
## ------------------------------------------------------------------------------

## ------------------------------------------------------------------------------
## Installation

file (GLOB boost_boost_type_traits_detail *.hpp)

install (FILES ${boost_boost_type_traits_detail}
  DESTINATION include/boost/type_traits/detail
  )
