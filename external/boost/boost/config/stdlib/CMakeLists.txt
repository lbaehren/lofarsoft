
## ------------------------------------------------------------------------------
#
#  [./boost/boost/config/stdlib/CMakeLists.txt]
#
## ------------------------------------------------------------------------------

## ------------------------------------------------------------------------------
## Installation

file (GLOB boost_boost_config_stdlib *.hpp)

install (FILES ${boost_boost_config_stdlib}
  DESTINATION include/boost/config/stdlib
  )
