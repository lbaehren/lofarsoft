
## ------------------------------------------------------------------------------
#
#  [./boost/boost/concept_check/CMakeLists.txt]
#
## ------------------------------------------------------------------------------

## ------------------------------------------------------------------------------
## Installation

file (GLOB boost_boost_concept_check *.hpp)

install (FILES ${boost_boost_concept_check}
  DESTINATION include/boost/concept_check
  )
