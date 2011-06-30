# $Id$

include_directories (
  ${BOOST_INCLUDES}
  ${PYTHON_INCLUDES}
  ${NUMPY_INCLUDES}
  ${NUM_UTIL_INCLUDES}
  ${NUM_UTIL_INCLUDES}/..
  )

## Fortran-based version
if (ANAAMIKA_ENABLE_FBDSM)
  add_subdirectory (fBDSM)
endif (ANAAMIKA_ENABLE_FBDSM)

## Python-based version
if (ANAAMIKA_ENABLE_PYBDSM)
  add_subdirectory (PyBDSM)
endif (ANAAMIKA_ENABLE_PYBDSM)
