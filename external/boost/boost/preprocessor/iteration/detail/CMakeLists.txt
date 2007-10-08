
## ------------------------------------------------------------------------------
#
#  [./boost/boost/preprocessor/iteration/detail/CMakeLists.txt]
#
## ------------------------------------------------------------------------------

add_subdirectory (bounds)
add_subdirectory (iter)

## ------------------------------------------------------------------------------
## Installation

file (GLOB boost_boost_preprocessor_iteration_detail *.hpp)

install (FILES ${boost_boost_preprocessor_iteration_detail}
  DESTINATION include/boost/preprocessor/iteration/detail
  )
