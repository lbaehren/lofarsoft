
## ------------------------------------------------------------------------------
#
#  [./boost/boost/preprocessor/control/CMakeLists.txt]
#
## ------------------------------------------------------------------------------

add_subdirectory (detail)

## ------------------------------------------------------------------------------
## Installation

file (GLOB boost_boost_preprocessor_control *.hpp)

install (FILES ${boost_boost_preprocessor_control}
  DESTINATION include/boost/preprocessor/control
  )
