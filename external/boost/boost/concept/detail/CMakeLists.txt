
## ------------------------------------------------------------------------------
#
#  [./boost/boost/concept/detail/CMakeLists.txt]
#
## ------------------------------------------------------------------------------

## ------------------------------------------------------------------------------
## Installation

file (GLOB boost_boost_concept_detail *.hpp)

install (FILES ${boost_boost_concept_detail}
  DESTINATION include/boost/concept/detail
  )
