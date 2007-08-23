
##-------------------------------------------------------------------------------
## $Id                                                                          $
##-------------------------------------------------------------------------------

##-------------------------------------------------------------------------------
## Clients which need libglish only

# antennaflagger    GLISH LAPACK F77 GLISH CFITSIO WCSLIB
# antennagaincurve  GLISH LAPACK F77 CFITSIO WCSLIB
# ccData2Gaincurve  GLISH LAPACK F77 CFITSIO WCSLIB
# dstb              LAPACK F77 GLISH CFITSIO WCSLIB
# dynamicspectrum   WCSLIB LAPACK F77 GLISH CFITSIO
# eventsaveclient   GLISH CFITSIO WCSLIB
# fits2wf           GLISH CFITSIO WCSLIB
# glempf            GLISH CFITSIO WCSLIB
# glsender          GLISH CFITSIO WCSLIB
# lopestools        GLISH CFITSIO WCSLIB LAPACK F77
# moviemaker        GLISH
# ppsync            GLISH
# readdat           WCSLIB LAPACK F77 GLISH CFITSIO
# skymapper         WCSLIB LAPACK F77 GLISH CFITSIO
# timclient         GLISH
# timebeam          WCSLIB LAPACK F77 GLISH CFITSIO
# CalTableClient    WCSLIB LAPACK F77 GLISH CFITSIO

set (glish_clients
  antennaflagger
  antennagaincurve
  ccData2Gaincurve
  dstb
  dynamicspectrum
  eventsaveclient
  fits2wf
  glempf
  glsender
  lopestools
  moviemaker
  ppsync
  readdat
  skymapper
  timclient
  timebeam
  CalTableClient
)

foreach (client ${glish_clients})
  ## compiler instructions
  add_executable (${client} ${client}.cc)
  ## linker instructions
  target_link_libraries (${client} cr ${GLISH_LIBRARIES})
endforeach (client)

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
