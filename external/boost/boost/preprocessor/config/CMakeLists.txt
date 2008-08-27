
## ------------------------------------------------------------------------------
#
#  [./boost/boost/preprocessor/config/CMakeLists.txt]
#
## ------------------------------------------------------------------------------

## ------------------------------------------------------------------------------
## Installation

file (GLOB boost_boost_preprocessor_config *.hpp)

install (FILES ${boost_boost_preprocessor_config}
  DESTINATION include/boost/preprocessor/config
  )
