
## ------------------------------------------------------------------------------
## Check for required external components

find_path (HAVE_BZLIB_H bzlib.h
  PATHS ${include_locations}
  )

## ------------------------------------------------------------------------------
## Sub-directories

add_subdirectory (src)

if (BOOST_ENABLE_TESTING)
  add_subdirectory (test)
endif (BOOST_ENABLE_TESTING)