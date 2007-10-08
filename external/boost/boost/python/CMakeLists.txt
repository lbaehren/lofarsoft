
## ------------------------------------------------------------------------------
#
#  [./boost/boost/python/CMakeLists.txt]
#
## ------------------------------------------------------------------------------

## ------------------------------------------------------------------------------
## Sub-directories

add_subdirectory (converter)
add_subdirectory (detail)
add_subdirectory (object)
add_subdirectory (suite)

## ------------------------------------------------------------------------------
## Installation

file (GLOB boost_boost_python *.hpp)

install (FILES ${boost_boost_python}
  DESTINATION include/boost/python
  )
