# $Id$

file(GLOB fbdsm_nr_sources *.f)

add_library(fbdsm_nr ${fbdsm_nr_sources})
if(HAVE_PGPLOT)
  target_link_libraries(fbdsm_nr fbdsm_withpg)
else(HAVE_PGPLOT)
  target_link_libraries(fbdsm_nr fbdsm_woutpg)
endif(HAVE_PGPLOT)

