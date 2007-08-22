
##-------------------------------------------------------------------------------
## $Id:: CMakeLists.txt 575 2007-08-17 14:04:03Z baehren                        $
##-------------------------------------------------------------------------------

## ppsync          GLISH
## CGaccessClient  PGPLOT X11 WCSLIB LAPACK F77 GLISH CFITSIO
## CGuiPipeClient  PGPLOT X11 WCSLIB LAPACK F77 GLISH CFITSIO
## CalTableClient  WCSLIB LAPACK F77 GLISH CFITSIO
## readdat         WCSLIB LAPACK F77 GLISH CFITSIO

add_executable (ppsync ppsync.cc)
target_link_libraries (ppsync cr ${GLISH_LIBRARIES})
  
add_executable (readdat readdat.cc)
target_link_libraries (readdat cr ${GLISH_LIBRARIES})
  
set (glish_clients
  CGaccessClient
  CGuiPipeClient
  )

foreach (client ${glish_clients})
  ## compiler settings
  add_executable (${client} ${client}.cc)
  ## linker settings
  target_link_libraries (${client}
    cr
    ${GLISH_LIBRARIES}
    ${PGPLOT_LIBRARIES}
    ${X11_LIBRARIES}
    )
endforeach (client)
