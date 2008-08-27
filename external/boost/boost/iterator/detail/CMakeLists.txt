
## ------------------------------------------------------------------------------
#
#  [./boost/boost/iterator/detail/CMakeLists.txt]
#
## ------------------------------------------------------------------------------

## ------------------------------------------------------------------------------
## Installation

file (GLOB boost_boost_iterator_detail *.hpp)

install (FILES ${boost_boost_iterator_detail}
  DESTINATION include/boost/iterator/detail
  )
