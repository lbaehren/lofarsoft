# $Id$

file(GLOB fbdsm_io_sources *.f)

add_library(fbdsm_io ${fbdsm_io_sources})
if(HAVE_PGPLOT)
  target_link_libraries(fbdsm_io fbdsm_withpg)
else(HAVE_PGPLOT)
  target_link_libraries(fbdsm_io fbdsm_woutpg)
endif(HAVE_PGPLOT)
