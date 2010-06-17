
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

configure_file (${BDSM_SOURCE_DIR}/implement/create.in ${BDSM_BINARY_DIR}/implement/bdsm/create)

## Fortran-based version

if (BDSM_WITH_FBDSM)
  add_subdirectory (bdsm)
endif (BDSM_WITH_FBDSM)

## Python-based version

if (BDSM_WITH_PYBDSM)
  add_subdirectory (minpack)
  add_subdirectory (port3)
  add_subdirectory (pybdsm)
endif (BDSM_WITH_PYBDSM)
