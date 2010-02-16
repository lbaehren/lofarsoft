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

####SET (CMAKE_Fortran_COMPILER g77)
###SET (CMAKE_Fortran_COMPILER g95)
####set (ENV{FC} g77)
###set (ENV{FC} g95)
#unset (CMAKE_OSX_DEPLOYMENT_TARGET)
unset (COMPILE_FLAGS)
set (CMAKE_OSX_DEPLOYMENT_TARGET "")

include_directories (${PULSAR_SOURCE_DIR}/apps/sigproc-4.3)

file (GLOB fort_sigproc_sources baseline.f
#best.f
#chaninfo.f
clock.f
cpower.f
dosearch.f
#epnhdr.f
epnpack.f
fast_mean.f
#ffa.f
ffft.f
fftdata.f
fftwdata.f
#fftwdummy.f
find_norm.f
formspec.f
freq.f
getddis.f
getfast_rmea.f
getmeanrms.f
getmjd.f
getrmea.f
getrrms.f
glun.f
#grey.f
histmax.f
indexxf77.f
length.f
#mask.f
meanrms.f
minmax.f
#mjdnow.f
mmzap.f
normal.f
#peak.f
#plotpulses.f
#plotpulses.old.f
#postproc.f
#quickplot.f
quikgray.f
ralphs_mean.f
rdfbtab.f
rdhead.f
readdat.f
readspec.f
readsus.f
rebin.f
recon_prof.f
resample.f
rotate_time.f
rwepn.f
#seek.f
seekin.f
short.f
singlepulse.f
slfit.f
#sort3.f
spcsnr.f
#spec.f
sprof.f
#step.f
#sumfft.f
sumhrm.f
#tbary.f
timer.f
#tssb.f
writeepn.f
writespec.f
zap_birdies.f
zapit.f
)

add_library (sigproc_fort STATIC ${fort_sigproc_sources})

add_executable (sigproc_seek
  seek.f
  )

add_executable (sumfft
  sumfft.f
  )

add_executable (peak
  peak.f
  )

