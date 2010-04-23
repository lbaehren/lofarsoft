CFLAGS=-L. -lftd2xx -Wl,-rpath /usr/local/lib

test_lasa1: test_lasa1.c gps_data.c ftdstuf.c cmdlib.c Auger.h Makefile
	g++ -O  -o test_lasa1  $(CFLAGS) -Wall -I/usr/local/include \
	test_lasa1.c gps_data.c ftdstuf.c cmdlib.c \
	-L/usr/local/lib -lfftw3 -lm

test_Event: test_Event.c Makefile
	g++ -O  -o test_Event -Wall -I/usr/local/include \
	test_Event.c \
	-L/usr/local/lib -lm

main_lasa: main_lasa.c  Makefile
	g++ -O -o main_lasa $(CFLAGS) -Wall -I/usr/local/include \
	 main_lasa.c \
	-L/usr/local/lib -lfftw3 -lm
	
clean:
	rm -f *.o
