
## ------------------------------------------------------------------------------
#
#  [./boost/boost/exception/detail/CMakeLists.txt]
#
## ------------------------------------------------------------------------------

## ------------------------------------------------------------------------------
## Installation

file (GLOB boost_boost_exception_detail *.hpp)

install (FILES ${boost_boost_exception_detail}
  DESTINATION include/boost/exception/detail
  )
