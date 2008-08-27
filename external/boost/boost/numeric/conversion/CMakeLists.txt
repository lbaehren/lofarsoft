
## ------------------------------------------------------------------------------
#
#  [./boost/boost/numeric/conversion/CMakeLists.txt]
#
## ------------------------------------------------------------------------------

## ------------------------------------------------------------------------------
## Sub-directories

add_subdirectory (detail)

## ------------------------------------------------------------------------------
## Installation

file (GLOB boost_boost_numeric_conversion *.hpp)

install (FILES ${boost_boost_numeric_conversion}
  DESTINATION include/boost/numeric/conversion
  )
