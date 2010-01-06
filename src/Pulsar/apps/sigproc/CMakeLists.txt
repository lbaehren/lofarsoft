
## Libraries

## libsigproc

set (sigproc_sources 
add_channels.c
add_samples.c
aliases.c
angle_split.c
aoftm.c
bandfactors.c
## baseline.c
bit.c
bpp2fb.c
cel2gal.c
chan_freqs.c
## clock.c
close_log.c
## cpower.c
cprofc.c
decimate_data.c
decimate_header.c
dedisperse_data.c
dedisperse_header.c
deg2dms.c
dialog.C
dmdelay.c
dmshift.c
## dosearch.c
## epnpack.c
eraseDP.c
error_message.c
## fast_mean.c
fetch_hdrval.c
## ffft.c
ffreq.c
## fftdata.c
## $(FFTWF))\
file_exists.c
filterbank_header.c
## find_norm.c
fold_data.c
fold_header.c
## formspec.c
## freq.c
fshift_prof.c
## getddis.c
## getfast_rmea.c
## getmeanrms.c
## getmjd.c
## getrmea.c
## getrrms.c
## glun.c
gmrt2fb.c
help.c
## histmax.c
ignored_channels.c
## indexxf77.c
indexx.c
inv_cerf.c
## length.c
lookup.c
machine2prf.c
## meanrms.c
## minmax.c
mjd.c
## mmzap.c
## normal.c
norm_prof.c
np2.c
nrselect.c
nrsort.c
nrutil.c
nsamples.c
ooty2fb.c
open_file.c
open_files.c
open_log.c
pack_unpack.c
phcalc.c
print_version.c
process.c
prof_adds.c
prof_ddis.c
prof_sbas.c
prof_sumc.c
prof_sumifs.c
pspm2fb.c
pspm_decode.c
pspm_prof.c
pspm_tstart.c
pulsar2k2fb.c
pulse.c
put.c
## quikgray.c
## ralphs_mean.c
## random.c
## rdfbtab.c
## rdhead.c
read_aoscan.c
read_block.c
## readdat.c
read_header.c
read_polyco.c
## readspec.c
## readsus.c
readtim.c
## rebin.c
recipes.c
## recon_prof.c
## resample.c
## rotate_time.c
## rwepn.c
scaledata.c
scale_prof.c
scamp2fb.c
## seekin.c
select.c
send_stuff.c
shift_prof.c
## short.c
single_ch.c
## singlepulse.c
sizeof_file.c
slalib.c
## slfit.c
smooth.c
## spcsnr.c
## sprof.c
ssm.c
strings_equal.c
submean.c
submedian.c
## sumhrm.c
swap_bytes.c
thresh_1d.c
## timer.c
typeof_inputdata.c
update_log.c
uttime.c
vanvleck.c
vmax.c
vmin.c
wappcorrect.c
wapp_prof.c
whiten.c
write_epn.c
## writeepn.c
write_profiles.c
write_pulses.c
## writespec.c
y.tab.c
## zap_birdies.c
## zapit.c
)

#set (sigproc_binaries
#    filterbank fake header bandpass reader decimate 
#	dedisperse fold polyco2period profile flux splice barycentre 
#	seek best step postproc grey mask csearch polyco depolyco  
#	chaninfo 
#)

set (sigproc_binaries
    filterbank fake extract gmrt2fil
)

#set (sigproc_fc_binaries
#   tune pgplotter quickplot plotpulses getpulse
#)

##__________________________________________________________
## libsigproc

add_library (sigproc STATIC ${sigproc_sources})

target_link_libraries (sigproc
  ${CMAKE_INSTALL_PREFIX}/lib/libfftw3f.a
  libfftw3.a
  ${CFITSIO_LIBRARIES}
  ${PGPLOT_LIBRARIES}
  ${X11_LIBRARIES}
  )

if (UNIX)
 if (APPLE)
   set_target_properties (sigproc
     PROPERTIES
     SUFFIX .a
     COMPILE_FLAGS "-g -O2 -DPSRFITS -DFFTW -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64"
     LINK_FLAGS "-static"
     )
 else (APPLE)
   set_target_properties (sigproc
     PROPERTIES
     SUFFIX .a
     COMPILE_FLAGS "-g -O2 -DPSRFITS -DFFTW -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64"
     LINK_FLAGS "-static"
     )
 endif (APPLE)
endif (UNIX)

add_dependencies (sigproc  
  sigproc_fort
  )

##__________________________________________________________
## lib fortran

add_subdirectory (fortran)

## ==============================================================================
##
##  Executables
##
## ==============================================================================

##__________________________________________________________
## Build instructions for executables

add_executable (filterbank
  filterbank.c head_parse.c wapp2fb.c
  )

add_executable (extract
  extract.c
  )

add_executable (fake
  fake.c
  )

add_executable (gmrt2fil
  gmrt2fil.c 
  )



## linker instructions

foreach (src ${sigproc_binaries})
  set_target_properties (${src}
    PROPERTIES
    COMPILE_FLAGS "-g -O2 -DPSRFITS -DFFTW -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64"
  )
  target_link_libraries (${src} sigproc ${CMAKE_INSTALL_PREFIX}/lib/libfftw3f.a ${CFITSIO_LIBRARIES} ${libm})
endforeach (src)

foreach (src ${sigproc_fc_binaries})
  set_target_properties (${src}
    PROPERTIES
    LINKER_LANGUAGE Fortran
    COMPILE_FLAGS "-g -O2 -DPSRFITS -DFFTW -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64"
  )
  target_link_libraries (${src} sigproc ${CFITSIO_LIBRARIES} ${libm})
endforeach (src)

add_custom_target (sigproc_prep
  COMMAND touch ${PULSAR_SOURCE_DIR}/apps/sigproc/*_cmd.c
  WORKING_DIRECTORY ${PULSAR_SOURCE_DIR}/apps/sigproc/src
  COMMENT "Performing of 'make prep' command as 'touch ${PULSAR_SOURCE_DIR}/apps/sigproc/*_cmd.c'"
  )

add_custom_target (sigproc_cat_install
  COMMAND cp pulsars.cat parkes_birds.txt psr_catalog.txt aliases.txt a2x.ps a2xshell.ps ${CMAKE_INSTALL_PREFIX}/lib/
  WORKING_DIRECTORY ${PULSAR_SOURCE_DIR}/apps/sigproc
  COMMENT "Executing 'copy from ${PULSAR_SOURCE_DIR}/apps/sigproc of pulsars.cat parkes_birds.txt psr_catalog.txt aliases.txt a2x.ps a2xshell.ps to ${CMAKE_INSTALL_PREFIX}/lib/'"
  )

## ==============================================================================
##
##  Installation
## 
## ==============================================================================

install (
  TARGETS sigproc
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )

install (
  TARGETS ${sigproc_binaries} ${sigproc_fc_binaries}
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )

## ==============================================================================
##
##  Test programs
## 
## ==============================================================================

##add_executable (testbinresp
##  ${sigproc_SOURCE_DIR}/tests/testbinresp.c
##  ${PLOT2DOBJS}
##  )

