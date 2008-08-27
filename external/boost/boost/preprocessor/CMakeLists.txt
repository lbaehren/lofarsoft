
## ------------------------------------------------------------------------------
#
#  [./boost/boost/preprocessor/CMakeLists.txt]
#
## ------------------------------------------------------------------------------

## ------------------------------------------------------------------------------
## Sub-directories

add_subdirectory (arithmetic)
add_subdirectory (array)
add_subdirectory (config)
add_subdirectory (control)
add_subdirectory (debug)
add_subdirectory (detail)
add_subdirectory (facilities)
add_subdirectory (iteration)
add_subdirectory (list)
add_subdirectory (logical)
add_subdirectory (punctuation)
add_subdirectory (repetition)
add_subdirectory (seq)
add_subdirectory (slot)
add_subdirectory (tuple)

## ------------------------------------------------------------------------------
## Installation

file (GLOB boost_boost_preprocessor *.hpp)

install (FILES ${boost_boost_preprocessor}
  DESTINATION include/boost/preprocessor
  )
