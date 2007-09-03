
##-------------------------------------------------------------------------------
## $Id:: CMakeLists.txt 575 2007-08-17 14:04:03Z baehren                        $
##-------------------------------------------------------------------------------

## imagefromfits  WCSLIB MIRLIB LAPACK F77 CFITSIO

#if (HAVE_CASACORE OR HAVE_CASA)
#  if (HAVE_CFITSIO)
#    add_executable (imagefromfits imagefromfits.cc)
#    target_link_libraries (imagefromfits
#      ${CASA_LIBRARIES}
#      ${CFITSIO_LIBRARIES}
#      ${LAPACK_LIBRARIES}
#      ${CPGPLOT_LIBRARIES}
#      )
#  endif (HAVE_CFITSIO)
#endif (HAVE_CASACORE OR HAVE_CASA)
  
## treetest  GLISH CFITSIO WCSLIB

add_executable (treetest treetest.cc)
target_link_libraries (treetest cr)
install (TARGETS treetest RUNTIME DESTINATION bin)

## trigrun   GLISH POPT LAPACK F77 CFITSIO WCSLIB

if (HAVE_POPT)
  add_executable (trigrun trigrun.cc)
  target_link_libraries (trigrun cr ${POPT_LIBRARIES})
  install (TARGETS trigrun RUNTIME DESTINATION bin)
endif (HAVE_POPT)

## ------------------------------------------------------------------------------
## Glish client programs

if (HAVE_GLISH)
  add_subdirectory (GlishClients)
endif (HAVE_GLISH)
