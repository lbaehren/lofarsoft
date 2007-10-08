
## ------------------------------------------------------------------------------
#
#  [./boost/boost/preprocessor/array/CMakeLists.txt]
#
## ------------------------------------------------------------------------------

## ------------------------------------------------------------------------------
## Installation

file (GLOB boost_boost_preprocessor_array *.hpp)

install (FILES ${boost_boost_preprocessor_array}
  DESTINATION include/boost/preprocessor/array
  )
