
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

add_custom_target (create
  COMMAND ./create
  WORKING_DIRECTORY ${BDSM_BINARY_DIR}/implement/bdsm
  )

## Fortran-based version

if (BDSM_WITH_FBDSM)
  add_subdirectory (fbdsm)
endif (BDSM_WITH_FBDSM)

## Python-based version

if (BDSM_WITH_PYBDSM)
  add_subdirectory (minpack)
  add_subdirectory (port3)
  add_subdirectory (pybdsm)
endif (BDSM_WITH_PYBDSM)
