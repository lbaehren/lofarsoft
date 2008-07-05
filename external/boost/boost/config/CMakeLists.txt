
## ------------------------------------------------------------------------------
#
#  [./boost/boost/config/CMakeLists.txt]
#
## ------------------------------------------------------------------------------

## ------------------------------------------------------------------------------
## Sub-directories

add_subdirectory (compiler)
add_subdirectory (no_tr1)
add_subdirectory (platform)
add_subdirectory (stdlib)

## ------------------------------------------------------------------------------
## Installation

file (GLOB boost_boost_config *.hpp)

install (FILES ${boost_boost_config}
  DESTINATION include/boost/config
  )
