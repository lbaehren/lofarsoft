
## ------------------------------------------------------------------------------
#
#  [./boost/boost/preprocessor/slot/CMakeLists.txt]
#
## ------------------------------------------------------------------------------

add_subdirectory (detail)

## ------------------------------------------------------------------------------
## Installation

file (GLOB boost_boost_preprocessor_slot *.hpp)

install (FILES ${boost_boost_preprocessor_slot}
  DESTINATION include/boost/preprocessor/slot
  )
