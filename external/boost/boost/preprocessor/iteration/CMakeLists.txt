
## ------------------------------------------------------------------------------
#
#  [./boost/boost/preprocessor/iteration/CMakeLists.txt]
#
## ------------------------------------------------------------------------------

add_subdirectory (detail)

## ------------------------------------------------------------------------------
## Installation

file (GLOB boost_boost_preprocessor_iteration *.hpp)

install (FILES ${boost_boost_preprocessor_iteration}
  DESTINATION include/boost/preprocessor/iteration
  )
