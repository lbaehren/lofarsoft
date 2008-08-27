
## ------------------------------------------------------------------------------
#
#  [./boost/boost/numeric/CMakeLists.txt]
#
## ------------------------------------------------------------------------------

## ------------------------------------------------------------------------------
## Sub-directories

add_subdirectory (conversion)
add_subdirectory (interval)
add_subdirectory (ublas)

## ------------------------------------------------------------------------------
## Installation

file (GLOB boost_boost_numeric *.hpp)

install (FILES ${boost_boost_numeric}
  DESTINATION include/boost/numeric
  )
