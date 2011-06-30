# $Id$

file(GLOB fbdsm_woutpg_sources *.f)

add_library(fbdsm_woutpg ${fbdsm_woutpg_sources})
target_link_libraries(fbdsm_woutpg
  fbdsm_io
  fbdsm_wcsfits
  fbdsm_gsm
)

add_f2py_module(_py2fbdsm
  check.f
  get_keyword.f
  iland_mat2list.f
  sub_prog.f
  DESTINATION lib/python/bdsm)

target_link_libraries(_py2fbdsm fbdsm_io)
