#include <iostream>

#include "rm.h"						// RM Synthesis class

// casa includes (from /usr/local/include/casacore)
#include <casa/Utilities/DataType.h>
#include <tables/Tables/TiledFileAccess.h>
#include <lattices/Lattices/TiledShape.h>
#include <fits/FITS/fitsio.h>
//#include <fits/FITS/fits.h>
//#include <fits/FITS/BasicFITS.h>
//#include <fits/FITSImage.h>					// high-level FITSImage interface
#include <images/Images/ImageInterface.h>



// casa includes (from $LOFARSOFT)
//#include <casa/casa/Utilities/DataType.h>
//#include <tables/tables/Tables/TiledFileAccess.h>
//#include <lattices/lattices/Lattices/TiledShape.h>
//#include <fits/FITS/fits.h>
//#include <fits/FITS/BasicFITS.h>
//#include <images/images/ImageInterface.h>

using namespace std;
using namespace casa;

int main (int argc, char * const argv[]) {

	char *filename_Q;		// filename of FITS file to read
	Bool status;			// status of casa calls
	String casaerror;		// error message of casa calls
	Array<float> image_Q;	// Casa-array to read FITS file into


	if(argc<2)				// if no filenames was given, display usage/help (MUST change to 3!)
	{
		cout << "Usage: " << argv[0] << " <Q.fits> <U.fits" << endl;
		cout << "Computes the RM cubes in Q,U (and Q+iU) from two polarized input" << endl;
		cout << "FITS images." << endl;		
		return(0);
	}
	
	// Handle command line arguments
	filename_Q=(char*) calloc(strlen(argv[1]), sizeof(char));	// allocate memory
	strncpy(filename_Q, argv[1], strlen(argv[1]));				// copy to filename
	
	cout << "Filename_Q: " << filename_Q << endl;				// Debug output

	image_Q=ReadFITS(filename_Q, status, casaerror);			// read Q FITS image
	if(status)													// on error....
	{
		cout << casaerror << endl;								// ... display error message
		exit(0);
	}

	FitsInput fin("myfile.fits",FITS::Disk);    // open disk file for FITS input
	if (fin.err() == FitsIO::IOERR) {            // check if open succeeded
		cout << "Could not open FITS input\n";
		exit(0);
	}
	if (fin.rectype() == FITS::HDURecord &&      // test for primary array
	fin.hdutype() == FITS::PrimaryArrayHDU) {
	}


//    std::cout << "Read the file: " << filename << "!\n";		// Debug output
    return 0;
}
