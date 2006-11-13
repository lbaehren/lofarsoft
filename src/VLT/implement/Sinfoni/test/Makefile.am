SUBDIRS				= . Blitz PSF

check_PROGRAMS			= tAsciiReader \
				  tFitsIO \
				  tMath \
				  tFFT

TESTS				= tAsciiReader.sh \
				  tFitsIO.sh \
				  tMath.sh \
				  tFFT.sh

tAsciiReader_SOURCES		= tAsciiReader.cc
tAsciiReader_LDADD		= ../src/libsinfoni.la
tAsciiReader_DEPENDENCIES	= ../src/libsinfoni.la $(LOFAR_DEPEND)

tFitsIO_SOURCES			= tFitsIO.cc
tFitsIO_LDADD			= ../src/libsinfoni.la
tFitsIO_DEPENDENCIES		= ../src/libsinfoni.la $(LOFAR_DEPEND)

tMath_SOURCES			= tMath.cc
tMath_LDADD			= ../src/libsinfoni.la
tMath_DEPENDENCIES		= ../src/libsinfoni.la $(LOFAR_DEPEND)

tFFT_SOURCES			= tFFT.cc
tFFT_LDADD			= ../src/libsinfoni.la
tFFT_DEPENDENCIES		= ../src/libsinfoni.la $(LOFAR_DEPEND)


CXXFLAGS			+= -Wno-comment

include $(lofar_sharedir)/Makefile.common
