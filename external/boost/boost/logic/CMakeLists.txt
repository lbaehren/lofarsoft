
## ------------------------------------------------------------------------------
#
#  [./boost/boost/logic/CMakeLists.txt]
#
## ------------------------------------------------------------------------------

## ------------------------------------------------------------------------------
## Installation

file (GLOB boost_boost_logic *.hpp)

install (FILES ${boost_boost_logic}
  DESTINATION include/boost/logic
  )
