
## Libraries

## libpresto

set (presto_sources 
  amoeba.c
  atwood.c barycenter.c birdzap.c cand_output.c
  characteristics.c cldj.c chkio.c corr_prep.c corr_routines.c
  correlations.c database.c dcdflib.c dispersion.c
  fastffts.c fftcalls.c fminbr.c fold.c fresnl.c ioinf.c
  get_candidates.c iomak.c ipmpar.c maximize_r.c maximize_rz.c
  median.c minifft.c misc_utils.c clipping.c
  orbint.c output.c read_fft.c responses.c
  rzinterp.c rzwinterp.c select.c sorter.c swapendian.c
  transpose.c twopass.c twopass_real_fwd.c
  twopass_real_inv.c vectors.c multifiles.c mask.c
  fitsfile.c hget.c hput.c imio.c
)

add_library (presto ${presto_sources})

## libsla

file (GLOB sla_sources slalib/*.f)

#add_library (sla ${sla_sources})
#set_target_properties (sla)

## Executables
##
## ==============================================================================

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

add_executable (mpiprepsubband
  mpiprepsubband_cmd.c
  mpiprepsubband_cmd.c
  mpiprepsubband_utils.c
  mpiprepsubband.c
  ${INSTRUMENTOBJS}
  )
#target_link_libraries (mpiprepsubband $(PRESTOLINK) -lcfitsio -lm)
