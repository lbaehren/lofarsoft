# $Id$

file(GLOB fbdsm_gsm_sources *.f)

add_library(fbdsm_gsm ${fbdsm_gsm_sources})
target_link_libraries(fbdsm_gsm fbdsm_nr)
