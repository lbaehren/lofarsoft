/*!
  \file main.cpp

  \ingroup RM

  \brief RM-Synthesis test
 
  \author Sven Duscha
 
  \date 28.01.09.

  <h3>Synopsis</h3>
  RM-synthesis command line tool. Using algorithms provided in librm.a library. I/O of FITS format
  through casacore high-level functions.
*/

#include <iostream>

#include "rm.h"							// RM Synthesis class

// casa includes (from /usr/local/include/casacore)
#include <casa/Arrays.h>
#include <casa/Arrays/Array.h>
#include <casa/Logging/LogIO.h>
#include <casa/Utilities/DataType.h>
#include <images/Images/FITSImage.h>				// high-level FITSImage interface
#include <tables/Tables/TiledFileAccess.h>
#include <lattices/Lattices/TiledShape.h>
#include <lattices/Lattices/LatticeBase.h>
#include <lattices/Lattices/LatticeIterator.h>			// Iterator over lattices
#include <images/Images/ImageOpener.h>				// wrapper class for Image type opening
#include <images/Images/ImageFFT.h>
#include <images/Images/ImageStatistics.h>
#include <images/Images/ImageInterface.h>


#define _debug	

using namespace std;
using namespace casa;					// casa(core) namespace

int main (int argc, char * const argv[]) {

//	Bool status;					// status of casa calls
	String casaerror;				// error message of casa calls
	LatticeBase *lattice_Q;				// lattice for Q input image, why do we need LatticeBase here?

	Lattice<Float> *lattice_Q_float;		// lattice to determine type of lattice returned by imageOpen()
	Lattice<Complex> *lattice_Q_complex;
	Lattice<DComplex> *lattice_Q_dcomplex;

	float min=0, max=0;		// DEBUG: using minMax() function on image


	if(argc<2)				// if no filenames was given, display usage/help (MUST change to 3!)
	{
		cout << "Usage: " << argv[0] << " <Q.fits> <U.fits>" << endl;
		cout << "Computes the RM cubes in Q,U (and Q+iU) from two polarized input" << endl;
		cout << "FITS images." << endl;		
		return(0);
	}

	const string filename_Q=argv[1];	// get filename for Q image from command line
	//	const string filename_U=argv[2];	// get filename for U image from command line
	
	#ifdef _debug
	cout << "Filename_Q: " << filename_Q << endl;				// Debug output
	#endif

	FITSImage::registerOpenFunction();			// Register the FITS and Miriad image types.

	lattice_Q=ImageOpener::openImage (filename_Q);		// try open the file with generic casa function
	if(lattice_Q==NULL)					// on error	
	{
		cout << "Error opening " << filename_Q << endl;
		exit(0);
	}
		
	#ifdef _debug
	cout << lattice_Q->dataType() << endl;
	#endif

	switch(lattice_Q->dataType()){
		case TpFloat:
			lattice_Q_float=dynamic_cast<ImageInterface<Float>*>(lattice_Q);
			break;
		case TpDouble:
			lattice_Q_float=dynamic_cast<ImageInterface<Float>*>(lattice_Q);
			break;
		case TpComplex:
			lattice_Q_complex=dynamic_cast<ImageInterface<Complex>*>(lattice_Q);
			break;
		case TpDComplex:
			lattice_Q_dcomplex=dynamic_cast<ImageInterface<DComplex>*>(lattice_Q);
			break;
		default:
			throw AipsError("Image has an invalid data type");
			break;
	}
	
	std::vector<double> freq(10), lambdaSq(10);	// Test frequency and lambda squared conversions 
	
	for(unsigned int i=0; i <= lambdaSq.size(); i++)
	  lambdaSq[i]=i*100.0;

	// create a rm_cube from rm constructor
	new rm::rm_cube(10,10,10, 1.0, stdout);		
	
	lambdaSq=rm.freqToLambdaSq(freq);
	
	
	
	// Lattice and iteration over line of sight
	// create Lattice shape and iterator
//	const uInt cursorSize = lattice_Q_float->advisedMaxPixels();
//	const IPosition cursorShape = lattice_Q_float->niceCursorShape(cursorSize);
 	const IPosition cursorShape(2, lattice_Q_float->shape()(0), lattice_Q_float->shape()(1));
    
	int i=0;
	
	RO_LatticeIterator<Float> iter(*lattice_Q_float, cursorShape);
	for (iter.reset(); !iter.atEnd(); iter++) {
		minMax(min, max, iter.cursor());
		cout << i++ << " Min = " << min << " Max = " << max << iter.cursor() << endl; 
	}	

	cout << "Deleting lattice_Q" << endl;

	delete lattice_Q_float;		// delete the lattice of image Q
	delete &iter;				// delete iterator over image Q

	#ifdef _debug
	cout << "Finished: " << filename_Q << "!\n";	// Debug output
    #endif
	
	return 0;
}
