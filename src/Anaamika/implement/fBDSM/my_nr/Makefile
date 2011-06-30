# All the modified NR routines here and the user defined functions in directory ./functions 
# are compiled into one single library libfbdsm_nr.a using gfortran and suitable flags 
# depending on os. Include files are in ../includes

ODIR=../libs

ifeq ($(shell uname -m), x86_64)
 FC=gfortran -fPIC -fno-second-underscore $(CFLAGS)
else   # ! i686
 FC=/usr/bin/g77 -g -fno-second-underscore -fbounds-check $(CFLAGS)
endif

ifeq ($(shell uname -s), Darwin)
 FC=gfortran -fbounds-check $(CFLAGS)
endif

path=functions

OBJ := $(patsubst %.f,%.o,$(wildcard *.f functions/*.f))

all: libfbdsm_nr.a clean

libfbdsm_nr.a : $(OBJ)
	ar r $(ODIR)/libfbdsm_mynr.a $?

%.o: %.f
	$(FC) -c $< -o $@

$(path)/%.o: $(path)/%.f
	$(FC) -c $< -o $@

clean: 
	rm -fr *.o 


