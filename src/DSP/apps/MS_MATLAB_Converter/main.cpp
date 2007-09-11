#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
//#include <sstream>
#include <iomanip>
//#include <unistd.h>

#include <sys/stat.h>
#include <sys/types.h>

#include <casa/Arrays.h>
#include <casa/aipstype.h>
#include <casa/complex.h>

#include "SimpleOpt.h"

#include "MSReader.h"
#include "MatWriter.h"
#include "MSInfo.h"

# define TCHAR		char
# define _T(x)		x
# define _tprintf	printf

using namespace std;

enum FillMode { FillFrequency, FillTime };

void countZeros(const Cube<complex<float> >& cube);

CSimpleOpt::SOption cmdOptions[] = {
	{ 0, _T("-?"), SO_NONE },
	{ 0, _T("-h"), SO_NONE },
	{ 0, _T("-help"), SO_NONE },
	{ 1, _T("-fstart"), SO_REQ_CMB },
	{ 2, _T("-fend"), SO_REQ_CMB },
	{ 3, _T("-tstart"), SO_REQ_CMB },
	{ 4, _T("-tend"), SO_REQ_CMB },
	{ 5, _T("-polarization"), SO_REQ_CMB  },
	{ 6, _T("-band"), SO_REQ_CMB  },
	{ 7, _T("-frequency"), SO_NONE  },
	{ 7, _T("-f"), SO_NONE  },
	{ 8, _T("-time"), SO_NONE  },
	{ 8, _T("-t"), SO_NONE  },
	{ 9, _T("-all"), SO_NONE },
	SO_END_OF_OPTIONS
};

void ShowUsage()
{
	cout << "Usage: ./MSToMatlab {ARGS} directory_input_measurementset directory_output_dir"<< endl;
	cout << "Arguments available: " << endl;
	cout << setiosflags(ios::left);
	cout << setw(20) << "-help -h -?"  	<< setw(60)  	<< "Displays this help message." << endl;
	cout << setw(20) << "-fstart"				<< setw(60)   <<  "Frequency bin start. " << endl;
	cout << setw(20) << "-fend"  				<< setw(60)   <<  "Frequency bin end." << endl;
	cout << setw(20) << "-tstart"				<< setw(60)   << setiosflags(ios::left)<< "Time slot start. " << endl;
	cout << setw(20) << "-tend"  				<< setw(60)   << setiosflags(ios::left)<< "Time slot end." << endl;
	cout << setw(20) << "-all"  				<< setw(60)   << setiosflags(ios::left)<< "All time slots." << endl;
	cout << setw(20) << "-polarization"	<< setw(60)		<< setiosflags(ios::left)<< "Polarization ID." << endl;
	cout << setw(20) << "-band"  				<< setw(60)   << setiosflags(ios::left)<< "Band ID." << endl;
	cout << setw(20) << "-frequency"		<< setw(60)   << setiosflags(ios::left)<< "Cube output is in R(i,j,t). (real slow)" << endl;
	cout << setw(20) << "-time"					<< setw(60)   << setiosflags(ios::left)<< "Cube output is in R(i,j,f). (default)" << endl;

	cout << endl << "It is possible to abbreviate the arguments to respectivelly to" << endl;
	cout << "-h -fs= -fe= -ts= te= -p= -b=" << endl << endl;
	
	cout << "Example: from timeslot 0 to timeslot 5" << endl;
	cout << "./MSToMatlab -tstart=0 -tend=5 /measurementsets/L2007_01575_SB0-1.MS ./outputDir" << endl <<endl;
	cout << "Or with abbreviated arguments: " << endl ;
	cout << "./MSToMatlab -ts=0 -te=5 /measurementsets/L2007_01575_SB0-1.MS ./outputDir" << endl;
}


