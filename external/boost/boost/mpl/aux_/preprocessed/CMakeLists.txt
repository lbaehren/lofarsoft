
## ------------------------------------------------------------------------------
#
#  [./boost/boost/mpl/aux_/preprocessed/CMakeLists.txt]
#
## ------------------------------------------------------------------------------

## ------------------------------------------------------------------------------
## Sub-directories

add_subdirectory (gcc)

## ------------------------------------------------------------------------------
## Installation

file (GLOB boost_boost_mpl_aux_preprocessed *.hpp)

install (FILES ${boost_boost_mpl_aux_preprocessed}
  DESTINATION include/boost/mpl/aux_/preprocessed
  )
