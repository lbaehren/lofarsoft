
noinst_LTLIBRARIES = libstar.la

libstar_la_SOURCES = altaz.f cldj.f dat.f dcc2s.f dcs2c.f deuler.f	\
	dimxv.f dmxv.f drange.f dranrm.f dsep.f dsepv.f dtt.f dvn.f	\
	dvdv.f dvxv.f epj.f eqgal.f evp.f galeq.f geoc.f gmst.f pa.f	\
	prec.f pvobs.f eqecl.f ecleq.f ecmat.f

# C wrappers to the Fortran SLALIB bindings
include_HEADERS = slalib.h
libstar_la_SOURCES += sla_wrap.c

include $(top_srcdir)/config/Makefile.include

