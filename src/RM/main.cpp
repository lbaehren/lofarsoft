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

#include <iostream>						// STL iostream

#include "rm.h"							// RM Synthesis class
#include "rmCube.h"						// RM Cube class

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

	double bzero=0;					// FITS scaling
	double bscale=1;
   	
	unsigned int i=0;				// loop variable

	// Q Image
	LatticeBase *lattice_Q;				// lattice for Q input image, why do we need LatticeBase here?
	
	Lattice<Float> *lattice_Q_float;		// lattice to determine type of lattice returned by imageOpen()
	Lattice<Complex> *lattice_Q_complex;
	Lattice<DComplex> *lattice_Q_dcomplex;
	
	// U Image
	LatticeBase *lattice_U;
	Lattice<Float> *lattice_U_float;		// lattice to determine type of lattice returned by imageOpen()
	Lattice<Complex> *lattice_U_complex;
	Lattice<DComplex> *lattice_U_dcomplex;
	

	float min=0, max=0;		// DEBUG: using minMax() function on image
	vector<double> phi;		// Faraday depths to probe for
	complex<double> rmpol;		// polarized intensity at Faraday depth probed for


	if(argc<3)	// if no filenames was given, display usage/help (MUST change to 3!)
	{
		cout << "Usage: " << argv[0] << " <Q.fits> <U.fits>" << endl;
		cout << "Computes the RM cubes in Q,U (and Q+iU) from two polarized input" << endl;
		cout << "FITS images." << endl;		
		return(0);
	}

	const string filename_Q=argv[1];	// get filename for Q image from command line
	const string filename_U=argv[2];	// get filename for U image from command line
	
	#ifdef _debug
	cout << "Filename_Q: " << filename_Q << endl;		// Debug output
	cout << "Filename_U: " << filename_U << endl;
	#endif

	FITSImage::registerOpenFunction();			// Register the FITS and Miriad image types.

	lattice_Q=ImageOpener::openImage (filename_Q);		// try open the file with generic casa function
	if(lattice_Q==NULL)					// on error	
	{
		cout << "Error opening " << filename_Q << endl;
		exit(0);
	}
		
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

	// U-Image
	lattice_U=ImageOpener::openImage (filename_U);		// try open the file with generic casa function
	if(lattice_U==NULL)					// on error	
	{
		cout << "Error opening " << filename_Q << endl;
		exit(0);
	}
		
	switch(lattice_U->dataType()){
		case TpFloat:
			lattice_U_float=dynamic_cast<ImageInterface<Float>*>(lattice_U);
			break;
		case TpDouble:
			lattice_U_float=dynamic_cast<ImageInterface<Float>*>(lattice_U);
			break;
		case TpComplex:
			lattice_U_complex=dynamic_cast<ImageInterface<Complex>*>(lattice_U);
			break;
		case TpDComplex:
			lattice_U_dcomplex=dynamic_cast<ImageInterface<DComplex>*>(lattice_U);
			break;
		default:
			throw AipsError("Image has an invalid data type");
			break;
	}

	
	/*
	// Lattice and iteration over line of sight
	// create Lattice shape and iterator
 	const uInt cursorSize = lattice_Q_float->advisedMaxPixels();
	const IPosition cursorShape = lattice_Q_float->niceCursorShape(cursorSize);
 //	const IPosition cursorShape(2, lattice_Q_float->shape()(0), lattice_Q_float->shape()(1));
	*/
	
	
	// Traverse through Q Image
	IPosition latticeShape=(*lattice_Q_float).shape(); // get lattice' shape
	
	// Try to determine frequency axis from header keyword
	
	int chans=latticeShape(2);
	const IPosition cursorShape(1,chans);		
	IPosition axisPath(3,0,1,2);			// walk along frequency axis


	// create a rmCube object with an associated two dimensional buffer
	rmCube FaradayCube(latticeShape[0], latticeShape[0], 100, 5.0);
	FaradayCube.setFaradayDepths(-100, 100, 10); // Set up Faraday depths to be probed
	//FaradayCube.createBufferCube();		     // create buffer for whole RM cube


	// TEST: inverseFourier RM-Synthesis
	// compute 30 pts along 0 to 29
	vector<complex<double> > f(30);
	vector<complex<double> > F(30);
	vector<double> weights(30);	// weights
	vector<double> freq(30);
	vector<double> delta_freq(30);


	// Generate Rectangular pulse in f (real)
	for(i=0; i<=13; i++)
	{
	  f[i]=1.0;
	}
	f[14]=10.0; f[15]=10.0; f[16]=10.0; f[17]= 10.0;	//"Rectangular pulse"
	for(i=18; i<f.size(); i++)
	{
	  f[i]=1.0;
	}
	
	
	for(i=0; i<weights.size(); i++)
	{
	  weights[i]=1;		// weight them all equally
	  delta_freq[i]=1;	// use 1 as spacing
	  freq[i]=i;		// use 0 to 29 as freq
	}
	
	

	rmpol=FaradayCube.inverseFourier(0,
				 f,
				 freq,
				 weights,
				 delta_freq,
				 false);	// call inverseFourier


	/*
	RO_LatticeIterator<Float> iter(*lattice_Q_float, cursorShape);
	for (iter.reset(); !iter.atEnd(); iter++) {
		minMax(min, max, iter.cursor());
//  		cout << i++ << " Min = " << min << " Max = " << max << iter.cursor() << endl;		
	}	
	*/
	
	
	#ifdef _debug
	cout << "Finished: " << filename_Q << "!\n";	// Debug output
	#endif
	
	return 0;
}
