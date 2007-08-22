
##-------------------------------------------------------------------------------
## $Id                                                                          $
##-------------------------------------------------------------------------------

##-------------------------------------------------------------------------------
## Clients which need libglish only

# glempf          GLISH CFITSIO WCSLIB
# glsender        GLISH CFITSIO WCSLIB
# lopestools      GLISH CFITSIO WCSLIB LAPACK F77
# readdat         WCSLIB LAPACK F77 GLISH CFITSIO
# ppsync          GLISH
# CalTableClient  WCSLIB LAPACK F77 GLISH CFITSIO

add_executable (glempf glempf.cc)
target_link_libraries (glempf cr ${GLISH_LIBRARIES})

add_executable (glsender glsender.cc)
target_link_libraries (glsender cr ${GLISH_LIBRARIES})
  
add_executable (lopestools lopestools.cc)
target_link_libraries (lopestools cr ${GLISH_LIBRARIES})
  
add_executable (readdat readdat.cc)
target_link_libraries (readdat cr ${GLISH_LIBRARIES})
  
add_executable (ppsync ppsync.cc)
target_link_libraries (ppsync cr ${GLISH_LIBRARIES})
  
add_executable (CalTableClient CalTableClient.cc)
target_link_libraries (CalTableClient cr ${GLISH_LIBRARIES})
  
##-------------------------------------------------------------------------------
## Clients which need additional external packages (e.g. for plotting)

# CGaccessClient  PGPLOT X11 WCSLIB LAPACK F77 GLISH CFITSIO
# CGuiPipeClient  PGPLOT X11 WCSLIB LAPACK F77 GLISH CFITSIO

add_executable (CGaccessClient CGaccessClient.cc)
target_link_libraries (CGaccessClient
  cr
  ${GLISH_LIBRARIES}
  ${PGPLOT_LIBRARIES}
  ${X11_LIBRARIES}
  )

add_executable (CGuiPipeClient CGuiPipeClient.cc)
target_link_libraries (CGuiPipeClient
  cr
  ${GLISH_LIBRARIES}
  ${PGPLOT_LIBRARIES}
  ${X11_LIBRARIES}
  )

