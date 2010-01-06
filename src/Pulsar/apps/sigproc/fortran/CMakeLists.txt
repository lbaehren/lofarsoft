## ==============================================================================
##
##  Project settings
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

include_directories (${PULSAR_SOURCE_DIR}/apps/sigproc)

file (GLOB fort_sigproc_sources baseline.f
best.f
chaninfo.f
clock.f
cpower.f
dosearch.f
epnhdr.f
epnpack.f
fast_mean.f
ffa.f
ffft.f
fftdata.f
fftwdata.f
fftwdummy.f
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
grey.f
histmax.f
indexxg77.f
length.f
mask.f
meanrms.f
minmax.f
mjdnow.f
mmzap.f
normal.f
peak.f
plotpulses.f
plotpulses.old.f
postproc.f
quickplot.f
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
seek.f
seekin.f
short.f
singlepulse.f
slfit.f
sort3.f
spcsnr.f
spec.f
sprof.f
step.f
sumfft.f
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

if (UNIX)
 if (APPLE)
   set_target_properties (sigproc_fort
     PROPERTIES
     SUFFIX .a
     COMPILE_FLAGS ""
     LINK_FLAGS ""
     )
 else (APPLE)
   set_target_properties (sigproc_fort
     PROPERTIES
     SUFFIX .a
     COMPILE_FLAGS ""
     LINK_FLAGS ""
     )
 endif (APPLE)
endif (UNIX)

##set_target_properties (sigproc_fort
##  PROPERTIES
##  COMPILE_FLAGS "-fPIC -g -O2 -fno-second-underscore"
##  LINK_FLAGS "-fno-second-underscore"
##  )

install (
  TARGETS sigproc_fort
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )
