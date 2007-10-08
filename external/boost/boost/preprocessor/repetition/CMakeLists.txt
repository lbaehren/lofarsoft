
## ------------------------------------------------------------------------------
#
#  [./boost/boost/preprocessor/repetition/CMakeLists.txt]
#
## ------------------------------------------------------------------------------

add_subdirectory (detail)

## ------------------------------------------------------------------------------
## Installation

file (GLOB boost_boost_preprocessor_repetition *.hpp)

install (FILES ${boost_boost_preprocessor_repetition}
  DESTINATION include/boost/preprocessor/repetition
  )
