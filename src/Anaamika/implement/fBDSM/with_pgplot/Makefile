# All the routines which use pgplot (and thoe which dont have) are compiled into one single library libfbdsm_withpg.a 
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

all: libfbdsm_withpg.a 

libfbdsm_withpg.a : $(OBJ)
	ar r $(ODIR)/libfbdsm_withpg.a $?

%.o: %.f
	$(FC) -c $< -o $@

clean: 
	rm -fr *.o 


