
## ------------------------------------------------------------------------------
#
#  [./boost/boost/algorithm/string/CMakeLists.txt]
#
## ------------------------------------------------------------------------------

## ------------------------------------------------------------------------------
## Sub-directories

add_subdirectory (detail)
add_subdirectory (std)

## ------------------------------------------------------------------------------
## Installation

file (GLOB boost_boost_algorithm_string *.hpp)

install (FILES ${boost_boost_algorithm_string}
  DESTINATION include/boost/algorithm/string
  )
