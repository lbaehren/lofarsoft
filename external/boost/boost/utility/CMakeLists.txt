
## ------------------------------------------------------------------------------
#
#  [./boost/boost/utility/CMakeLists.txt]
#
## ------------------------------------------------------------------------------

## ------------------------------------------------------------------------------
## Sub-directories

add_subdirectory (detail)

## ------------------------------------------------------------------------------
## Installation

file (GLOB boost_boost_utility *.hpp)

install (FILES ${boost_boost_utility}
  DESTINATION include/boost/utility
  )
