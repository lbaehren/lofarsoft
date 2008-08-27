
## ------------------------------------------------------------------------------
#
#  [./boost/boost/optional/CMakeLists.txt]
#
## ------------------------------------------------------------------------------

## ------------------------------------------------------------------------------
## Installation

file (GLOB boost_boost_optional *.hpp)

install (FILES ${boost_boost_optional}
  DESTINATION include/boost/optional
  )
