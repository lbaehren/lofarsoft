DIR=../../implement/fBDSM/
CFLAGS = -I$(DIR)/includes
LIBS=$(DIR)/libs

ifeq ($(shell uname -m), x86_64)
 PGLIB = -lpng -lX11 -lpgplot
 CFITS = -L/data/lib64/cfitsio/lib/ -lcfitsio
 WCSINC = -I/data/lib64/wcslib/wcslibgfort_pic/include/wcslib
 WCSLIB = -L/data/lib64/wcslib/wcslibgfort_pic/lib -lwcs -lfftw3 
 #FC=/usr/bin/g77  -fbounds-check -fno-second-underscore $(CFLAGS)
 #FC1=/usr/bin/g77 -fbounds-check -fno-second-underscore -Wno-globals -fno-globals $(CFLAGS)
 FC=gfortran -fPIC -fno-second-underscore $(CFLAGS)     # fbounds-check gives s_rnge not found error for py
 FC1=gfortran -fPIC -fno-second-underscore $(CFLAGS)  # for python

else   # ! i686
 PGLIB =  -L/usr/lib -L/usr/X11R6/lib -lpng -lX11 -L/usr/lib/pgplot -lpgplot
 CFITS =  -L/disks/strw9/niruj/strw1/fitsio_dir/lib/ -lcfitsio
 WCSLIB = -L/disks/strw9/niruj/strw1/wcslib/lib -lwcs -lfftw3
 WCSINC = -I/disks/strw9/niruj/strw1/wcslib/include/wcslib
 FC=/usr/bin/g77 -g -fno-second-underscore -fbounds-check $(CFLAGS)
 FC1=/usr/bin/g77 -g -fno-second-underscore -fbounds-check -Wno-globals -fno-globals $(CFLAGS)
endif

ifeq ($(shell uname -s), Darwin)
 CFITS = -L/sw/cfitsio/lib/ -lcfitsio
 WCSLIB = -L/sw/wcslib/lib -lwcs    
 WCSINC = -I/sw/wcslib/include/wcslib
 PGLIB =  -L/usr/X11R6/lib -lX11 -Wl,-framework -Wl,Foundation -L/sw/lib -lpng -lz -laquaterm -L/sw/lib/pgplot -lpgplot
 PGLIB2 = -L/usr/X11R6/lib -lX11 -L/sw/lib -lpng -lz -laquaterm -L/sw/lib/pgplot -lpgplot
 FC=gfortran -fbounds-check $(CFLAGS)
 FC1=gfortran -fbounds-check $(CFLAGS)
endif

LIBNR=$(LIBS)/libfbdsm_mynr.a 
LIBIO=$(LIBS)/libfbdsm_io.a 
LIBNP=$(LIBS)/libfbdsm_woutpg.a 
LIBPG=$(LIBS)/libfbdsm_withpg.a 
LIBWF=$(LIBS)/libfbdsm_wcsfits.a 
LIBGS=$(LIBS)/libfbdsm_gsm.a 

noise : noise.f $(LIBNR) $(LIBIO) $(LIBWF) $(LIBPG)
	$(FC) noise.f $(LIBNR) $(LIBIO) $(LIBWF) $(LIBPG) $(CFLAGS) $(PGLIB) $(WCSLIB) $(CFITS) -o noise

bdsm : bdsm.f $(LIBNR) $(LIBIO) $(LIBNP) $(LIBWF)
	$(FC) bdsm.f $(LIBNR) $(LIBIO) $(LIBNP) $(LIBWF) $(CFLAGS) $(WCSLIB) $(CFITS) -o bdsm

or_gsm : or_gsm.f $(LIBNR) $(LIBIO) $(LIBWF) $(LIBPG) $(LIBGS)
	$(FC) or_gsm.f $(LIBNR) $(LIBIO) $(LIBWF) $(LIBPG) $(LIBGS) $(WCSLIB) $(PGLIB) $(CFITS) -o or_gsm


all: noise bdsm or_gsm

clean: 
	rm -fr noise or_gsm bdsm

