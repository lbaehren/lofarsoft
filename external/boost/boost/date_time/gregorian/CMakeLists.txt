
## ------------------------------------------------------------------------------
#
#  [./boost/boost/date_time/gregorian/CMakeLists.txt]
#
## ------------------------------------------------------------------------------

## ------------------------------------------------------------------------------
## Installation

file (GLOB boost_boost_date_time_gregorian *.hpp)

install (FILES ${boost_boost_date_time_gregorian}
  DESTINATION include/boost/date_time/gregorian
  )
