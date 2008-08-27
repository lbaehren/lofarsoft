
## ------------------------------------------------------------------------------
#
#  [./boost/boost/tuple/CMakeLists.txt]
#
## ------------------------------------------------------------------------------

## ------------------------------------------------------------------------------
## Sub-directories

add_subdirectory (detail)

## ------------------------------------------------------------------------------
## Installation

file (GLOB boost_boost_tuple *.hpp)

install (FILES ${boost_boost_tuple}
  DESTINATION include/boost/tuple
  )
