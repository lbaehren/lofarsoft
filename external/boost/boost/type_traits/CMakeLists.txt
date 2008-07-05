
## ------------------------------------------------------------------------------
#
#  [./boost/boost/type_traits/CMakeLists.txt]
#
## ------------------------------------------------------------------------------

add_subdirectory (detail)

## ------------------------------------------------------------------------------
## Installation

file (GLOB boost_boost_type_traits *.hpp)

install (FILES ${boost_boost_type_traits}
  DESTINATION include/boost/type_traits
  )
