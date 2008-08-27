
## ------------------------------------------------------------------------------
#
#  [./boost/boost/bind/CMakeLists.txt]
#
## ------------------------------------------------------------------------------

## ------------------------------------------------------------------------------
## Installation

file (GLOB boost_boost_bind *.hpp)

install (FILES ${boost_boost_bind}
  DESTINATION include/boost/bind
  )
