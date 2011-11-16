
## =============================================================================
##
##  Build instructions
##
## =============================================================================

add_executable (smosaic smosaic.cc)

target_link_libraries (smosaic
  ${CASACORE_LIBRARIES}
  ${HDF5_LIBRARIES}
  ${CFITSIO_LIBRARIES}
  ${WCSLIB_LIBRARIES}
  ${LAPACK_LIBRARIES}
  )
