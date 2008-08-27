
## ------------------------------------------------------------------------------
#
#  [./boost/boost/date_time/posix_time/CMakeLists.txt]
#
## ------------------------------------------------------------------------------

## ------------------------------------------------------------------------------
## Installation

file (GLOB boost_boost_date_time_posix_time *.hpp)

install (FILES ${boost_boost_date_time_posix_time}
  DESTINATION include/boost/date_time/posix_time
  )
