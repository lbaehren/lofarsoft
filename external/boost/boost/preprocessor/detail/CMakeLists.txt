
## ------------------------------------------------------------------------------
#
#  [./boost/boost/preprocessor/detail/CMakeLists.txt]
#
## ------------------------------------------------------------------------------

## ------------------------------------------------------------------------------
## Installation

file (GLOB boost_boost_preprocessor_detail *.hpp)

install (FILES ${boost_boost_preprocessor_detail}
  DESTINATION include/boost/preprocessor/detail
  )
