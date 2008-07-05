
## ------------------------------------------------------------------------------
#
#  [./boost/boost/config/compiler/CMakeLists.txt]
#
## ------------------------------------------------------------------------------

## ------------------------------------------------------------------------------
## Installation

file (GLOB boost_boost_config_compiler *.hpp)

install (FILES ${boost_boost_config_compiler}
  DESTINATION include/boost/config/compiler
  )
