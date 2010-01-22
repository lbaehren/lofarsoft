## ==============================================================================
##
##  SIGPROC/fortran Project settings
##
## ==============================================================================

#if (G77_COMPILER)
#  set (ENV{FC} g77)
#endif (G77_COMPILER)

#SET (CMAKE_Fortran_COMPILER g77)
#set (ENV{FC} g77)

enable_language(Fortran)

SET (CMAKE_Fortran_COMPILER g77)
set (ENV{FC} g77)
#unset (CMAKE_OSX_DEPLOYMENT_TARGET)
unset (COMPILE_FLAGS)
set (CMAKE_OSX_DEPLOYMENT_TARGET "")

include_directories (${PULSAR_SOURCE_DIR}/apps/sigproc)

file (GLOB fort_sigproc_sources baseline.f
best.f
clock.f
dosearch.f
#epnhdr.f
epnpack.f
ffft.f
fftdata.f
formspec.f
freq.f
getddis.f
getmjd.f
getrmea.f
getrrms.f
glun.f
grey.f
histmax.f
indexxf77.f
length.f
mask.f
minmax.f
postproc.f
#quickplot.f
quikgray.f
rdfbtab.f
rdhead.f
readdat.f
readspec.f
readsus.f
rebin.f
resample.f
rwepn.f
seek.f
seekin.f
short.f
sigproc.pdf
singlepulse.f
slfit.f
spcsnr.f
sprof.f
step.f
sumhrm.f
tbary.f
timer.f
tssb.f
writeepn.f
writespec.f
zap_birdies.f
zapit.f
)

add_library (sigproc_fort STATIC ${fort_sigproc_sources})

add_executable (sigproc_seek
  seek.f
  )

add_executable (best
  best.f
  )

add_executable (step
  step.f
  )

add_executable (postproc
  postproc.f
  )

add_executable (grey
  grey.f
  )

add_executable (mask
  mask.f
  )

add_executable (quickplot
  quickplot.f
  )

set (sigproc_fc1_binaries
   best step postproc grey mask quickplot
)

set (sigproc_fc2_binaries
   sigproc_seek
)

# sigproc_seek

if (UNIX)
## if (APPLE)
   set_target_properties (sigproc_fort
     PROPERTIES
     SUFFIX .a
     COMPILE_FLAGS "-ffixed-line-length-none -fbackslash -fno-common"
     LINK_FLAGS "-static"
     CMAKE_SHARED_LIBRARY_LINK_C_FLAGS ""
     )
## else (APPLE)
##   set_target_properties (sigproc_fort
##     PROPERTIES
##     SUFFIX .a
##     COMPILE_FLAGS ""
##     LINK_FLAGS ""
##     )
## endif (APPLE)
endif (UNIX)

##set_target_properties (sigproc_fort
##  PROPERTIES
##  COMPILE_FLAGS "-fPIC -g -O2 -fno-second-underscore"
##  LINK_FLAGS "-fno-second-underscore"
##  )

foreach (src ${sigproc_fc1_binaries})
  set_target_properties (${src}
    PROPERTIES
#    CMAKE_Fortran_COMPILE_OBJECT g77
#    CMAKE_Fortran_LINK_EXECUTABLE g77
    COMPILE_FLAGS "-g -O2 -DPSRFITS -DFFTW -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -fno-common"
    LINK_FLAGS ""
    LINKER_LANGUAGE Fortran
    CMAKE_SHARED_LIBRARY_LINK_C_FLAGS ""
    CMAKE_SHARED_LINKER_FLAGS ""
  )
#  target_link_libraries (${src} sigproc_fort sigproc ${CMAKE_INSTALL_PREFIX}/lib/libfftw3f.a ${CFITSIO_LIBRARIES} ${libm})
  target_link_libraries (${src} sigproc_fort ${CMAKE_INSTALL_PREFIX}/lib/libfftw3f.a ${CFITSIO_LIBRARIES} ${PGPLOT_LIBRARIES} ${libm})
endforeach (src)

foreach (src ${sigproc_fc2_binaries})
  set_target_properties (${src}
    PROPERTIES
#    CMAKE_Fortran_COMPILE_OBJECT g77
#    CMAKE_Fortran_LINK_EXECUTABLE g77
    COMPILE_FLAGS "-g -O2 -DPSRFITS -DFFTW -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -fno-common"
    LINK_FLAGS ""
    LINKER_LANGUAGE Fortran
    CMAKE_SHARED_LIBRARY_LINK_C_FLAGS ""
    CMAKE_SHARED_LINKER_FLAGS ""
  )
  target_link_libraries (${src} sigproc_fort sigproc ${CMAKE_INSTALL_PREFIX}/lib/libfftw3f.a ${CFITSIO_LIBRARIES} ${libm})
#  target_link_libraries (${src} sigproc_fort ${CMAKE_INSTALL_PREFIX}/lib/libfftw3f.a ${CFITSIO_LIBRARIES} ${PGPLOT_LIBRARIES} ${libm})
endforeach (src)


#set (CMAKE_Fortran_COMPILE_OBJECT g77)
#set (CMAKE_Fortran_LINK_EXECUTABLE g77)

## ==============================================================================
##
##  Install of library and executables
##
## ==============================================================================

install (
  TARGETS sigproc_fort 
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )
  
  install (
  TARGETS ${sigproc_fc1_binaries} ${sigproc_fc2_binaries} 
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )

