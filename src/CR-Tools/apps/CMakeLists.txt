
##-------------------------------------------------------------------------------
## $Id:: CMakeLists.txt 575 2007-08-17 14:04:03Z baehren                        $
##-------------------------------------------------------------------------------

if (HAVE_CASACORE OR HAVE_CASA)
  add_executable (imagefromfits imagefromfits.cc)
  target_link_libraries (imagefromfits ${CASA_LIBRARIES})
endif (HAVE_CASACORE OR HAVE_CASA)
  
## treetest  GLISH CFITSIO WCSLIB

add_executable (treetest treetest.cc)
target_link_libraries (treetest cr)

## trigrun   GLISH POPT LAPACK F77 CFITSIO WCSLIB

if (HAVE_POPT)
  add_executable (trigrun trigrun.cc)
  target_link_libraries (trigrun cr ${POPT_LIBRARIES})
endif (HAVE_POPT)

## ------------------------------------------------------------------------------
## Glish client programs

if (HAVE_GLISH)
  add_subdirectory (GlishClients)
endif (HAVE_GLISH)

