
## ------------------------------------------------------------------------------
#
#  [./boost/boost/preprocessor/iteration/detail/bounds/CMakeLists.txt]
#
## ------------------------------------------------------------------------------

## ------------------------------------------------------------------------------
## Installation

file (GLOB boost_boost_preprocessor_iteration_detail_bounds *.hpp)

install (FILES ${boost_boost_preprocessor_iteration_detail_bounds}
  DESTINATION include/boost/preprocessor/iteration/detail/bounds
  )
