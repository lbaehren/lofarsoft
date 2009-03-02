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
#include <casa/Logging/LogIO.h>
#include <casa/Utilities/DataType.h>
#include <images/Images/FITSImage.h>					// high-level FITSImage interface
#include <tables/Tables/TiledFileAccess.h>
#include <lattices/Lattices/TiledShape.h>
#include <lattices/Lattices/LatticeBase.h>
#include <images/Images/ImageOpener.h>					// wrapper class for Image type opening
#include <images/Images/ImageFFT.h>
//#include <fits/FITS/BasicFITS.h>
//#include <images/Images/PagedImage.h>
//#include <images/Images/ImageFITSConverter.h>
#include <images/Images/ImageStatistics.h>
#include <images/Images/ImageInterface.h>


#define _debug	

using namespace std;
using namespace casa;

int main (int argc, char * const argv[]) {

	Bool status;						// status of casa calls
	String casaerror;					// error message of casa calls
	LatticeBase *lattice_Q;				// lattice for Q input image, why do we need LatticeBase here?
	LatticeBase *itsLattice;			// lattice to determine type of lattice returned by imageOpen()
	Lattice<Float> *itsImageFloat_Q;
//	Array<double> image_Q;				// FITS for input image

	if(argc<2)				// if no filenames was given, display usage/help (MUST change to 3!)
	{
		cout << "Usage: " << argv[0] << " <Q.fits> <U.fits>" << endl;
		cout << "Computes the RM cubes in Q,U (and Q+iU) from two polarized input" << endl;
		cout << "FITS images." << endl;		
		return(0);
	}

	const string filename_Q=argv[1];
	char *filename_U=argv[2];
	
	#ifdef _debug
	cout << "Filename_Q: " << filename_Q << endl;				// Debug output
	#endif

	FITSImage::registerOpenFunction();				// Register the FITS and Miriad image types.

//	MaskSpecifier::MaskSpecifier mask_Q;			// create a mask (needed for openFITSImage)
//	lattice_Q=FITSImage::openFITSImage(filename_Q, mask_Q);

	
	lattice_Q=ImageOpener::openImage (filename_Q);	// try open the file with generic casa function
	if(lattice_Q==NULL)								// on error	
	{
		cout << "Error opening " << filename_Q << endl;
		exit(0);
	}
		
	// Lattice and iteration over line of sight
	itsLattice=lattice_Q;
	switch(lattice_Q->dataType()){
		case TpFloat:
			lattice_Q=dynamic_cast<ImageInterface<Float>*>(lattice_Q);
		case TpDouble:
			lattice_Q=dynamic_cast<ImageInterface<Float>*>(lattice_Q);
		default:
			throw AipsError("Image has an invalid data type");
	}
	
	
	//floatlattice_Q=dynamic_cast<ImageInterface<Float>*>(lattice_Q);
	
	cout << itsImageFloat_Q->dataType() << endl;
	
	// the shape function is forced upon us by the Lattice base class
//	uInt rowLength = image_Q.shape()(0);
//    IPosition rowShape(image_Q.ndim());
//    rowShape = 1; rowShape(0) = rowLength;
//    Float sumPix = 0;
 //   RO_LatticeIterator<Float> iter(image_Q, rowShape);
 //   while(!iter.atEnd()){
 //     sumPix += sum(iter.vectorCursor());
 //     iter++;
 //   }

	cout << "Deleting lattice_Q" << endl;

	delete lattice_Q;	// delete the lattice of image Q

	#ifdef _debug
	cout << "Finished: " << filename_Q << "!\n";	// Debug output
    #endif
	
	return 0;
}

