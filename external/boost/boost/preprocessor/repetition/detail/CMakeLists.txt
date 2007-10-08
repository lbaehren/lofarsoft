
## ------------------------------------------------------------------------------
#
#  [./boost/boost/preprocessor/repetition/detail/CMakeLists.txt]
#
## ------------------------------------------------------------------------------

add_subdirectory (dmc)
add_subdirectory (edg)
add_subdirectory (msvc)

## ------------------------------------------------------------------------------
## Installation

file (GLOB boost_boost_preprocessor_repetition_detail *.hpp)

install (FILES ${boost_boost_preprocessor_repetition_detail}
  DESTINATION include/boost/preprocessor/repetition/detail
  )
