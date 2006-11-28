C++BIN := /usr/bin/c++
C++DEFINES := -DAIPS_STDLIB -DAIPS_DARWIN -DAIPS_BIG_ENDIAN -DNATIVE_EXCP -DAIPS_NO_TEMPLATE_SRC
C++OPTLEVEL := -O3
C++FLAGS := -fPIC  -fexceptions -pipe -Wall -Wno-non-template-friend -Woverloaded-virtual -Wcast-align -Wno-comment -pipe -fno-implicit-templates -Wreturn-type -Wimplicit -fPIC

FILES := `ls *.cc`

objects: Functional.o Math.o Random.o

Functional.o: Functional.cc
	$(C++BIN) $(C++DEFINES)  -I. -I../.. $(C++OPTLEVEL) $(C++FLAGS) -c Functional.cc

Math.o: Math.cc
	$(C++BIN) $(C++DEFINES)  -I. -I../.. $(C++OPTLEVEL) $(C++FLAGS) -c Math.cc

Random.o: Random.cc
	$(C++BIN) $(C++DEFINES)  -I. -I../.. $(C++OPTLEVEL) $(C++FLAGS) -c Random.cc

clean:
	rm -f *~ *.o
