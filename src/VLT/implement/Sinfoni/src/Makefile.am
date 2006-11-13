SUBDIRS			= . Blitz PSF

INSTHDRS 		= AsciiReader.h \
			  Assert.h \
			  Config.h \
			  Exception.h \
			  Exceptions.h \
			  FFT.h \
			  Math.h \
			  Typedefs.h \
			  Types.h

NOINSTHDRS		= 

TCCHDRS			= 

pkginclude_HEADERS	= $(INSTHDRS) $(TCCHDRS)

noinst_HEADERS		= $(NOINSTHDRS)

DOCHDRS			= $(INSTHDRS) $(NOINSTHDRS)

lib_LTLIBRARIES		= libsinfoni.la

libsinfoni_la_SOURCES	= AsciiReader.cc \
			  Exception.cc \
			  FFT.cc

CXXFLAGS		+= -Wno-comment

include $(lofar_sharedir)/Makefile.common
