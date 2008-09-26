

## ------------------------------------------------------------------------------
## Project subdirectories

add_subdirectory (src)

if (NUMPY_ENABLE_TESTING)
  add_subdirectory (tests)
endif (NUMPY_ENABLE_TESTING)
