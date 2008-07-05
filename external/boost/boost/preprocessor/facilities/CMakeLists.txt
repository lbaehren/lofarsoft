
## ------------------------------------------------------------------------------
#
#  [./boost/boost/preprocessor/facilities/CMakeLists.txt]
#
## ------------------------------------------------------------------------------

## ------------------------------------------------------------------------------
## Installation

file (GLOB boost_boost_preprocessor_facilities *.hpp)

install (FILES ${boost_boost_preprocessor_facilities}
  DESTINATION include/boost/preprocessor/facilities
  )
