
## ------------------------------------------------------------------------------
#
#  [./boost/boost/preprocessor/seq/CMakeLists.txt]
#
## ------------------------------------------------------------------------------

add_subdirectory (detail)

## ------------------------------------------------------------------------------
## Installation

file (GLOB boost_boost_preprocessor_seq *.hpp)

install (FILES ${boost_boost_preprocessor_seq}
  DESTINATION include/boost/preprocessor/seq
  )
