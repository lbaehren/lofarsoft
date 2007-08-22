
##-------------------------------------------------------------------------------
## $Id:: CMakeLists.txt 575 2007-08-17 14:04:03Z baehren                        $
##-------------------------------------------------------------------------------

# treetest  GLISH CFITSIO WCSLIB

## trigrun   GLISH POPT LAPACK F77 CFITSIO WCSLIB

add_executable (trigrun trigrun.cc)
target_link_libraries (trigrun cr)

## ------------------------------------------------------------------------------
## Glish client programs

if (HAVE_GLISH)
  add_subdirectory (GlishClients)
endif (HAVE_GLISH)

# CGaccessClient  PGPLOT X11 WCSLIB LAPACK F77 GLISH CFITSIO
# CGuiPipeClient  PGPLOT X11 WCSLIB LAPACK F77 GLISH CFITSIO
# CalTableClient  WCSLIB LAPACK F77 GLISH CFITSIO

