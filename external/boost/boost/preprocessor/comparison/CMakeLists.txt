
## ------------------------------------------------------------------------------
#
#  [./boost/boost/preprocessor/comparison/CMakeLists.txt]
#
## ------------------------------------------------------------------------------

## ------------------------------------------------------------------------------
## Installation

file (GLOB boost_boost_preprocessor_comparison *.hpp)

install (FILES ${boost_boost_preprocessor_comparison}
  DESTINATION include/boost/preprocessor/comparison
  )
