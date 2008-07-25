
if (HAVE_CASA OR HAVE_CASACORE)

  include_directories (
    ${CASA_INCLUDES}
    ${WCSLIB_INCLUDES}
    ${CFITSIO_INCLUDES}
    )
  
  add_executable (stitch stitch.cc)

  target_link_libraries (stitch
    ${CASA_LIBRARIES}
    ${CFITSIO_LIBRARIES}
    ${WCSLIB_LIBRARIES}
    ${HDF5_LIBRARIES}
    ${LAPACK_LIBRARIES}
    ${BLAS_LIBRARIES}
    ${G2C_LIBRARIES}
    )

endif (HAVE_CASA OR HAVE_CASACORE)
