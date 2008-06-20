
## ------------------------------------------------------------------------------
#
#  [./boost/boost/algorithm/CMakeLists.txt]
#
## ------------------------------------------------------------------------------

## ------------------------------------------------------------------------------
## Sub-directories

add_subdirectory (string)

## ------------------------------------------------------------------------------
## Installation

file (GLOB boost_boost_algorithm *.hpp)

install (FILES ${boost_boost_algorithm}
  DESTINATION include/boost/algorithm
  )
