
## ------------------------------------------------------------------------------
#
#  [./boost/boost/program_options/detail/CMakeLists.txt]
#
## ------------------------------------------------------------------------------

## ------------------------------------------------------------------------------
## Installation

file (GLOB boost_boost_program_options_detail *.hpp)

install (FILES ${boost_boost_program_options_detail}
  DESTINATION include/boost/program_options/detail
  )
