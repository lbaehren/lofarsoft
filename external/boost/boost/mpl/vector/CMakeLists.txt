
## ------------------------------------------------------------------------------
#
#  [./boost/boost/mpl/vector/CMakeLists.txt]
#
## ------------------------------------------------------------------------------

## ------------------------------------------------------------------------------
## Sub-directories

add_subdirectory (aux_)

## ------------------------------------------------------------------------------
## Installation

file (GLOB boost_boost_mpl_vector *.hpp)

install (FILES ${boost_boost_mpl_vector}
  DESTINATION include/boost/mpl/vector
  )
