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

#include <iostream>									// STL iostream

#include "../../implement/rm.h"					// RM Synthesis class
#include "../../implement/rmCube.h"				// RM Cube class
#include "../../../DAL/implement/dalFITS.h"

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
using namespace DAL;

int main (int argc, char * const argv[]) {

	try{

//	Bool status;					// status of casa calls

	unsigned int i=0;				// loop variable
	unsigned int N=30;				// length of test vectors

	vector<double> phi;		// Faraday depths to probe for
	complex<double> rmpol;		// polarized intensity at Faraday depth probed for
	vector<complex<double> > rmsf;	// Rotation Measure Spread Function


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


	// create a rmCube object with an associated two dimensional buffer
	rmCube FaradayCube(1024, 1024, 100, 5.0);
	FaradayCube.setFaradayDepths(-100, 100, 10); // Set up Faraday depths to be probed
	//FaradayCube.createBufferCube();		     // create buffer for whole RM cube


	// TEST: inverseFourier RM-Synthesis
	// compute 30 pts along 0 to 29
	vector<complex<double> > f(N);
	vector<complex<double> > F(N);
	vector<double> weights(N);	// weights
	vector<double> freq(N);
	vector<double> delta_freq(N);

	// Test file read functions
	freq=FaradayCube.readFrequenciesAndDeltaFrequencies("frequenciesDelta.txt", delta_freq);


	// Generate Rectangular pulse in f (real)

	
	for(i=0; i<weights.size(); i++)
	{
	  weights[i]=1.0;	// weight them all equally
	 // delta_freq[i]=1.0;	// use 1 as spacing
	  
	  cout << "delta_freq[" << i << "] = " << delta_freq[i] << endl;
	  
	  freq[i]=i;		// use 0 to 29 as freq
	}
	
	
	dalFITS fitsimage("Leiden_GEETEE_CS1_1.FITS", READONLY);
	fitsimage.moveAbsoluteHDU(1);
	
	int naxis=0;
	long naxes[3]={0,0,0};
	
	
	naxis=fitsimage.getImgDim();
	fitsimage.getImgSize(3 , naxes);
	
	double *buffer=new double[naxes[0]*naxes[1]];
	
	//fitsimage.readPlane(buffer, 1);
	
	
	// call inverseFourier with phi=5
	for(i=0; i<N; i++)
	{
	  rmpol=FaradayCube.inverseFourier(i, f, freq, weights,  delta_freq, false);
	  cout << rmpol << endl;
	}
	 
	FaradayCube.computeRMSF(freq, delta_freq, true);

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
	
	catch(const char *s)
	{
	  cout << s << endl;
	}
	
}
