
## Libraries

## libpresto

set (presto_sources 
  amoeba.c
  atwood.c
  barycenter.c
  birdzap.c
  cand_output.c
  characteristics.c
  cldj.c
  chkio.c
  corr_prep.c
  corr_routines.c
  correlations.c
  database.c
  dcdflib.c
  dispersion.c
  fastffts.c
  fftcalls.c
  fminbr.c
  fold.c
  fresnl.c
  ioinf.c
  get_candidates.c
  iomak.c
  ipmpar.c
  maximize_r.c
  maximize_rz.c
  median.c
  minifft.c
  misc_utils.c
  clipping.c
  orbint.c
  output.c
  read_fft.c
  responses.c
  rzinterp.c
  rzwinterp.c
  select.c
  sorter.c
  swapendian.c
  transpose.c
  twopass.c
  twopass_real_fwd.c
  twopass_real_inv.c
  vectors.c
  multifiles.c
  mask.c
  fitsfile.c
  hget.c
  hput.c
  imio.c
)

set (presto_binaries
  makedata makeinf mjd2cal realfft quicklook
  search_bin search_rzw swap_endian prepdata
  check_parkes_raw bary shiftdata dftfold
  patchdata readfile toas2dat taperaw
  prepsubband cal2mjd split_parkes_beams
  dat2sdat sdat2dat downsample rednoise un_sc_td bincand
  dump_spigot_zerolag spigot2filterbank
  spigotSband2filterbank GBT350filterbank test_multifiles
)

set (presto_glib_binaries
  accelsearch
)

set (presto_fc_binaries
  rfifind plotbincand profile
  psrorbit window zapbirds explorefft exploredat
  weight_psrfits 
)

set (presto_fc_special_binaries
  prepfold 
  show_pfd 
)

set (presto_makewisdom_binaries
  makewisdom
)

##__________________________________________________________
## libpresto

add_library (presto SHARED ${presto_sources})

target_link_libraries (presto
  ${CMAKE_INSTALL_PREFIX}/lib/libfftw3f.a
  ${CFITSIO_LIBRARIES}
  ${GLIB_LIBRARIES}
  ${PGPLOT_LIBRARIES}
  ${GFORTRAN_LIBRARIES}
  ${X11_LIBRARIES}
  ${MPI_LIBRARIES}
  )

#target_link_libraries (presto
#  ${CMAKE_INSTALL_PREFIX}/lib/libfftw3f.a
#  /opt/local/lib/libfftw3f.a
#  ${CFITSIO_LIBRARIES}
#  ${GLIB_LIBRARIES}
#  ${PGPLOT_LIBRARIES}
#  ${GFORTRAN_LIBRARIES}
#  ${X11_LIBRARIES}
#  ${MPI_LIBRARIES}
#  )


if (UNIX)
 if (APPLE)
   set_target_properties (presto
     PROPERTIES
     SUFFIX .dylib
     COMPILE_FLAGS "-g -fPIC -DDATABASE='\"${CMAKE_INSTALL_PREFIX}/lib\"' -DUSEFFTW -DUSEMMAP -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -ffast-math"
     LINK_FLAGS "-dynamiclib"
     )
 else (APPLE)
   set_target_properties (presto
     PROPERTIES
     SUFFIX .so
     COMPILE_FLAGS "-g -fPIC -DDATABASE='\"${CMAKE_INSTALL_PREFIX}/lib\"' -DUSEFFTW -DUSEMMAP -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -ffast-math"
     LINK_FLAGS "-shared"
     )
 endif (APPLE)
endif (UNIX)

add_dependencies (presto
  fftw_install
  tempo_install
  bf2presto_install
  presto_includes
  presto_pyscripts_install
  presto_pylibscripts_install
  presto_clig_install
  presto_cat_install
  )

##__________________________________________________________
## libsla

add_subdirectory (slalib)
add_subdirectory (fortran)

## ==============================================================================
##
##  Executables
##
## ==============================================================================

##__________________________________________________________
## Commonly used sets of source files

set (INSTRUMENTOBJS
  multibeam.c
  bpp.c
  gmrt.c
  spigot.c
  sigproc_fb.c
  wapp.c
  wapp_head_parse.c
  wapp_y.tab.c
  psrfits.c
  )

set (PLOT2DOBJS
  powerplot.c 
  xyline.c
  )

##__________________________________________________________
## Source files in subdirectories

add_subdirectory (mpi)

##__________________________________________________________
## Build instructions for executables

add_executable (accelsearch
  accelsearch_cmd.c 
  accel_utils.c 
  accelsearch.c 
  zapping.c
  )

