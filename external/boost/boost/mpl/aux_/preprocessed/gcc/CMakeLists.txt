
## ------------------------------------------------------------------------------
#
#  [./boost/boost/mpl/aux_/preprocessed/gcc/CMakeLists.txt]
#
## ------------------------------------------------------------------------------

## ------------------------------------------------------------------------------
## Sub-directories

#add_subdirectory (gcc)

## ------------------------------------------------------------------------------
## Installation

file (GLOB boost_boost_mpl_aux_preprocessed_gcc *.hpp)

install (FILES ${boost_boost_mpl_aux_preprocessed_gcc}
  DESTINATION include/boost/mpl/aux_/preprocessed/gcc
  )
