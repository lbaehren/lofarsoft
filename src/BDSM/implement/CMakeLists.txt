
include_directories (
  ${BOOST_INCLUDES}
  ${PYTHON_INCLUDES}
  ${NUMPY_INCLUDES}
  ${NUM_UTIL_INCLUDES}
  ${NUM_UTIL_INCLUDES}/..
  )

## ==============================================================================
##
##  Projet subdirectories
##
## ==============================================================================

add_subdirectory (bdsm)

if (BDSM_WITH_PYBDSM)
  add_subdirectory (minpack)
  add_subdirectory (port3)
  add_subdirectory (pybdsm)
endif (BDSM_WITH_PYBDSM)
