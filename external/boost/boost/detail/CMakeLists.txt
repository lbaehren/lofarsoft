
## ------------------------------------------------------------------------------
#
#  [./boost/boost/detail/CMakeLists.txt]
#
## ------------------------------------------------------------------------------

## ------------------------------------------------------------------------------
## Installation

file (GLOB boost_boost_detail *.hpp)

install (FILES ${boost_boost_detail}
  DESTINATION include/boost/detail
  )
