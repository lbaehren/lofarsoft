
## ------------------------------------------------------------------------------
#
#  [./boost/boost/preprocessor/punctuation/CMakeLists.txt]
#
## ------------------------------------------------------------------------------

## ------------------------------------------------------------------------------
## Installation

file (GLOB boost_boost_preprocessor_punctuation *.hpp)

install (FILES ${boost_boost_preprocessor_punctuation}
  DESTINATION include/boost/preprocessor/punctuation
  )
