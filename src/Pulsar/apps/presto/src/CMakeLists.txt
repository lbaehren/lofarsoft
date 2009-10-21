
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
  mpiprepsubband
  makedata makeinf mjd2cal realfft quicklook
  search_bin search_rzw swap_endian prepdata
  check_parkes_raw bary shiftdata dftfold
  patchdata readfile toas2dat taperaw
  accelsearch prepsubband cal2mjd split_parkes_beams
  dat2sdat sdat2dat downsample rednoise un_sc_td bincand
  dump_spigot_zerolag spigot2filterbank
  spigotSband2filterbank GBT350filterbank
  psrorbit window plotbincand prepfold show_pfd
  rfifind zapbirds explorefft exploredat weight_psrfits
)

##__________________________________________________________
## libpresto

add_library (presto ${presto_sources})

target_link_libraries (presto
  ${CMAKE_INSTALL_PREFIX}/lib/libfftw3f.a
  ${CFITSIO_LIBRARIES}
  ${GLIB_LIBRARIES}
  ${PGPLOT_LIBRARIES}
  ${GFORTRAN_LIBRARIES}
  ${X11_LIBRARIES}
  )

set_target_properties (presto
  PROPERTIES
  COMPILE_FLAGS "-DDATABASE='\"${presto_SOURCE_DIR}/lib\"' -DFFTTYPE=USEFFTW  -DUSEMMAP -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64"
  )

add_dependencies (presto
  fftw_install
  tempo_install
  )

##__________________________________________________________
## libsla

add_subdirectory (slalib)

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
## Build instructions for executables

add_executable (mpiprepsubband
  mpiprepsubband_cmd.c
  mpiprepsubband_utils.c
  mpiprepsubband.c
  ${INSTRUMENTOBJS}
  )

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
  least_squares.f
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
target_link_libraries (GBT350filterbank presto)

add_executable (spigotSband2filterbank
  spigotSband2filterbank.c
  spigot.c
  sigproc_fb.c
  )

add_executable (show_pfd
  show_pfd_cmd.c show_pfd.c prepfold_utils.c prepfold_plot.c least_squares.f range_parse.c ${PLOT2DOBJS}
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
  TARGETS ${presto_binaries}
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )

## ==============================================================================
##
##  Test programs
## 
## ==============================================================================

add_executable (testbinresp
  ${presto_SOURCE_DIR}/tests/testbinresp.c
  ${PLOT2DOBJS}
  )

