# $Id$

file(GLOB fbdsm_wcsfits_sources *.f)

add_library(fbdsm_wcsfits ${fbdsm_wcsfits_sources})
target_link_libraries(fbdsm_wcsfits ${WCSLIB_LIBRARIES} ${CFITSIO_LIBRARIES})
