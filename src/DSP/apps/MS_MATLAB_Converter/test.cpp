#include <iostream>
#include <string>
#include <sstream>

#include "MSReader.h"
#include "MatWriter.h"

#include <casa/Arrays.h>

using namespace std;

int main(int argc, char ** argv){
	string msFileName("/dop156_0/jeurink/L2007_01575_SB0-1.MS");
	string outputDir = "testdir/";

	string polarizations[4];
	polarizations[0] = "xx";
	polarizations[1] = "xy";
	polarizations[2] = "yx";
	polarizations[3] = "yy";
	
	int freqStart = 0;
	int freqStop = 256;
	int timeSlotStart = 0;
	int timeSlotStop = 5;

	int bandID = 0;
	int polarizationID = 1;

	Cube<int> iCube;

	cout << "Rows: " << iCube.nrow() << endl;
	cout << "Columns: " << iCube.ncolumn() << endl;
	cout << "Planes: " << iCube.nplane() << endl;
	
	iCube.resize(10,10,10);
	
	cout << "Rows: " << iCube.nrow() << endl;
	cout << "Columns: " << iCube.ncolumn() << endl;
	cout << "Planes: " << iCube.nplane() << endl;
	
	
	MSReader* msReader = new MSReader(msFileName);
	int nAntenae = msReader->getNumberAntennae();

	//msReader->setCubeSize(nAntenae, freqStop-freqStart);
	MatWriter* matWriter = new MatWriter(nAntenae, nAntenae, freqStop - freqStart);

	cout << "Create ostringstream..." << flush ;
	ostringstream stm("");
	stm << "bla";
	cout << stm.str();
	return 0;
}
