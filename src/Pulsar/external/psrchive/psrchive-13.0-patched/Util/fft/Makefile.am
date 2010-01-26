
noinst_LTLIBRARIES = libpsrfft.la

noinst_SCRIPTS = install_bench

dist_bin_SCRIPTS = fft_bench

bin_PROGRAMS = fft_speed

fft_speed_SOURCES = fft_speed.C

include_HEADERS = interpolate.h median_smooth.h \
	FTransform.h FTransformAgent.h FTransformPlan.h FTransformBench.h \
	QuaternionFT.h

libpsrfft_la_SOURCES = test_libraries.h shift.C fccf.f ffft.f \
	FTransformAgent.C FTransform.C FTransformBench.C QuaternionFT.C

FFT_BENCH =

if HAVE_MKL_DFTI
libpsrfft_la_SOURCES += MKL_DFTI_Transform.C MKL_DFTI_Transform.h
endif

if HAVE_MKL
libpsrfft_la_SOURCES += MKL_Transform.C MKL_Transform.h
endif

if HAVE_IPP
libpsrfft_la_SOURCES += IPP_Transform.C IPP_Transform.h
FFT_BENCH += IPP
endif

if HAVE_FFTW3
libpsrfft_la_SOURCES += FFTW3_Transform.C FFTW3_Transform.h
FFT_BENCH += FFTW3
endif

TESTS = test_frexp test_enum test_normalization test_interpolate \
	test_real_complex test_FTransformBench

check_PROGRAMS = $(TESTS) test_QuaternionFT

test_enum_SOURCES		= test_enum.C
test_normalization_SOURCES	= test_normalization.C
test_interpolate_SOURCES	= test_interpolate.C
test_interpolate_LDADD		= $(LDADD) ../units/libunits.la
test_real_complex_SOURCES	= test_real_complex.C
test_QuaternionFT_SOURCES	= test_QuaternionFT.C
test_FTransformBench_SOURCES	= test_FTransformBench.C

bench: ./install_bench ./fft_bench ./fft_speed
	csh -f ./install_bench $(FFT_BENCH)

#############################################################################
#

LDADD = libpsrfft.la \
	$(top_builddir)/Util/genutil/libgenutil.la \
	$(top_builddir)/Util/units/libunits.la \
	$(top_builddir)/Util/third/libthird.la

libpsrfft_la_LIBADD = @FFT_LIBS@

include $(top_srcdir)/config/Makefile.include

AM_CPPFLAGS += @FFT_CFLAGS@ 

