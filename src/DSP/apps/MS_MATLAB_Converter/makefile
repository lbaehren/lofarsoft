AIPSDIR = 
AIPSLIBRARIES= -lms -lmeasures -ltables -lscimath -lcasa 
MATLABLIBRARIES = -lmat #-lmx -lut -licudata -licui18n -licuuc -lustdio
INCLUDES = -I/dop156_0/jeurink/V19.1635_listfen_gcc410/code/include/ -I/usr/local/matlab-14/extern/include
LDFLAGS =  -L/dop156_0/jeurink/V19.1635_listfen_gcc410/linux_gnu/lib/ -L/dop156_0/jeurink/glnx86/ -L/usr/lib/ #-L/dop156_0/jeurink/userlib/
VERSIONO = /dop156_0/jeurink/V19.1635_listfen_gcc410/linux_gnu/lib/version.o

PROGRAM= MSToMatlab
OBJECTS= MSReader.o main.o

CXXFLAGS+= -D AIPS_NO_TEMPLATE_SRC -D AIPS_ARRAY_INDEX_CHECK -W -Wall -Wno-non-template-friend -Wno-unknown-pragmas -Woverloaded-virtual

all: $(PROGRAM)

MSToMatlab: MSReader.o MatWriter.o Antenna.o MSInfo.o main.o
	/usr/bin/g++ -gddb $(INCLUDES) $(CXXFLAGS) -o $(PROGRAM) MatWriter.o MSReader.o Antenna.o MSInfo.o main.o \
 $(VERSIONO)\
 $(LDFLAGS)\
 -Wl,-Bstatic $(AIPSLIBRARIES)\
 -Wl,-Bdynamic $(MATLABLIBRARIES)\
 /usr/lib/libstdc++.so

test: MSReader.o MatWriter.o test.o
	/usr/bin/g++ $(INCLUDES) $(CXXFLAGS) -o test MatWriter.o MSReader.o test.o \
 $(VERSIONO)\
 $(LDFLAGS)\
 $(AIPSLIBRARIES)\
 $(MATLABLIBRARIES)\
 /usr/lib/libstdc++.so
 
MSReader.o: MSReader.cpp MSReader.h
	/usr/bin/g++ -c $(INCLUDES) $(CXXFLAGS) -o MSReader.o MSReader.cpp

MatWriter.o: MatWriter.cpp MatWriter.h
	/usr/bin/g++ -c $(INCLUDES) $(CXXFLAGS) -o MatWriter.o MatWriter.cpp

Antenna.o: Antenna.cpp Antenna.h
	/usr/bin/g++ -c $(INCLUDES) $(CXXFLAGS) -o Antenna.o Antenna.cpp
	
MSInfo.o: MSInfo.cpp MSInfo.h
	/usr/bin/g++ -c $(INCLUDES) $(CXXFLAGS) -o MSInfo.o MSInfo.cpp

main.o: main.cpp 
	/usr/bin/g++ -c $(INCLUDES) $(CXXFLAGS) -o main.o main.cpp


clean:
	rm -f *.o $(PROGRAM)


#18 april 2007

#MSToMatlab: MSReader.o MatWriter.o main.o
#	/usr/bin/g++ -gddb $(INCLUDES) $(CXXFLAGS) -o $(PROGRAM) MatWriter.o MSReader.o main.o \
# $(VERSIONO)\
# $(LDFLAGS)\
# -Wl,-Bstatic $(AIPSLIBRARIES)\
# -Wl,-Bdynamic $(MATLABLIBRARIES)\
# /usr/lib/libstdc++.so

#AIPSDIR =
#AIPSLIBRARIES= -lgfortran -lm -lc -lblas -llapack -lms -lfits -lmeasures -ltables -lscimath -lcasa -lcfitsio
# also working: AIPSLIBRARIES= -lms -lmeasures -ltables -lscimath -lcasa
#MATLABLIBRARIES = -lut -licudata -licui18n -licuuc -lustdio -lmat -lmx
# also working MATLABLIBRARIES = -lmat 
#INCLUDES = -I/dop156_0/jeurink/V19.1635_listfen_gcc410/code/include/ -I/usr/local/matlab-14/extern/include
#LDFLAGS = -L/dop156_0/jeurink/V19.1635_listfen_gcc410/linux_gnu/lib/ -L/dop156_0/jeurink/glnx86/ -L/usr/lib/ #-L/dop156_0/jeurink/userlib/
#VERSIONO = /dop156_0/jeurink/V19.1635_listfen_gcc410/linux_gnu/lib/version.o

#PROGRAM= MSToMatlab
#OBJECTS= MSReader.o main.o

#end 18 april 2007

# werkt op dop 77
#AIPSDIR =
#AIPSLIBRARIES= -lgfortran -lm -lc -lblas -llapack -lms -lfits -lmeasures -ltables -lscimath -lcasa -lcfitsio
#MATLABLIBRARIES = -licudata -licui18n -licuuc -lustdio -lmat -lmx
#INCLUDES = -I/dop156_0/jeurink/V19.1635_listfen_gcc410/code/include/ -I/usr/local/matlab-14/extern/include
#LDFLAGS = -L/dop156_0/jeurink/V19.1635_listfen_gcc410/linux_gnu/lib/ -L/usr/local/matlab-14/bin/glnx86 -L/usr/lib
#VERSIONO = /dop156_0/jeurink/V19.1635_listfen_gcc410/linux_gnu/lib/version.o

#	g++ -m32 -v -D AIPS_NO_TEMPLATE_SRC -pthread -with-included-casa -Wall -Wno-non-template-friend -Wno-unknown-pragmas -Woverloaded-virtual -g -L/dop77_1/renting/jeurink/glnx86 -L/aips++/daily/dop10_gnu/lib -I/dop77_1/renting/jeurink/include -I/aips++/daily/code/include -licudata -licui18n -licuuc -lustdio -lmat -lmx -ltables -lcasa matlabTest.cpp -o matlabTest

#matlab compileerd
#all: g++ -L../../glnx86 -L/aips++/V19.1488_wop30_gcc320/linux_gnu/lib  -I../../include/ -I/aips++/V19.1488_wop30_gcc320/code/include -lmat -lut -lmx -licudata -licui18n -licuuc -lustdio -lz matlabTest.cpp -o matlabTest


#g++ -m32 -L/usr/local/matlab-14/bin/glnx86 -L/aips++/V19.1488_wop30_gcc320/linux_gnu/lib -L/usr/lib/ -I/usr/local/matlab-14/extern/include -I/aips++/V19.1488_wop30_gcc320/code/include matlabTest.cpp -o matlabTest

#-Wl,-melf_i386
#-lfits -lmeasures -ltables -lscimath -lcasa -lmx -lmat ,march=i386 

#	g++ -B /usr/local/matlab-14/extern/lib/glnxa64/ -I /usr/local/matlab-14/extern/include matlabTest.cpp -o matlabTest

#	matlabTest.o: matlabTest.cpp /usr/local/matlab-14/extern/include/mat.h 

