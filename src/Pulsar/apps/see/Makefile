PREFIX = /usr/local/src/psp
CC = g++
CFLAGS = -c -g -O3 -D_GNU_SOURCE -D_FILE_OFFSET_BITS=64 -Wno-deprecated
INCL = -I./
X_CFLAGS = -I/usr/X11R6/include
X_LIBS = -L/usr/X11R6/lib
X_EXTRA_LIBS = 
X_PRE_LIBS = -lSM -lICE
LIBS = -lX11 -lXt $(X_LIBS)
all: see strip clean

see: see.o regard.o mem.o util.o gifsave.o
	$(CC) see.o regard.o mem.o util.o gifsave.o -o see $(X_PRE_LIBS) $(LIBS) $(X_EXTRA_LIBS)	

regard.o: regard.cpp see.hpp ps.hpp defines.hpp options.hpp gifsave.h
	$(CC) $(CFLAGS) $(X_CFLAGS) $(INCL) regard.cpp
	
see.o: see.cpp see.hpp opt.hpp defines.hpp options.hpp
	$(CC) $(CFLAGS) $(INCL) see.cpp
	
mem.o: mem.cpp see.hpp defines.hpp options.hpp
	$(CC) $(CFLAGS) $(INCL) mem.cpp

util.o: util.cpp
	$(CC) $(CFLAGS) util.cpp

gifsave.o: gifsave.cpp gifsave.h
	$(CC) $(CFLAGS) gifsave.cpp

strip:
	strip see

clean:
	rm -rf *.o *~

install:
	cp see ${PREFIX}/bin
