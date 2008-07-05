
## ------------------------------------------------------------------------------
#
#  [./boost/boost/preprocessor/arithmetic/CMakeLists.txt]
#
## ------------------------------------------------------------------------------

## ------------------------------------------------------------------------------
## Installation

file (GLOB boost_boost_preprocessor_arithmetic *.hpp)

install (FILES ${boost_boost_preprocessor_arithmetic}
  DESTINATION include/boost/preprocessor/arithmetic
  )
