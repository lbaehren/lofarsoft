#include <iostream>

#include "rm.h"						// RM Synthesis class

// casa includes (from /usr/local/include/casacore)
#include <casa/Arrays.h>
#include <casa/Arrays/Array.h>
#include <casa/Utilities/DataType.h>
#include <images/Images/FITSImage.h>					// high-level FITSImage interface
#include <tables/Tables/TiledFileAccess.h>
#include <lattices/Lattices/TiledShape.h>
#include <images/Images/ImageOpener.h>
//#include <fits/FITS/BasicFITS.h>
//#include <images/Images/PagedImage.h>
//#include <images/Images/ImageFITSConverter.h>

//#include <images/Images/ImageInterface.h>


#define _debug	

using namespace std;
using namespace casa;

int main (int argc, char * const argv[]) {

	char *filename_Q, *filename_U;		// filename of FITS file to read
//	Bool status;						// status of casa calls
	String casaerror;					// error message of casa calls
	LatticeBase *image_Q;				// lattice for Q input image

	if(argc<2)				// if no filenames was given, display usage/help (MUST change to 3!)
	{
		cout << "Usage: " << argv[0] << " <Q.fits> <U.fits>" << endl;
		cout << "Computes the RM cubes in Q,U (and Q+iU) from two polarized input" << endl;
		cout << "FITS images." << endl;		
		return(0);
	}

	/*
	// Handle command line arguments
	if( (filename_Q=(char*) calloc(strlen(argv[1]), sizeof(char))+10) )	// only if we got memory allocated
		strncpy(filename_Q, argv[1], strlen(argv[1]));					// copy to filename for Q Image
	if( (filename_U=(char*) calloc(strlen(argv[2]), sizeof(char))+1) )	// only if we got memory allocated
		strncpy(filename_U, argv[2], strlen(argv[2]));					// copy to filename for U Image
	*/

	
	#ifdef _debug
	cout << "Filename_Q: " << filename_Q << endl;				// Debug output
	#endif



	// Register the FITS and Miriad image types.
	FITSImage::registerOpenFunction();

	image_Q=ImageOpener::openImage(argv[1]);		// try open the file with generic casa function
	if(image_Q==NULL)								// on error	
	{
		cout << "Error opening " << filename_Q << endl;
	
	}


	#ifdef _debug
	cout << "Finished: " << filename_Q << "!\n";	// Debug output
    #endif
	
	return 0;
}
