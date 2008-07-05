
## ------------------------------------------------------------------------------
#
#  [./boost/boost/preprocessor/logical/CMakeLists.txt]
#
## ------------------------------------------------------------------------------

## ------------------------------------------------------------------------------
## Installation

file (GLOB boost_boost_preprocessor_logical *.hpp)

install (FILES ${boost_boost_preprocessor_logical}
  DESTINATION include/boost/preprocessor/logical
  )
