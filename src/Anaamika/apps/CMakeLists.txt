# $Id$

if (ANAAMIKA_ENABLE_FBDSM)
  add_subdirectory (fBDSM)
endif (ANAAMIKA_ENABLE_FBDSM)

if (ANAAMIKA_ENABLE_PYBDSM)
  add_subdirectory (PyBDSM)
endif (ANAAMIKA_ENABLE_PYBDSM)
