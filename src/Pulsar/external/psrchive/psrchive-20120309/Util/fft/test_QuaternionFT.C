/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "QuaternionFT.h"
#include "random.h"

using namespace std;

int main ()
{
  // QuaternionFT::verbose = true;
  bool verbose = false;

  unsigned npts = 256;
  unsigned nloops = unsigned (1e3);

  std::vector< Quaternion<float> > in (256);

  std::vector< Quaternion<float> > out1;
  std::vector< Quaternion<float> > out2;

  QuaternionFT fft;

  cerr << "Testing the Quaternion Fourier Transform on " << nloops <<
    " sets of " << npts << " random Quaternions" << endl;

  unsigned errors = 0;

  for (unsigned i=0; i<nloops; i++) {

    random_matrix (in, 10.0);

    double theta;
    double phi;

    random_value (theta, M_PI);
    random_value (phi, M_PI/2.0);

    fft.set_axis (theta, phi);

    QuaternionFT::Direction direction = QuaternionFT::Forward;
    if (nloops % 2)
      direction = QuaternionFT::Backward;

    fft.set_direction (direction);
  
    try {

      // fast
      fft.fft (in, out1);

      // slow
      fft.ft (in, out2);

    }
    catch (Error& error) {
      cerr << error << endl;
      return -1;
    }
    
    for (unsigned ipt=0; ipt < npts; ipt++) {
      float diff = norm (out1[ipt] - out2[ipt]) / norm(out1[ipt]);
      if (diff > 2e-6) {
	if (verbose)
	  cerr << "Unequal output[" << ipt << "]: fast=" << out1[ipt] 
	       << " slow=" << out2[ipt] << " diff=" << diff << endl;
	errors ++;
      }
    }


  }

  if (errors) {
    cerr << errors << " errors. (" << (float(errors)*1e3)/(10.0*npts*nloops)
	 << "%)" << endl;
    return -1;
  }

  return 0;
}

