
## ------------------------------------------------------------------------------
#
#  [./boost/boost/preprocessor/list/CMakeLists.txt]
#
## ------------------------------------------------------------------------------

add_subdirectory (detail)

## ------------------------------------------------------------------------------
## Installation

file (GLOB boost_boost_preprocessor_list *.hpp)

install (FILES ${boost_boost_preprocessor_list}
  DESTINATION include/boost/preprocessor/list
  )