add_executable (ffa
  ffa.f
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

add_executable (chaninfo
  chaninfo.f
  )

add_executable (mask
  mask.f
  )

add_executable (quickplot
  quickplot.f
  )

add_executable (plotpulses
  plotpulses.f
  )
  
set (sigproc_fc1_binaries
    step postproc grey sumfft chaninfo
)

set (sigproc_fc2_binaries
   sigproc_seek
)

set (sigproc_fc3_binaries
   best mask quickplot plotpulses
)

set (sigproc_fc4_binaries
   peak
)

set (sigproc_fc5_binaries
   ffa
)

if (UNIX)
## if (APPLE)
   set_target_properties (sigproc_fort
     PROPERTIES
     SUFFIX .a
#     COMPILE_FLAGS "-ffixed-line-length-132 -freal-loops -fno-common"
#     COMPILE_FLAGS "-D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -fopenmp -O2 -g -DPSRFITS -DFFTW -ffixed-line-length-132"
#     COMPILE_FLAGS "-D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -O2 -g -DPSRFITS -DFFTW -ffixed-line-length-132 -freal-loops"
     COMPILE_FLAGS "-D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -O2 -g -DPSRFITS -DFFTW -ffixed-line-length-132"
     LINK_FLAGS "-static"
###     CMAKE_SHARED_LIBRARY_LINK_C_FLAGS ""
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
#    COMPILE_FLAGS "-g -O2 -DPSRFITS -DFFTW -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -fno-common"
#     COMPILE_FLAGS "-D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -fopenmp -O2 -g -DPSRFITS -DFFTW -ffixed-line-length-132 -freal-loops"
#     COMPILE_FLAGS "-D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -O2 -g -DPSRFITS -DFFTW -ffixed-line-length-132 -freal-loops"
     COMPILE_FLAGS "-D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -O2 -g -DPSRFITS -DFFTW -ffixed-line-length-132"
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
#    COMPILE_FLAGS "-g -O2 -DPSRFITS -DFFTW -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64"
#     COMPILE_FLAGS "-D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -fopenmp -O2 -g -DPSRFITS -DFFTW -ffixed-line-length-132 -freal-loops"
#     COMPILE_FLAGS "-D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -O2 -g -DPSRFITS -DFFTW -ffixed-line-length-132 -freal-loops"
     COMPILE_FLAGS "-D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -O2 -g -DPSRFITS -DFFTW -ffixed-line-length-132"
    LINK_FLAGS ""
    LINKER_LANGUAGE Fortran
###    CMAKE_SHARED_LIBRARY_LINK_C_FLAGS ""
###    CMAKE_SHARED_LINKER_FLAGS ""
  )
  target_link_libraries (${src} sigproc_fort sigproc ${CMAKE_INSTALL_PREFIX}/lib/libfftw3f.a)
endforeach (src)

foreach (src ${sigproc_fc3_binaries})
  set_target_properties (${src}
    PROPERTIES
#    COMPILE_FLAGS "-g -O2 -DPSRFITS -DFFTW -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -fno-common"
#     COMPILE_FLAGS "-D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -fopenmp -O2 -g -DPSRFITS -DFFTW -ffixed-line-length-132 -freal-loops"
#     COMPILE_FLAGS "-D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -O2 -g -DPSRFITS -DFFTW -ffixed-line-length-132 -freal-loops"
     COMPILE_FLAGS "-D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -O2 -g -DPSRFITS -DFFTW -ffixed-line-length-132"
    LINK_FLAGS ""
    LINKER_LANGUAGE Fortran
    CMAKE_SHARED_LIBRARY_LINK_C_FLAGS ""
    CMAKE_SHARED_LINKER_FLAGS ""
  )
  target_link_libraries (${src} sigproc_fort ${PGPLOT_LIBRARIES} ${X11_X11_LIB})
endforeach (src)

foreach (src ${sigproc_fc4_binaries})
  set_target_properties (${src}
    PROPERTIES
#    COMPILE_FLAGS "-g -O2 -DPSRFITS -DFFTW -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -fno-common -freal-loops"
#     COMPILE_FLAGS "-D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -fopenmp -O2 -g -DPSRFITS -DFFTW -ffixed-line-length-132 -freal-loops"
#     COMPILE_FLAGS "-D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -O2 -g -DPSRFITS -DFFTW -ffixed-line-length-132 -freal-loops"
     COMPILE_FLAGS "-D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -O2 -g -DPSRFITS -DFFTW -ffixed-line-length-132"
    LINK_FLAGS ""
    LINKER_LANGUAGE Fortran
    CMAKE_SHARED_LIBRARY_LINK_C_FLAGS ""
    CMAKE_SHARED_LINKER_FLAGS ""
  )
  target_link_libraries (${src} sigproc_fort sigproc ${PGPLOT_LIBRARIES} ${X11_X11_LIB})
endforeach (src)

foreach (src ${sigproc_fc5_binaries})
  set_target_properties (${src}
    PROPERTIES
#    COMPILE_FLAGS "-g -O2 -DPSRFITS -DFFTW -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -fno-common -freal-loops"
#     COMPILE_FLAGS "-D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -fopenmp -O2 -g -DPSRFITS -DFFTW -ffixed-line-length-132 -freal-loops"
#     COMPILE_FLAGS "-D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -O2 -g -DPSRFITS -DFFTW -ffixed-line-length-132 -freal-loops"
     COMPILE_FLAGS "-D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -O2 -g -DPSRFITS -DFFTW -ffixed-line-length-132"
    LINK_FLAGS ""
    LINKER_LANGUAGE Fortran
    CMAKE_SHARED_LIBRARY_LINK_C_FLAGS ""
    CMAKE_SHARED_LINKER_FLAGS ""
  )
  target_link_libraries (${src} sigproc_fort sigproc)
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
  TARGETS ${sigproc_fc1_binaries} ${sigproc_fc2_binaries} ${sigproc_fc3_binaries} ${sigproc_fc4_binaries} ${sigproc_fc5_binaries} 
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )

