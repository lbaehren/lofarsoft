
set (Timba_contrib_SBY_beams_sources
  beam.c
  beam_cutoff.c
  beam_dr.c
  beam_dr_phi_imag.c
  beam_dr_phi_real.c
  beam_dr_theta_imag.c
  beam_dr_theta_real.c
  )

add_library (timba_beams ${Timba_contrib_SBY_beams_sources})