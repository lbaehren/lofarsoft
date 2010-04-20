
/*!
	\file tWienerFilter.cpp
	\ingroup RM
	\brief IT++ Test program and Wiener Filter test implementation

	\author Sven Duscha (sduscha@mpa-garching.mpg.de)
	\date 09-06-2009  (Last change: 05-11-2009)
*/

#include <iostream>
#include <vector>
#include <string>
#include <math.h>
#include "itpp/itbase.h"
#include "WienerFilter.h"

// Namespace usage
using namespace std;
using namespace itpp;
using RM::wienerfilter;

#define NOISELEVEL 1e-20
#define debug_

int main(int argc, char **argv)
{
  int c=0;
  int size=0;					// size of R Matrix
  int maxiterations=1;		// maximum number of iterations
  string configfile;
  double rmsnoise=0;


  // Parse command line parameters
  opterr = 0;

  while ((c = getopt (argc, argv, "c:i:n:s:")) != -1)
  {
    switch (c)
      {
      case 's':		  // rank N of matrices
	size = atoi(optarg);
	break;
      case 'n':		  // rms noise
	rmsnoise = atof(optarg);
	break;
      case 'c':			// read from config file
	configfile = optarg;  
	break;
      case 'i':			// maximum number of iterations
	maxiterations = atoi(optarg);
	break;
      case '?':
	if (optopt=='s' || optopt=='n'  || optopt=='c' )
	  fprintf (stderr, "Option -%c requires an argument.\n", optopt);
	else if (isprint (optopt))
	  fprintf (stderr, "Unknown option `-%c'.\n", optopt);
	else
	  fprintf (stderr,
		  "Unknown option character `\\x%x'.\n",
		  optopt);
	return 1;
      default:
	abort ();
      }
	}

  //*************************************************
  try
  {
  unsigned int nfreqs=300, nfaradays=300;		// Test No. frequencies and No. of Faraday Depths
  int deltaf=400000;
  double deltafaradays=1;

  wienerfilter rmsynthesis(nfreqs, nfaradays);		// create Wiener Filter instance for rmsynthesis

  // Load simulation data
//  rmsynthesis.read4SimFreqDataFromFile("sim/sim4Brentjensfreq.dat");		// complete set of 4 input vectors: lambdasq, deltalambdasq, complex intensity (real & imag)
  
  
  vector<double> faradays(nfaradays);				// Faraday depths to probe for
  for(unsigned int i=0; i < nfaradays; i++)
  {
     faradays[i]=(double) deltafaradays*i-deltafaradays*nfaradays/2;			// from -150 to +149 rad/m^2
     rmsynthesis.delta_faradaydepths[i]=deltafaradays;
  }
  for(unsigned int i=0; i < nfreqs; i++)					// Frequencies the data lives on
  {
    rmsynthesis.frequencies[i]=120000000+i*deltaf;		// equidistant 400kHz channels
    rmsynthesis.delta_frequencies[i]=deltaf;
  }


  rmsynthesis.readSignalFromFile("./data/gaussianScreen.dat");
  rmsynthesis.setFaradayDepths(faradays);


  rmsynthesis.writeASCII(faradays, rmsynthesis.s, "./signal/signal.dat");	// debug


  rmsynthesis.createResponseMatrix();
  rmsynthesis.d=rmsynthesis.R*rmsynthesis.s;	// create artificial data from signal

  vector<double> noise(nfreqs);
  for(unsigned int i=0; i < nfreqs; i++)
    noise[i]=NOISELEVEL;

  rmsynthesis.wienerFiltering(noise, 5, -1.7, "white");
//   rmsynthesis.wienerFiltering(NOISELEVEL, 5, 0.7, "white");
  rmsynthesis.writeASCII(rmsynthesis.faradaydepths, rmsynthesis.m, "./signal/map.dat");


/*
  rmsynthesis.createNoiseMatrix(NOISELEVEL); 	// create a Noise matrix with rms noise NOISELEVEL per channel

  // Read simulated Faraday emission from file
  rmsynthesis.readSignalFromFile("./data/gaussianScreen.dat");
  rmsynthesis.writeASCII(rmsynthesis.faradaydepths, rmsynthesis.s, "./signal/signal.dat");	// debug output for

  rmsynthesis.createResponseMatrix();
  rmsynthesis.writeASCII(rmsynthesis.R, "real", "./intermediate/R_r.dat");				// debug
  rmsynthesis.writeASCII(rmsynthesis.R, "imaginary", "./intermediate/R_i.dat");			// debug


  rmsynthesis.d=rmsynthesis.R*rmsynthesis.s;	// create artificial data from signal



//   rmsynthesis.writeASCII(rmsynthesis.frequencies, rmsynthesis.d, "data.dat");
  rmsynthesis.writeASCII(rmsynthesis.faradaydepths, rmsynthesis.d, "./intermediate/data.dat");		// for simple signal
  rmsynthesis.writeASCII(rmsynthesis.faradaydepths, rmsynthesis.s, "./signal/signal.dat");	// debug output for Response relationship
 
  
  rmsynthesis.setLambdaPhi(5);
  rmsynthesis.setVariance_s(20);
  rmsynthesis.createSMatrix("white");

  // Perform Wiener Filtering
  rmsynthesis.computej();  
  rmsynthesis.computeD();
  rmsynthesis.computeQ();
 
  rmsynthesis.computeW();
  rmsynthesis.computeM();
  rmsynthesis.writeASCII(rmsynthesis.M, "real" ,"./intermediate/M_r.dat"); 
  rmsynthesis.writeASCII(rmsynthesis.M, "imaginary" ,"./intermediate/M_i.dat"); 

  // Get and show results

  rmsynthesis.reconstructm();
  rmsynthesis.writeASCII(rmsynthesis.faradaydepths, rmsynthesis.m, "./signal/map.dat");
*/

  // N = LOFAR noise covariance matrix (e.g. EM x rms from header)
  // j = information source vector
  // R = matrix in frequency-space only (set fixed spectral index), R_i(x)
  // d = data vector



  // Wiener IFT algorithm:
  // N = diagMatrix * rms
  // compute variance S
  // guess S: exp(-abs(delta phi/lambda_phi)) or ~ -1/2(delta phi/lambda_phi)^2 
  // S(i,j)=variance_s^2*exp(0.5*abs(phi_i-phi_j)^2/(2*lambda_phi^2)) see Gaussian variance_s formula
  // j = R^(dagger)N^(inverse)d	(keep this intermediate result)
  // D= R^(dagger)N^(inverse)R*d (multiply previous result with R from the right)
  // then invert to get S^(inverse)
  // D^(inverse) = S^(inverse) + R^(dagger)*N^(inverse)*R
  // invert to get D

  // Results:
  //
  // map s = Dj
  // error of map = D
  // Instrument fidelity Q=RSR^(dagger)N^(inverse)



  } 			// end of try branch

  catch(const char* s)				// catch all const char* exceptions
  {	
     cout << s << endl;				// display thrown exception messages
  }


  // iterate result: (later)
  //
  // minimise error by minimising D and maximizing Q?
  //
}
