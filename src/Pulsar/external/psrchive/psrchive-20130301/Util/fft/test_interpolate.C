/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "test_libraries.h"
#include "interpolate.h"
#include "BoxMuller.h"

#include <iostream>
#include <stdlib.h>

using namespace std;

void meanvar (double& mean, double& var, vector<float>& data)
{
  mean = 0.0;
  var = 0.0;

  for (unsigned i=0; i<data.size(); i++) {
    mean += data[i];
    var += data[i] * data[i];
  }
  
  mean /= data.size();
  var /= data.size();
  var -= mean*mean;
}

void runtest ();

int main (int argc, char** argv)
{
  FTransform::test_libraries (runtest, "interpolation");
  return 0;
}

void runtest ()
{
  // from 8kpt
  int ndat = 8 * 1024;
  cerr << "Generating " << ndat << " random numbers" << endl;

  BoxMuller gasdev;

  vector<float> data (ndat);
  for (int idat=0; idat<ndat; idat++)
    data[idat] = gasdev();

  double mean_0 = 0.0;
  double var_0 = 0.0;
  meanvar (mean_0, var_0, data);

  int ntests = 4;
  unsigned bigger = 2;

  for (int j=0; j<ntests; j++) {

    vector<float> into (bigger*ndat);

    cerr << "Interpolating into " << into.size() << endl;
    fft::interpolate (into, data);

    double mean = 0.0;
    double var = 0.0;
    meanvar (mean, var, into);

    double diff = fabs(mean-mean_0);
    double tol = 1e-6;

    if ( diff > tol ) {
      cerr << "Mean:  input=" << mean_0 << " output=" << mean 
           << " diff=" << diff << " tol=" << tol << endl;
      exit(-1);
    }

    diff = fabs(var-var_0);
    tol = 1e-4;

    if ( diff > tol ) {
      cerr << "Variance:  input=" << var_0 << " output=" <<  var
           << " diff=" << diff << " tol=" << tol << endl;
      exit(-1);
    }

    bigger *= 4;

  }

  vector< complex<float> > C_in (128);
  vector< complex<float> > C_out (256);

  fft::interpolate (C_out, C_in);

  cerr << "fft::interpolate: all tests passed" << endl;

}
