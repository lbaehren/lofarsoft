
## ------------------------------------------------------------------------------
#
#  [./boost/boost/exception/CMakeLists.txt]
#
## ------------------------------------------------------------------------------

## ------------------------------------------------------------------------------
## Sub-directories

add_subdirectory (detail)

## ------------------------------------------------------------------------------
## Installation

file (GLOB boost_boost_exception *.hpp)

install (FILES ${boost_boost_exception}
  DESTINATION include/boost/exception
  )
