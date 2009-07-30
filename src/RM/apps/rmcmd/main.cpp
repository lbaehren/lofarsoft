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
/*
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
*/

#define _debug	

using namespace std;
//using namespace casa;					// casa(core) namespace
using namespace DAL;

int main (int argc, char * const argv[]) {

	try{

	unsigned int N=96;			// length of test vectors from Brentjens simulation


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




	// TEST: inverseFourier RM-Synthesis
	// compute 300 pts along 0 to 299
	vector<double> phis(300);
	vector<complex<double> > intensities(N);
	vector<double> weights(N);	// weights
	vector<double> freq(N);
	vector<double> delta_freq(N);
	vector<double> freq_low(N);
	vector<double> freq_high(N);
	vector<double> lambda_squared(N);
	vector<double> delta_lambda_squared(N);
	
	vector<complex<double> > rmsf(N);
	vector<complex<double> > rmpol(N);		// polarized intensity at Faraday depth probed for

	// Need to test lambdaSquaredTopHat function with WSRT text file
	rmCube rm(1024, 1024, 100, 5);
	
	// Faradayd depths to probe for
	for(unsigned int i=0; i<300; i++)
	{
		phis[i]=i-150.0;						// from -150 to +150
	}
	// Set weights
	for(unsigned int i=0; i<N; i++)
		weights[i]=1;							// all channels carry equal weight 1

	// Frequency information from measurement
	for(unsigned int i=0; i<N; i++)		// create Test data (only 96 channels though)
	{
		freq[i]=323937500.0+i*312500.0;	// from Brentjens mosaic.freq text file
		delta_freq[i]=312500.0;

		// compute freq_low and freq_high limits for delta_lambda_squareds
		freq_low[i]=323937500.0+i*312500-0.5*312500.0;
		freq_high[i]=323937500.0+i*312500+0.5*312500.0;
	}

		
	
	// Load simulated Brentjens data from file
	rm.readSimDataFromFile("sim4Brentjens.dat", lambda_squared, delta_lambda_squared, intensities);
	
	// perform RM-Synthesis
	rmsf=rm.inverseFourier(phis, intensities, lambda_squared, weights, delta_lambda_squared, 0);
	rm.writeRMtoFile(phis, rmsf, "rmpolBrentjens.dat");
	
	// need to verify with above deltaLambdas computation of RMSF!
	rmsf=rm.RMSF(phis, lambda_squared, weights, delta_lambda_squared);
	rm.writeRMtoFile(phis, rmsf, "rmsf.dat");				// debug write RMSF to file

	// verify with frequencies and delta frequencies computation of RMSF!
	vector<complex<double> > rmsffreq(N);

	rmsffreq=rm.RMSFfreq(phis, freq, weights, delta_freq);
	rm.writeRMtoFile(phis, rmsf, "rmsffreq.dat");		// debug write RMSF to file

	// Open Image and set parameters
	dalFITS fitsimage("Leiden_GEETEE_CS1_1.FITS", READONLY);
	fitsimage.moveAbsoluteHDU(1);

	
	int naxis=0;
	long naxes[3]={0,0,0};
	
	
	naxis=fitsimage.getImgDim();
	fitsimage.getImgSize(3 , naxes);
	
	double *buffer=new double[naxes[0]*naxes[1]];


	// create a rmCube object with an associated two dimensional buffer
	rmCube FaradayCube(1024, 1024, 100, 5.0);
	FaradayCube.setFaradayDepths(-100, 100, 10); // Set up Faraday depths to be probed
	
	
	//fitsimage.readPlane(buffer, 1);
	
	
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
