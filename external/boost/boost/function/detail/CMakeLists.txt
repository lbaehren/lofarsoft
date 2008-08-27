
## ------------------------------------------------------------------------------
#
#  [./boost/boost/function/detail/CMakeLists.txt]
#
## ------------------------------------------------------------------------------

## ------------------------------------------------------------------------------
## Installation

file (GLOB boost_boost_function_detail *.hpp)

install (FILES ${boost_boost_function_detail}
  DESTINATION include/boost/function/detail
  )
