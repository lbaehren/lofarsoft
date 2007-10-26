
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

if (LAPACK_LIBRARIES AND G2C_LIBRARIES)
  add_executable (treetest treetest.cc)
  target_link_libraries (treetest cr)
  install (TARGETS treetest RUNTIME DESTINATION bin)
else (LAPACK_LIBRARIES AND G2C_LIBRARIES)
  message (STATUS "[cr] Unable to build executable treetest.")
endif (LAPACK_LIBRARIES AND G2C_LIBRARIES)

## trigrun   GLISH POPT LAPACK F77 CFITSIO WCSLIB

if (HAVE_POPT AND LAPACK_LIBRARIES)
  ## compiler instructions
  add_executable (trigrun trigrun.cc)
  ## linker instructions
  target_link_libraries (trigrun
    cr
    ${POPT_LIBRARIES}
    )
#  set_target_properties (trigrun
#    PROPERTIES
#    COMPILE_FLAGS -fPIC -O2 -fPIC
#    )
  ## installation instructions
  install (TARGETS trigrun RUNTIME DESTINATION bin)
endif (HAVE_POPT AND LAPACK_LIBRARIES)

## simStationBeam

if (HAVE_CASA)
  add_executable (simStationBeam simStationBeam.cc)
  ## linker instructions
  target_link_libraries (simStationBeam cr)
  ## installation instructions
  install (TARGETS simStationBeam RUNTIME DESTINATION bin)
endif (HAVE_CASA)

## ------------------------------------------------------------------------------
## Glish client programs

if (CR_GLISH_SUPPORT AND HAVE_GLISH)
  add_subdirectory (GlishClients)
endif (CR_GLISH_SUPPORT AND HAVE_GLISH)

## ------------------------------------------------------------------------------
## LOPES-Star tools

if (HAVE_ROOT AND HAVE_FFTW3)
  add_subdirectory (LopesStar)
endif (HAVE_ROOT AND HAVE_FFTW3)
