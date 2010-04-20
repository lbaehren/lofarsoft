
#include <iostream>
#include <vector>
#include <string>
#include <math.h>
#include "itpp/itbase.h"
#include "wienerfilter.h"

// Namespace usage
using namespace std;
using namespace itpp;
using RM::wienerfilter;

/*!
	\file tWienerFilter-FourierTransform.cpp
	\ingroup RM
	\brief IT++ Test program and Wiener Filter test implementation
	
	\author Sven Duscha (sduscha@mpa-garching.mpg.de)
	\date 09-06-2009  (Last change: 28-06-2009)
*/

int main(int argc, char **argv)
{
  int c=0;
  int size=0;					// size of R Matrix
  int maxiterations=1;		// maximum number of iterations
  string configfile;
  double rmsnoise=0;

  int nfreqs=200, nfaradays=300;	// Test No. frequencies and No. of Faraday Depths


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
  cvec testdata(nfreqs);		// create vector with test data

  // fill up testdata for variance test
/*  testdata[0]=complex<double>(1.21, 0);
  testdata[1]=complex<double>(3.4, 0); 
  testdata[2]=complex<double>(2, 0);
  testdata[3]=complex<double>(4.66, 0);
  testdata[4]=complex<double>(1.5, 0);
  testdata[5]=complex<double>(5.61, 0);
  testdata[6]=complex<double>(7.22, 0);	*/

  wienerfilter rmsynthesis(nfreqs, nfaradays);		// create Wiener Filter instance for rmsynthesis

  rmsynthesis.createNoiseMatrix(0.01); 

  // fill lambda squareds vector
  vector<double> lambdaSqs(nfreqs); 
  vector<double> deltaLambdaSqs(nfreqs);
  vector<double> faradays(nfaradays);
  for(unsigned int i=0; i<lambdaSqs.size(); i++)
  {
     lambdaSqs[i]=i;
	  deltaLambdaSqs[i]=1;	 
	  faradays[i]=i;
  } 


  rmsynthesis.setLambdaSquareds(lambdaSqs);
  rmsynthesis.setDeltaLambdaSquareds(deltaLambdaSqs);
  rmsynthesis.setFaradayDepths(faradays);
  //rmsynthesis.setDataVector(testdata);			// write test data into data vector d
  //rmsynthesis.computeVariance_s();				// compute variance_s
 
  // Set up matrices
  //rmsynthesis.guessSMatrix("gaussian");					
  rmsynthesis.createResponseMatrix();

  // Fill testdata vector with polarized emission from Faraday example
  for(int i=0; i<nfaradays; i++)
  {
    testdata[i]=0;		// zero out

	 if(i>120 && i<130)
		testdata[i]=10;	// Faraday emission block: thin but high	A
	 if(i>180 && i<210)
		testdata[i]=2;	// Faraday emission block: wide but low
	 if(i>250 && i<270)
		testdata[i]=3;	// Faraday emission block: thin and higher
  }

	/*
  rmsynthesis.setDataVector(testdata);					// Write testdata into d vector
  rmsynthesis.writeDataToFile("testFaraday.dat");	// write input f out to file

  cvec ftsignal(nfaradays);						// vector containing Fourier transformed data

  cmat FT=rmsynthesis.getResponseMatrix();  	// get Response Matrix as Fourier Transform  
  ftsignal=FT*rmsynthesis.getDataVector();	// apply FT to data vector
  rmsynthesis.setSignalVector(ftsignal);		// write ftsignal into rmsynthesis.s

  rmsynthesis.writeSignalToFile("signalFaraday.dat"); 	// Write signal = Fourier transformed data to file
*/
  // N = LOFAR noise covariance matrix (e.g. EM x rms from header)
  // j = information source vector
  // R = matrix in frequency-space only (set fixed spectral index), R_i(x)
  // d = data vector


  // Fill matrices with test data



  // Wiener IFT algorithm:
  // N = diagMatrix * rms
  // S(i,j)=variance_s^2*exp(0.5*abs(phi_i-phi_j)^2/(2*lambda_phi^2)) see Gaussian variance_s formula
  // j = R^(dagger)N^(inverse)d	(keep this intermediate result)
  // D= R^(dagger)N^(inverse)R (multiply previous result with R from the right)
  // guess S: exp(-abs(delta phi/lambda_phi)) or ~ -1/2(delta phi/lambda_phi)^2 
  // then invert to get S^(inverse)
  // D^(inverse) = S^(inverse) + R^(dagger)*N^(inverse)*R
  // invert to get D

  // Results:
  //
  // map s = Dj
  // error of map = D
  // Instrument fidelity Q=RSR^(dagger)N^(inverse)

  } 			// end of try branch

  catch(const char* s)						// catch all const char* exceptions
  {	
     cout << s << endl;						// display thrown exception messages
  }


  // iterate result: (later)
  //
  // minimise error by minimising D and maximizing Q?
  //
}
