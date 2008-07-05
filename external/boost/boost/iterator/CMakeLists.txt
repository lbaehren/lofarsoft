
## ------------------------------------------------------------------------------
#
#  [./boost/boost/iterator/CMakeLists.txt]
#
## ------------------------------------------------------------------------------

## ------------------------------------------------------------------------------
## Sub-directories

add_subdirectory (detail)

## ------------------------------------------------------------------------------
## Installation

file (GLOB boost_boost_iterator *.hpp)

install (FILES ${boost_boost_iterator}
  DESTINATION include/boost/iterator
  )
