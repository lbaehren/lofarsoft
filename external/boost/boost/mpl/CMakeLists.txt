
## ------------------------------------------------------------------------------
#
#  [./boost/boost/mpl/CMakeLists.txt]
#
## ------------------------------------------------------------------------------

## ------------------------------------------------------------------------------
## Sub-directories

add_subdirectory (aux_)
add_subdirectory (limits)
add_subdirectory (vector)

## ------------------------------------------------------------------------------
## Installation

file (GLOB boost_boost_mpl *.hpp)

install (FILES ${boost_boost_mpl}
  DESTINATION include/boost/mpl
  )
