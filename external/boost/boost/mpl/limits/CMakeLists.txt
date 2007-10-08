
## ------------------------------------------------------------------------------
#
#  [./boost/boost/mpl/limits/CMakeLists.txt]
#
## ------------------------------------------------------------------------------

## ------------------------------------------------------------------------------
## Installation

file (GLOB boost_boost_mpl_limits *.hpp)

install (FILES ${boost_boost_mpl_limits}
  DESTINATION include/boost/mpl/limits
  )
