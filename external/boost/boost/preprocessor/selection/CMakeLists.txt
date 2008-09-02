
## ------------------------------------------------------------------------------
#
#  [./boost/boost/preprocessor/selection/CMakeLists.txt]
#
## ------------------------------------------------------------------------------

## ------------------------------------------------------------------------------
## Installation

file (GLOB boost_boost_preprocessor_selection *.hpp)

install (FILES ${boost_boost_preprocessor_selection}
  DESTINATION include/boost/preprocessor/selection
  )