int main(int argc, char ** argv){
	string polarizations[4];
	polarizations[0] = "xx";
	polarizations[1] = "xy";
	polarizations[2] = "yx";
	polarizations[3] = "yy";

	FillMode fillMode = FillTime;

	int error = 0;
	bool foundError=false;

	int freqStart = 	 0;
	int freqStop = 		-1; // if after parsing of cmd args this is still -1 than the max frequency will be put in.
	int timeSlotStart =0; 
	int timeSlotStop = 1; // default timeslot stop if not overrided

	int bandID = 0;
	int polarizationID = 0;

	string msFileName;
	string outputDir = ".";

	bool allTimeSlots=false;

	CSimpleOpt args(argc, argv, cmdOptions);

	while (args.Next()) {
		if (args.LastError() != SO_SUCCESS) {
			TCHAR * pszError = _T("Unknown error");
			stringstream temp("");
			string tempString("");
			switch (args.LastError()) {
				case SO_OPT_INVALID:
					pszError = _T("Unrecognized option");
					break;
				case SO_OPT_MULTIPLE:
					pszError = _T("Option matched multiple strings");
					break;
				case SO_ARG_INVALID:
					pszError = _T("Option does not accept argument");
					break;
				case SO_ARG_INVALID_TYPE:
					pszError = _T("Invalid argument format");
					break;
				case SO_ARG_MISSING:
					temp << "Required argument is missing, try " << args.OptionText() << "='value'" ;
					tempString = temp.str();
					pszError = _T((char*)tempString.c_str());
					break;
				case SO_SUCCESS:
					pszError = NULL;
			}
			_tprintf(_T("%s: '%s' (use --help to get command line help)\n"),pszError, args.OptionText());
		}

		if (args.LastError() == SO_SUCCESS) {
			if (args.OptionId() == 0) {
				ShowUsage();
				return 0;
			}
			istringstream iss;
			if(args.OptionArg() != NULL){
				iss.str(args.OptionArg());
			}
			switch(args.OptionId())
			{
				case 1:
					iss >> freqStart;
					break;
				case 2:
					iss >> freqStop;
					break;
				case 3:
					iss >> timeSlotStart;
					break;
				case 4:
					iss >> timeSlotStop;
					break;
				case 5:
					iss >> polarizationID;
					break;
				case 6:
					iss >> bandID;
					break;
				case 7:
					fillMode = FillFrequency;
					break;
				case 8:
					fillMode = FillTime;
					break;
				case 9:
					allTimeSlots = true;
					break;
				default:
					cout << "Invalid argument: " <<  args.OptionText();
					return 1;
			}
		}
	}

	switch(args.FileCount())
	{
		case 0:
			cout << "Measurement set directory is missing..." << endl;
			return -1;
			break;
		case 1:
			// only MS dir is given
			msFileName = args.File(0);
			// output dir is .
			break;
		case 2:
			msFileName = args.File(0);
			outputDir = args.File(1);
			break;
		default:
			cout << "Only two file or directory arguments are parsed " << args.File(2) << " is being ignored." << endl;
			break;
			
	}

	if(outputDir.at(outputDir.size()-1) != '/')
	{
		outputDir.append("/");
	}
	if(fillMode == FillTime)
	{
		cout << "Third dimension of cube is time." << endl;
	}
	else if(fillMode == FillFrequency)
	{
		cout << "Third dimension of cube is frequency." << endl;
	}

	cout << endl;

	// create directory
	int status = mkdir( outputDir.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH );
	if(status == -1)
	{
		cout << "Directory " << outputDir << " already exists or no rights to create."  << endl;
	}
	
	MSReader msReader(msFileName);
	
	// To tell MSReader witch band we are going to use.. needs some redesigning
	// It has to be called before msReader.getTimeCube and getFrequencyCube are called.
	msReader.setBandInfo(bandID);
	
	MSInfo tempMSInfo = msReader.getMSInfo();

	if(allTimeSlots)
	{
		timeSlotStop = msReader.getNTimeSlots();
	}

	if(freqStop == -1){ // the default is to copy the full frequency spectrum
		freqStop = tempMSInfo.nChannels;
	}

	cout << "Frequency range " << freqStart << "-" << freqStop << endl;
	cout << "Time range "  << timeSlotStart << "-" << timeSlotStop << endl;
	cout << "Polarization " << polarizations[polarizationID] << endl;
	cout << "Selected band " << bandID << endl;
	
	MatWriter matWriter;

	if(fillMode == FillTime){

		bool foundError = false;
		int error=0;
	
		stringstream stm("");
		string varName;
	
		cout << "For loop start "<< timeSlotStart << flush;
		cout << " For loop stop "<< timeSlotStop << flush << endl;

		for(int timeSlot =timeSlotStart; timeSlot <timeSlotStop && !foundError; timeSlot++)
		{
			stm << "R";
			stm << polarizations[polarizationID];
			//stm << "b";
			//stm << bandID;
			stm << "t";
			stm << setw(4) << setfill('0') << timeSlot;
			varName = stm.str();
			
			stm << ".mat";
			//cout << "Output file name: " << stm.str() << endl;
	
			cout << "Get cube " << timeSlot << " from ms..." << flush ;
			Cube<complex<float> > cube = msReader.getTimeCube(timeSlot, bandID, polarizationID, freqStart, freqStop);
			cout << "Done." << flush << endl;
			
			string fileName(outputDir+ stm.str());
			stm.str("");
			
			error = matWriter.openFile(fileName);
			if(error!=0){
				foundError = true;
			}
			error = matWriter.writeCube(cube);
			if(error!=0){
				foundError = true;
			}
			error = matWriter.closeFile(varName);
			if(error!=0){
				foundError = true;
			}
			cout << endl;
		}
	}
	else if(fillMode == FillFrequency)
	{
		stringstream stm("");
		string varName;
	
		cout << "Loop for frequencies: "<< freqStart << flush;
		cout << " - "<< freqStop << flush << endl;
		for(int freq =freqStart; freq < freqStop && !foundError; freq++)
		{
			stm << "R";
			stm << polarizations[polarizationID];
			//stm << "b";
			//stm << bandID;
			stm << "f";
			stm << setw(4) << setfill('0') << freq;
			varName = stm.str();
			stm << ".mat";
			cout << "Output file name: " << stm.str() << endl;
	
			cout << "Get cube " << freq << " from ms..." << flush ;
			Cube<complex<float> > cube = msReader.getFrequencyCube(freq, bandID, polarizationID, timeSlotStart, timeSlotStop);
			uInt nAnntenae1 = cube.nrow();
			uInt nAnntenae2 = cube.ncolumn();
			uInt nFreq = cube.nplane();

			cout << "Cube dimensions: " << nAnntenae1 << "," << nAnntenae2 << "," << nFreq << flush << endl;

			cout << "Done." << flush << endl;
			
			string fileName(outputDir+ stm.str());
			stm.str("");
			
			error = matWriter.openFile(fileName);
			if(error!=0){
				foundError = true;
			}
			error = matWriter.writeCube(cube);
			if(error!=0){
				foundError = true;
			}
			error = matWriter.closeFile(varName);
			if(error!=0){
				foundError = true;
			}			
			cout << endl;
		}
	}
	String infoFilename("info.mat");
	String infoFileFullName(outputDir);
	
	MSInfo msInfo = msReader.getMSInfo();
	
	infoFileFullName.append(infoFilename);

	matWriter.writeInfoFile(infoFileFullName, msInfo);
	
	if(foundError)
	{
		cout << "Found error. Quit... " << endl;
		return -1;
	}
	return 0;
}

