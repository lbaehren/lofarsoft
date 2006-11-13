noinst_PROGRAMS		= psfdemo

psfdemo_SOURCES		= psfdemo.cc
psfdemo_LDADD		= ../../src/PSF/libpsf.la
psfdemo_DEPENDENCIES	= ../../src/PSF/libpsf.la $(LOFAR_DEPEND)

CXXFLAGS		+= -Wno-comment

include $(lofar_sharedir)/Makefile.common
