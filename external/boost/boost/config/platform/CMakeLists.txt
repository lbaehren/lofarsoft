
## ------------------------------------------------------------------------------
#
#  [./boost/boost/config/platform/CMakeLists.txt]
#
## ------------------------------------------------------------------------------

## ------------------------------------------------------------------------------
## Installation

file (GLOB boost_boost_config_platform *.hpp)

install (FILES ${boost_boost_config_platform}
  DESTINATION include/boost/config/platform
  )
