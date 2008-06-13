
## ------------------------------------------------------------------------------
#
#  [./boost/boost/CMakeLists.txt]
#
## ------------------------------------------------------------------------------

## ------------------------------------------------------------------------------
## Sub-directories

add_subdirectory (bind)
add_subdirectory (config)
add_subdirectory (detail)
add_subdirectory (function)
add_subdirectory (iterator)
add_subdirectory (mpl)
add_subdirectory (multi_array)
add_subdirectory (numeric)
add_subdirectory (preprocessor)
add_subdirectory (python)
add_subdirectory (type_traits)
add_subdirectory (tuple)
add_subdirectory (utility)

## ------------------------------------------------------------------------------
## Installation

file (GLOB boost_boost *.hpp)

install (FILES ${boost_boost}
  DESTINATION include/boost
  )
