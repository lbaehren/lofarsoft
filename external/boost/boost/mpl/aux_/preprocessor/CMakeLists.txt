
## ------------------------------------------------------------------------------
#
#  [./boost/boost/mpl/aux_/preprocessor/CMakeLists.txt]
#
## ------------------------------------------------------------------------------

## ------------------------------------------------------------------------------
## Sub-directories

#add_subdirectory (preprocessor)

## ------------------------------------------------------------------------------
## Installation

file (GLOB boost_boost_mpl_aux_preprocessor *.hpp)

install (FILES ${boost_boost_mpl_aux_preprocessor}
  DESTINATION include/boost/mpl/aux_/preprocessor
  )
