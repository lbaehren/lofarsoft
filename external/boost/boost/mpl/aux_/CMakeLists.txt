
## ------------------------------------------------------------------------------
#
#  [./boost/boost/mpl/aux_/CMakeLists.txt]
#
## ------------------------------------------------------------------------------

## ------------------------------------------------------------------------------
## Sub-directories

add_subdirectory (config)
add_subdirectory (preprocessed)
add_subdirectory (preprocessor)

## ------------------------------------------------------------------------------
## Installation

file (GLOB boost_boost_mpl_aux *.hpp)

install (FILES ${boost_boost_mpl_aux}
  DESTINATION include/boost/mpl/aux_
  )
