
## ------------------------------------------------------------------------------
#
#  [./boost/boost/preprocessor/debug/CMakeLists.txt]
#
## ------------------------------------------------------------------------------

## ------------------------------------------------------------------------------
## Installation

file (GLOB boost_boost_preprocessor_debug *.hpp)

install (FILES ${boost_boost_preprocessor_debug}
  DESTINATION include/boost/preprocessor/debug
  )
