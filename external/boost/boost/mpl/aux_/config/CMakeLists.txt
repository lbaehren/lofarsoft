
## ------------------------------------------------------------------------------
#
#  [./boost/boost/mpl/aux_/config/CMakeLists.txt]
#
## ------------------------------------------------------------------------------

## ------------------------------------------------------------------------------
## Sub-directories

#add_subdirectory (config)

## ------------------------------------------------------------------------------
## Installation

file (GLOB boost_boost_mpl_aux_config *.hpp)

install (FILES ${boost_boost_mpl_aux_config}
  DESTINATION include/boost/mpl/aux_/config
  )
