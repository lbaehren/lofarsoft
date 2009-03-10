
## ------------------------------------------------------------------------------
## Sub-directories

add_subdirectory (src)

if (BOOST_ENABLE_TESTING)
  add_subdirectory (example)
  add_subdirectory (test)
endif (BOOST_ENABLE_TESTING)
