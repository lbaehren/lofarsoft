# All the wcslib and cfitsio related routines here are compiled into one single library libfbdsm_wcsfits.a 
# using gfortran and suitable flags depending on os. Include files are in ../includes
# $(FC) -c *.f -o *.o 

DIR=../
CFLAGS = -I$(DIR)/includes
ODIR=../libs

ifeq ($(shell uname -m), x86_64)
 FC=gfortran -fPIC -fno-second-underscore $(CFLAGS) 
 FC1=gfortran -fPIC -fno-second-underscore $(CFLAGS)
 WCSINC = -I/data/lib64/wcslib/wcslibgfort_pic/include/wcslib
else   # ! i686
 FC=/usr/bin/g77 -g -fno-second-underscore -fbounds-check $(CFLAGS)
 FC1=/usr/bin/g77 -g -fno-second-underscore -fbounds-check -Wno-globals -fno-globals $(CFLAGS)
 WCSINC = -I/disks/strw9/niruj/strw1/wcslib/include/wcslib
endif

ifeq ($(shell uname -s), Darwin)
 FC=gfortran -fbounds-check $(CFLAGS)
 FC1=gfortran -fbounds-check $(CFLAGS)
 WCSINC = -I/sw/wcslib/include/wcslib
endif

OBJ := $(patsubst %.f,%.o,$(wildcard *.f))

all: libfbdsm_wcsfits.a clean

libfbdsm_wcsfits.a: $(OBJ)
	ar r $(ODIR)/libfbdsm_wcsfits.a $?

wcs_struct.o : wcs_struct.f 
	$(FC1) -c -o wcs_struct.o wcs_struct.f $(WCSINC)
wcs_xy2radec.o : wcs_xy2radec.f 
	$(FC1) -c -o wcs_xy2radec.o wcs_xy2radec.f $(WCSINC)
wcs_radec2xy.o : wcs_radec2xy.f 
	$(FC1) -c -o wcs_radec2xy.o wcs_radec2xy.f $(WCSINC)
freewcs.o : freewcs.f 
	$(FC1) -c -o freewcs.o freewcs.f $(WCSINC)
wcs_isgoodpix.o : wcs_isgoodpix.f 
	$(FC1) -c -o wcs_isgoodpix.o wcs_isgoodpix.f $(WCSINC)
call_wcs_c2p.o : call_wcs_c2p.f wcs_struct.o wcs_radec2xy.o
call_wcs_p2c.o : call_wcs_p2c.f wcs_struct.o wcs_xy2radec.o
wcs_insideimage.o : wcs_insideimage.f 
wcs_isgoodcoord.o : wcs_isgoodcoord.f 
deletefile.o : deletefile.f 
printerror.o : printerror.f 

clean:
	rm -fr *.o 


