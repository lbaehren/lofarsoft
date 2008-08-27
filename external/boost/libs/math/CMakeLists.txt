
## ------------------------------------------------------------------------------
## Sub-directories

add_subdirectory (octonion)
add_subdirectory (quaternion)
add_subdirectory (special_functions)
add_subdirectory (tools)

if (BOOST_ENABLE_TESTING)
  add_subdirectory (test)
endif (BOOST_ENABLE_TESTING)