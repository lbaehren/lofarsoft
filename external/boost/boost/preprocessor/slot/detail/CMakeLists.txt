
## ------------------------------------------------------------------------------
#
#  [./boost/boost/preprocessor/slot/detail/CMakeLists.txt]
#
## ------------------------------------------------------------------------------

## ------------------------------------------------------------------------------
## Installation

file (GLOB boost_boost_preprocessor_slot_detail *.hpp)

install (FILES ${boost_boost_preprocessor_slot_detail}
  DESTINATION include/boost/preprocessor/slot/detail
  )
