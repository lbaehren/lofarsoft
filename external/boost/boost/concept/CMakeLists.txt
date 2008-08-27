
## ------------------------------------------------------------------------------
#
#  [./boost/boost/concept/CMakeLists.txt]
#
## ------------------------------------------------------------------------------

## ------------------------------------------------------------------------------
## Sub-directories

add_subdirectory (detail)

## ------------------------------------------------------------------------------
## Installation

file (GLOB boost_boost_concept *.hpp)

install (FILES ${boost_boost_concept}
  DESTINATION include/boost/concept
  )