add_executable (bary
  bary.c
  )

add_executable (bincand
  bincand_cmd.c
  bincand.c
  )

add_executable (dftfold
  dftfold_cmd.c 
  dftfold.c
  )

add_executable (shiftdata
  shiftdata.c
  )

add_executable (patchdata
  patchdata.c
  )

add_executable (dat2sdat
  dat2sdat.c
  )

add_executable (sdat2dat
  sdat2dat.c
  )

add_executable (check_parkes_raw
  check_parkes_raw.c
  multibeam.c
  )

add_executable (downsample
  downsample.c
  downsample_cmd.c
  )

add_executable (split_parkes_beams
  split_parkes_beams.c
  )

add_executable (test_multifiles
  test_multifiles.c
  )

add_executable (rfifind
  rfifind_cmd.c
  rfifind.c
  rfi_utils.c
  rfifind_plot.c
  range_parse.c
  ${INSTRUMENTOBJS}
  ${PLOT2DOBJS}
  )

add_executable (prepdata
  prepdata_cmd.c
  prepdata.c
  ${INSTRUMENTOBJS}
  )

add_executable (prepsubband
  prepsubband_cmd.c
  prepsubband.c
  ${INSTRUMENTOBJS}
  )

add_executable (prepfold
  prepfold_cmd.c
  prepfold.c
  prepfold_utils.c
  prepfold_plot.c
  polycos.c
  readpar.c
  ${INSTRUMENTOBJS}
  ${PLOT2DOBJS}
  )

add_executable (dump_spigot_zerolag
  dump_spigot_zerolag.c
  spigot.c
  )

add_executable (spigot2filterbank
  spigot2filterbank_cmd.c
  spigot2filterbank.c
  spigot.c
  sigproc_fb.c
  sla.c
  )

add_executable (GBT350filterbank
  GBT350filterbank.c
  spigot.c
  sigproc_fb.c
  )

add_executable (spigotSband2filterbank
  spigotSband2filterbank.c
  spigot.c
  sigproc_fb.c
  )

add_executable (show_pfd
  show_pfd_cmd.c 
  show_pfd.c 
  prepfold_utils.c 
  prepfold_plot.c 
  range_parse.c 
  ${PLOT2DOBJS}
)

add_executable (makedata
  com.c randlib.c makedata.c
)

add_executable (makeinf
  makeinf.c ioinf.c
)

add_executable (mjd2cal
  djcl.c mjd2cal.c
)

add_executable (cal2mjd
  cldj.c cal2mjd.c
)

add_executable (plotbincand
  plotbincand.c ${PLOT2DOBJS}
)

add_executable (profile 
  profile_cmd.c profile.c ${PLOT2DOBJS}
)

add_executable (psrorbit
  psrorbit.c ${PLOT2DOBJS}
)

add_executable (quicklook 
  quicklook.c
  )

add_executable (readfile
  readfile_cmd.c readfile.c ${INSTRUMENTOBJS}
  )

add_executable (realfft
  realfft_cmd.c realfft.c
  )

add_executable (rednoise
  rednoise.c rednoise_cmd.c
  )

add_executable (search_bin
  search_bin_cmd.c
  search_bin.c
  )

add_executable (search_rzw
  search_rzw_cmd.c
  search_rzw.c
  )

add_executable (taperaw
  taperaw.c
  )

add_executable (toas2dat
  toas2dat_cmd.c
  toas2dat.c
  )

add_executable (un_sc_td
  un_sc_td.c
  )

add_executable (swap_endian
  swap_endian.c
  )

add_executable (window
  window.c
  ${PLOT2DOBJS}
  )

add_executable (zapbirds
  zapbirds_cmd.c
  zapbirds.c
  zapping.c
  ${PLOT2DOBJS}
  )

add_executable (explorefft
  explorefft.c
  ${PLOT2DOBJS}
  )

add_executable (exploredat
  exploredat.c
  ${PLOT2DOBJS}
  )

add_executable (weight_psrfits
  weight_psrfits.c
  psrfits.c
  ${PLOT2DOBJS}
  )
  
add_executable (makewisdom
  makewisdom.c
  )

## linker instructions

foreach (src ${presto_binaries})
  set_target_properties (${src}
    PROPERTIES
    COMPILE_FLAGS "-g -fPIC -DDATABASE='\"${CMAKE_INSTALL_PREFIX}/lib\"' -DUSEFFTW -DUSEMMAP -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -ffast-math"
  )
  target_link_libraries (${src} presto sla ${CMAKE_INSTALL_PREFIX}/lib/libfftw3f.a ${CFITSIO_LIBRARIES} ${libm})
