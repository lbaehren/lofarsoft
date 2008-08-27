
## ------------------------------------------------------------------------------
#
#  [./boost/boost/range/detail/CMakeLists.txt]
#
## ------------------------------------------------------------------------------

## ------------------------------------------------------------------------------
## Sub-directories

add_subdirectory (mfc)
add_subdirectory (vc6)

## ------------------------------------------------------------------------------
## Installation

file (GLOB boost_boost_range_detail *.hpp)

install (FILES ${boost_boost_range_detail}
  DESTINATION include/boost/range/detail
  )
