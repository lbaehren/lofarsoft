# All the input/output routines here are compiled into one single library libfbdsm_io.a 
# using gfortran and suitable flags depending on os. Include files are in ../includes
# $(FC) -c *.f -o *.o

DIR=../
CFLAGS = -I$(DIR)/includes
ODIR=../libs

ifeq ($(shell uname -m), x86_64)
 FC=gfortran -fPIC -fno-second-underscore $(CFLAGS)     
else   # ! i686
 FC=/usr/bin/g77 -g -fno-second-underscore -fbounds-check $(CFLAGS)
endif

ifeq ($(shell uname -s), Darwin)
 FC=gfortran -fbounds-check $(CFLAGS)
endif

OBJ := $(patsubst %.f,%.o,$(wildcard *.f))

all: libfbdsm_io.a clean

libfbdsm_io.a : $(OBJ)
	ar r $(ODIR)/libfbdsm_io.a $?

%.o: %.f
	$(FC) -c $< -o $@

clean: 
	rm -fr *.o 


