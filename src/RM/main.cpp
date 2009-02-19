/*!
  \file main.cpp

  \ingroup RM

  \brief RM-Synthesis test
 
  \author Sven Duscha
 
  \date 28.01.09.

  <h3>Synopsis</h3>

*/

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

//	char *filename_Q=NULL, *filename_U=NULL;		// filename of FITS file to read
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

	char *filename_Q=argv[1];
	char *filename_U=argv[2];
	
	#ifdef _debug
	cout << "Filename_Q: " << filename_Q << endl;				// Debug output
	#endif

	FITSImage::registerOpenFunction();				// Register the FITS and Miriad image types.

	image_Q=ImageOpener::openImage(argv[1]);		// try open the file with generic casa function
	if(image_Q==NULL)								// on error	
	{
		cout << "Error opening " << filename_Q << endl;
		exit(0);
	}
	

	image_Q = dynamic_cast<ImageInterface<Float>*(image_Q);
	// the shape function is forced upon us by the Lattice base class

  uInt rowLength = image_Q.shape()(0);
      IPosition rowShape(image_Q.ndim());
      rowShape = 1; rowShape(0) = rowLength;
      Float sumPix = 0;
      RO_LatticeIterator<Float> iter(image_Q, rowShape);
      while(!iter.atEnd()){
        sumPix += sum(iter.vectorCursor());
        iter++;
      }



	#ifdef _debug
	cout << "Finished: " << filename_Q << "!\n";	// Debug output
    #endif
	
	return 0;
}

