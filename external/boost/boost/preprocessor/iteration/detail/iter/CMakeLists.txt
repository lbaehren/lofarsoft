
## ------------------------------------------------------------------------------
#
#  [./boost/boost/preprocessor/iteration/detail/iter/CMakeLists.txt]
#
## ------------------------------------------------------------------------------

## ------------------------------------------------------------------------------
## Installation

file (GLOB boost_boost_preprocessor_iteration_detail_iter *.hpp)

install (FILES ${boost_boost_preprocessor_iteration_detail_iter}
  DESTINATION include/boost/preprocessor/iteration/detail/iter
  )
