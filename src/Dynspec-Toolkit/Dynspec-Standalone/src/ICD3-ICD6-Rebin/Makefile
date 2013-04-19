CC=g++
CFLAGS=-O3 -s -Wall
LDFLAGS=-I /usr/local/hdf5/include/ -L /usr/local/hdf5/lib/ -lhdf5 -llofardal
EXEC=DynspecPart

all: $(EXEC)

DynspecPart: DynspecPart.o Stock_Write_Dynspec_Data_Part.o  Stock_Write_Dynspec_Metadata_Part.o Stock_Write_Root_Metadata_Part.o Reader_Dynspec_Part.o Reader_Root_Part.o 
	$(CC) -o DynspecPart DynspecPart.o Stock_Write_Dynspec_Data_Part.o Stock_Write_Dynspec_Metadata_Part.o Stock_Write_Root_Metadata_Part.o  Reader_Dynspec_Part.o Reader_Root_Part.o  $(LDFLAGS)

Stock_Write_Dynspec_Data_Part.o: Stock_Write_Dynspec_Data_Part.cpp 
	$(CC) -o Stock_Write_Dynspec_Data_Part.o -c Stock_Write_Dynspec_Data_Part.cpp $(CFLAGS) $(LDFLAGS)


Stock_Write_Dynspec_Metadata_Part.o: Stock_Write_Dynspec_Metadata_Part.cpp
	$(CC) -o Stock_Write_Dynspec_Metadata_Part.o -c Stock_Write_Dynspec_Metadata_Part.cpp $(CFLAGS) $(LDFLAGS)

Stock_Write_Root_Metadata_Part.o: Stock_Write_Root_Metadata_Part.cpp
	$(CC) -o Stock_Write_Dynspec_Metadata_Part.o -c Stock_Write_Dynspec_Metadata_Part.cpp $(CFLAGS) $(LDFLAGS)

Reader_Dynspec_Part.o: Reader_Dynspec_Part.o Stock_Write_Dynspec_Metadata_Part.h Stock_Write_Dynspec_Data_Part.h
	$(CC) -o Reader_Dynspec_Part.o -c Reader_Dynspec_Part.cpp $(CFLAGS) $(LDFLAGS)

Reader_Root_Part.o: Reader_Root_Part.o tock_Write_Root_Metadata_Part.h
	$(CC) -o Reader_Root_Part.o -c Reader_Root_Part.cpp  $(CFLAGS) $(LDFLAGS)

DynspecPart.o: DynspecPart.cpp Stock_Write_Dynspec_Data_Part.h  Stock_Write_Dynspec_Metadata_Part.h Stock_Write_Root_Metadata_Part.h Reader_Dynspec_Part.h Reader_Root_Part.h 
	$(CC) -o DynspecPart.o -c DynspecPart.cpp $(CFLAGS) $(LDFLAGS)

clean:
	rm -rf *.o

mrproper: clean
	rm -rf $(EXEC)


