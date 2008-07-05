
## ------------------------------------------------------------------------------
#
#  [./boost/boost/numeric/conversion/detail/CMakeLists.txt]
#
## ------------------------------------------------------------------------------

## ------------------------------------------------------------------------------
## Installation

file (GLOB boost_boost_numeric_conversion_detail *.hpp)

install (FILES ${boost_boost_numeric_conversion_detail}
  DESTINATION include/boost/numeric/conversion/detail
  )
