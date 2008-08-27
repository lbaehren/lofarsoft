
## ------------------------------------------------------------------------------
#
#  [./boost/boost/mpl/vector/aux_/CMakeLists.txt]
#
## ------------------------------------------------------------------------------

## ------------------------------------------------------------------------------
## Sub-directories

add_subdirectory (preprocessed)

## ------------------------------------------------------------------------------
## Installation

file (GLOB boost_boost_mpl_vector_aux *.hpp)

install (FILES ${boost_boost_mpl_vector_aux}
  DESTINATION include/boost/mpl/vector/aux_
  )