endforeach (src)

foreach (src ${presto_glib_binaries})
  set_target_properties (${src}
    PROPERTIES
#    COMPILE_FLAGS "-g -fPIC -DDATABASE='\"${CMAKE_INSTALL_PREFIX}/lib\"' -DUSEMMAP -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64"
    COMPILE_FLAGS "-g  -fPIC -DDATABASE='\"${CMAKE_INSTALL_PREFIX}/lib\"' -DUSEFFTW -DUSEMMAP -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -ffast-math"
  )
  target_link_libraries (${src} presto ${CMAKE_INSTALL_PREFIX}/lib/libfftw3f.a ${GLIB_LIBRARIES} ${libm})
endforeach (src)


foreach (src ${presto_fc_binaries})
  set_target_properties (${src}
    PROPERTIES
    LINKER_LANGUAGE Fortran
    COMPILE_FLAGS "-g -O -fPIC -DDATABASE='\"${CMAKE_INSTALL_PREFIX}/lib\"' -DUSEFFTW -DUSEMMAP -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -ffast-math"
  )
  target_link_libraries (${src} presto ${CFITSIO_LIBRARIES} ${libm})
endforeach (src)

foreach (src ${presto_fc_special_binaries})
  set_target_properties (${src}
    PROPERTIES
    LINKER_LANGUAGE Fortran
    COMPILE_FLAGS "-g -O -fPIC -DDATABASE='\"${CMAKE_INSTALL_PREFIX}/lib\"' -DUSEFFTW -DUSEMMAP -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -ffast-math"
  )
  target_link_libraries (${src} presto fort ${libpthread} ${libz} ${libdl} ${libm})
#  target_link_libraries (${src} presto fort X11 ${libpthread} png ${libz} ${libdl} ${libm})
endforeach (src)

add_custom_target (presto_prep
  COMMAND touch ${PULSAR_SOURCE_DIR}/apps/presto/src/*_cmd.c
  WORKING_DIRECTORY ${PULSAR_SOURCE_DIR}/apps/presto/src
  COMMENT "Performing of 'make prep' command as 'touch ${PULSAR_SOURCE_DIR}/apps/presto/src/*_cmd.c'"
  )

foreach (src ${presto_makewisdom_binaries})
  set_target_properties (${src}
    PROPERTIES
    COMPILE_FLAGS "-fPIC -DDATABASE='\"${CMAKE_INSTALL_PREFIX}/lib\"' -DUSEFFTW -DUSEMMAP -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -O3 -ffast-math -W"
    LINKER_FLAGS "-fPIC -DDATABASE='\"${CMAKE_INSTALL_PREFIX}/lib\"' -DUSEFFTW -DUSEMMAP -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -O3 -ffast-math -W"
  )
  target_link_libraries(${src} ${CMAKE_INSTALL_PREFIX}/lib/libfftw3f.a m)
endforeach (src)

if (PRESTO_MAKEWISDOM)
add_custom_target (presto_makewisdom
  COMMAND ./makewisdom
  COMMAND cp fftw_wisdom.txt ${CMAKE_INSTALL_PREFIX}/lib/
  WORKING_DIRECTORY ${PULSAR_BINARY_DIR}/apps/presto/src
  COMMENT "Executing '${PULSAR_BINARY_DIR}//apps/presto/src/makewisdom' followed by 'cp ${PULSAR_BINARY_DIR}/apps/presto/src/fftw_wisdom.txt ${CMAKE_INSTALL_PREFIX}/lib/'"
  )
add_dependencies (presto_makewisdom makewisdom)
endif (PRESTO_MAKEWISDOM)

add_custom_target (presto_cat_install
  COMMAND cp pulsars.cat parkes_birds.txt psr_catalog.txt aliases.txt a2x.ps a2xshell.ps ${CMAKE_INSTALL_PREFIX}/lib/
  WORKING_DIRECTORY ${PULSAR_SOURCE_DIR}/apps/presto/lib
  COMMENT "Executing 'copy from ${PULSAR_SOURCE_DIR}/apps/presto/lib of pulsars.cat parkes_birds.txt psr_catalog.txt aliases.txt a2x.ps a2xshell.ps to ${CMAKE_INSTALL_PREFIX}/lib/'"
  )

## ==============================================================================
##
##  Installation
## 
## ==============================================================================

install (
  TARGETS presto
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )

install (
  TARGETS ${presto_binaries} ${presto_fc_binaries} ${presto_fc_special_binaries} 
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
##  ${presto_SOURCE_DIR}/tests/testbinresp.c
##  ${PLOT2DOBJS}
##  )

