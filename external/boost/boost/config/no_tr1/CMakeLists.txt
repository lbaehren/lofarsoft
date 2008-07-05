
## ------------------------------------------------------------------------------
#
#  [./boost/boost/config/no_tr1/CMakeLists.txt]
#
## ------------------------------------------------------------------------------

## ------------------------------------------------------------------------------
## Installation

file (GLOB boost_boost_config_no_tr1 *.hpp)

install (FILES ${boost_boost_config_no_tr1}
  DESTINATION include/boost/config/no_tr1
  )
