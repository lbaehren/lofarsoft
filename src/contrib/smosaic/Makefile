#### Build rules for smosaic

### locations of include files & libraries of casacore and cfitsio
# these are for my mac
CASACORE_INC += -I/opt/casacore/include/casacore -I/sw/include
CASACORE_LIB += -L/opt/casacore/lib -L/sw/lib
# and for Leiden network installation
CASACORE_INC += -I/disks/strw3/usov/casacore3/include/casacore -I/disks/strw3/usov/xtra/include -I/usr/include/cfitsio
CASACORE_LIB += -L/disks/strw3/usov/casacore3/lib -L/disks/strw3/usov/xtra/lib   -Wl,--rpath -Wl,/disks/strw3/usov/xtra/lib

CC = g++

### now build rules
CFLAGS = ${CASACORE_INC}
LDFLAGS = ${CASACORE_LIB} -lcasa_images -lcasa_coordinates -lcasa_measures -lcasa_fits -lcasa_lattices -lcasa_tables -lcasa_scimath -lcasa_casa -lwcs -lcfitsio -llapack -lblas -lhdf5


smosaic: smosaic.cc
	${CC} ${CFLAGS} smosaic.cc  ${LDFLAGS} -o smosaic
