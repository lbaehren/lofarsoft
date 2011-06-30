# $Id$

file(GLOB fbdsm_withpg_sources *.f)

add_library(fbdsm_withpg ${fbdsm_withpg_sources})

# PGPLOT is often underlinked, so we need to link to -lX11 and -lg2c ourselves
set(PGPLOT_EXTRA_LIBS)
if(X11_X11_LIB)
  list(APPEND PGPLOT_EXTRA_LIBS ${X11_X11_LIB})
endif(X11_X11_LIB)
if(G2C_LIBRARY)
  list(APPEND PGPLOT_EXTRA_LIBS ${G2C_LIBRARY})
endif(G2C_LIBRARY)

target_link_libraries(fbdsm_withpg
  fbdsm_io
  fbdsm_wcsfits
  fbdsm_gsm
  ${PGPLOT_LIBRARIES}
  ${PGPLOT_EXTRA_LIBS}
)

add_f2py_module(_py2fbdsm
  check.f
  get_keyword.f
  iland_mat2list.f
  sub_prog.f
  DESTINATION lib/python/bdsm)

target_link_libraries(_py2fbdsm fbdsm_io)
