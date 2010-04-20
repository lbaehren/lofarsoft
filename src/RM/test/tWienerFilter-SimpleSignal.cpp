
// Standard header files
#include <iostream>
#include <vector>
#include <string>
#include <math.h>
// IT++ header files
#include "itpp/itbase.h"
// RM header files
#include "wienerfilter.h"

// Namespace usage
using namespace std;
using namespace itpp;
using RM::wienerfilter;

#define debug_

/*!
  \file tWienerFilter-SimpleSignal.cpp
  \ingroup RM
  \brief IT++ Test program and Wiener Filter test implementation

  \author Sven Duscha (sduscha@mpa-garching.mpg.de)
  \date 09-06-2009 (Last change: 30-09-2009)
*/
int main (int argc, char **argv)
{
  int c             = 0;
  int size          = 0;          // size of R Matrix
  int maxiterations = 1;          // maximum number of iterations
  double rmsnoise   = 0;
  string configfile;

  //______________________________________________
  // Parse command line parameters

  opterr = 0;

  while ((c = getopt (argc, argv, "c:i:n:s:")) != -1)
  {
    switch (c) {
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
  try {
    int nfreqs    = 300;    // nof. frequency steps
    int nfaradays = 300;    // nof. Faraday depths
    
    /* create Wiener Filter instance for rmsynthesis */
    wienerfilter filter (nfreqs, nfaradays);
    
    // Load simulation data
    filter.read4SimFreqDataFromFile("sim/sim4Brentjensfreq.dat");		// complete set of 4 input vectors: lambdasq, deltalambdasq, complex intensity (real & imag)
    
    
    vector<double> faradays(nfaradays);		// Faraday depths to probe for
    for(int i=0; i < nfaradays; i++)
      {
	faradays[i]=(int) i-150.0;			// from -150 to +149 rad/m^2
      }
    
    /* create a Noise matrix with rms noise of 1.0 per channels */
    filter.createNoiseMatrix(0.2);
    filter.setFaradayDepths(faradays);
    
    // Set physical parameters, and create Response matrix
    filter.setNuZero(filter.getFrequencies()[0]);
//     filter.setEpsilonZero(filter.getDataVector()[0]);
    
    filter.createResponseMatrix();
    
    // Create an artificial signal/data relationship with a simple ResponseMatrix
    filter.createSimpleResponseMatrix(0, 2);
    filter.createData(10, 8);
    
    //  cout << "R = " << filter.R << endl;
    //  cout << "R*d = " << filter.R*filter.d << endl;
    filter.s=inv(filter.R)*(filter.d - diag(filter.N));	// create artifical signal from data
    
    cout << "d.size()=" << filter.d.size() << "\ts.size()=" << filter.s.size() << endl;
    
    //   filter.writeASCII(filter.frequencies, filter.d, "data.dat");
    filter.writeASCII(filter.faradaydepths, filter.d, "data.dat");		// for simple signal
    filter.writeASCII(filter.faradaydepths, filter.s, "signal.dat");	// debug output for Response relationship
    
    
    filter.setLambdaPhi(40);
    filter.computeVariance_s();
//     filter.computeSMatrix("white");
    
    // Perform Wiener Filtering
    filter.computej();
    
    // cout << "j = " << filter.j << endl;
    filter.writeASCII(filter.faradaydepths, filter.j, "j.dat");
    
    filter.computeD();
    filter.computeQ();
    
    /*
      filter.writeASCII(filter.N, "N.dat");
      
      filter.writeASCII(filter.D, "real" ,"D_r.dat");
      filter.writeASCII(filter.D, "imaginary" ,"D_i.dat");
      
      filter.writeASCII(filter.S, "real" ,"S_r.dat");
      filter.writeASCII(filter.S, "imaginary" ,"S_i.dat");
      
      filter.writeASCII(filter.R, "real" ,"R_r.dat");
      filter.writeASCII(filter.R, "imaginary" ,"R_i.dat");
    */
    
    filter.computeW();
    filter.computeM();
    filter.writeASCII(filter.M, "real" ,"M_r.dat"); 
    filter.writeASCII(filter.M, "imaginary" ,"M_i.dat"); 
    
    // Get and show results
    
    filter.reconstructm();
    
    filter.writeASCII(filter.faradaydepths, filter.m, "map.dat");

    // N = LOFAR noise covariance matrix (e.g. EM x rms from header)
    // j = information source vector
    // R = matrix in frequency-space only (set fixed spectral index), R_i(x)
    // d = data vector
    
    
    // Fill matrices with test data
    
    
    
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
    
  } // END : try {}
  catch (const char* s) {	
    std::cerr << s << std::endl;
  }
  
  
  // iterate result: (later)
  //
  // minimise error by minimising D and maximizing Q?
  //
}
