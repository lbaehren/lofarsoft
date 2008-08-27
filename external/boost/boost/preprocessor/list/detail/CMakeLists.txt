
## ------------------------------------------------------------------------------
#
#  [./boost/boost/preprocessor/list/detail/CMakeLists.txt]
#
## ------------------------------------------------------------------------------

## ------------------------------------------------------------------------------
## Installation

file (GLOB boost_boost_preprocessor_list_detail *.hpp)

install (FILES ${boost_boost_preprocessor_list_detail}
  DESTINATION include/boost/preprocessor/list/detail
  )
