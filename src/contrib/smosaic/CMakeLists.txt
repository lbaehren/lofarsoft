
## =============================================================================
##
##  Build instructions
##
## =============================================================================

include_directories (
  ${CASA_INCLUDES}
  ${CFITSIO_INCLUDES}
  ${HDF5_INCLUDES}
  ${WCSLIB_INCLUDES}
  )

add_executable (smosaic smosaic.cc)

target_link_libraries (smosaic
  ${CASACORE_LIBRARIES}
  ${HDF5_LIBRARIES}
  ${CFITSIO_LIBRARIES}
  ${WCSLIB_LIBRARIES}
  ${LAPACK_LIBRARIES}
  )
