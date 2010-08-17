#############################################################################
#
# GMRT udp capture and correlator 
#
#############################################################################

lib_LTLIBRARIES = libgmrt.la 

gmrt_hdr = gmrt_udp.h gmrt_def.h gmrt_udpdb.h gmrt_udpheader.h gmrt_delay_fns.h

libgmrt_la_SOURCES = $(casper_hdr) gmrt_udp.c gmrt_delay_fns.c gmrt_corrsub.c gmrt_astro_cal.c

bin_PROGRAMS = gmrt_udpdb gmrt_udpdb_multi gmrt_udpheader gmrt_udpheader_multi gmrt_delay gmrt_delay_test

LDADD = libgmrt.la $(top_builddir)/src/libpsrdada.la

if HAVE_CUDA

lib_LTLIBRARIES += libgmrtcuda.la

gmrt_corr_hdr = gmrt_corr_dada.h gmrt_corr_lib.h gmrt_corr_cuda.h

libgmrt_la_SOURCES += $(gmrt_corr_hdr) gmrt_corr_dada.c gmrt_corr_lib.c

libgmrtcuda_la_SOURCES = $(gmrt_corr_hdr) gmrt_corr_cuda.cu 

libgmrtcuda_la_LIBADD = gmrt_corr_cuda.lo

gmrt_corr_cuda.lo : gmrt_corr_cuda.cu complex_multiply_device.hpp fill_device.hpp maths_functions.hpp phase_shift_device.hpp unpack_device.hpp device_interface.hpp kernel_defs.hpp maths.hpp stopwatch.hpp fft_device.hpp mac_device.hpp transform_device.hpp 
	@CUDA_NVCC@ -m64 -O3 -c $(srcdir)/gmrt_corr_cuda.cu
	cp $(srcdir)/gmrt_corr_cuda.losave gmrt_corr_cuda.lo

bin_PROGRAMS += gmrt_correlator

gmrt_correlator_SOURCES = gmrt_correlator.C
gmrt_correlator_LDFLAGS = @CUFFT_LIBS@ 

LDADD += libgmrtcuda.la

AM_CPPFLAGS = @CUFFT_CFLAGS@

endif

INCLUDES = -I$(top_srcdir)/src
