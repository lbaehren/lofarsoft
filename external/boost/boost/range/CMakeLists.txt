
## ------------------------------------------------------------------------------
#
#  [./boost/boost/range/CMakeLists.txt]
#
## ------------------------------------------------------------------------------

## ------------------------------------------------------------------------------
## Sub-directories

add_subdirectory (detail)

## ------------------------------------------------------------------------------
## Installation

file (GLOB boost_boost_range *.hpp)

install (FILES ${boost_boost_range}
  DESTINATION include/boost/range
  )
