
## ------------------------------------------------------------------------------
#
#  [./boost/boost/preprocessor/control/detail/CMakecontrols.txt]
#
## ------------------------------------------------------------------------------

## ------------------------------------------------------------------------------
## Installation

file (GLOB boost_boost_preprocessor_control_detail *.hpp)

install (FILES ${boost_boost_preprocessor_control_detail}
  DESTINATION include/boost/preprocessor/control/detail
  )
