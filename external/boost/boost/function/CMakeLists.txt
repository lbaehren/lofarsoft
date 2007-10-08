
## ------------------------------------------------------------------------------
#
#  [./boost/boost/function/CMakeLists.txt]
#
## ------------------------------------------------------------------------------

## ------------------------------------------------------------------------------
## Sub-directories

add_subdirectory (detail)

## ------------------------------------------------------------------------------
## Installation

file (GLOB boost_boost_function *.hpp)

install (FILES ${boost_boost_function}
  DESTINATION include/boost/function
  )
