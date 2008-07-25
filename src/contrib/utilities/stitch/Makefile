SHELL = /bin/sh
CC = g++

all: stitch

stitch: stitch.o
	$(CC) $< -L/aips++/Unstable/linux_gnu/lib/ -limages -lmir -lcomponents -lcoordinates -lfits -lmeasures \
	-llattices -lscimath -lscimath_f -ltables -lcasa -L/usr/local/lib -lwcs -llapack -lblas -lgfortran -g -o $@

stitch.o: stitch.cc
	$(CC) -Wall -I. -I/aips++/Unstable/code/include/ -DAIPS_NO_TEMPLATE_SRC -g -c -o $@ $<

clean:
	rm -f stitch *.o
