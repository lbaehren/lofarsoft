
## ------------------------------------------------------------------------------
#
#  [./boost/boost/preprocessor/tuple/CMakeLists.txt]
#
## ------------------------------------------------------------------------------

## ------------------------------------------------------------------------------
## Installation

file (GLOB boost_boost_preprocessor_tuple *.hpp)

install (FILES ${boost_boost_preprocessor_tuple}
  DESTINATION include/boost/preprocessor/tuple
  )
