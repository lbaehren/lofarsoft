
## ------------------------------------------------------------------------------
#
#  [./boost/boost/date_time/CMakeLists.txt]
#
## ------------------------------------------------------------------------------

## ------------------------------------------------------------------------------
## Sub-directories

add_subdirectory (gregorian)
add_subdirectory (local_time)
add_subdirectory (posix_time)

## ------------------------------------------------------------------------------
## Installation

file (GLOB boost_boost_date_time *.hpp *.ipp)

install (FILES ${boost_boost_date_time}
  DESTINATION include/boost/date_time
  )
