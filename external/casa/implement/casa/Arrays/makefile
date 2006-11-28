C++BIN := /usr/bin/c++
C++DEFINES := -DAIPS_STDLIB -DAIPS_DARWIN -DAIPS_BIG_ENDIAN -DNATIVE_EXCP -DAIPS_NO_TEMPLATE_SRC
C++OPTLEVEL := -O3
C++FLAGS := -fPIC  -fexceptions -pipe -Wall -Wno-non-template-friend -Woverloaded-virtual -Wcast-align -Wno-comment -pipe -fno-implicit-templates -Wreturn-type -Wimplicit -fPIC

FILES := `ls *.cc`

objects: Array.o Array2.o ArrayBase.o ArrayIO.o ArrayError.o

Array.o: Array.cc
	$(C++BIN) $(C++DEFINES)  -I. -I../.. $(C++OPTLEVEL) $(C++FLAGS) -c Array.cc

Array2.o: Array2.cc
	$(C++BIN) $(C++DEFINES)  -I. -I../.. $(C++OPTLEVEL) $(C++FLAGS) -c Array2.cc

ArrayBase.o: ArrayBase.cc
	$(C++BIN) $(C++DEFINES)  -I. -I../.. $(C++OPTLEVEL) $(C++FLAGS) -c ArrayBase.cc

ArrayIO.o: ArrayIO.cc
	$(C++BIN) $(C++DEFINES)  -I. -I../.. $(C++OPTLEVEL) $(C++FLAGS) -c ArrayIO.cc

ArrayError.o: ArrayError.cc
	$(C++BIN) $(C++DEFINES)  -I. -I../.. $(C++OPTLEVEL) $(C++FLAGS) -c ArrayError.cc

clean:
	rm -f *~ *.o
