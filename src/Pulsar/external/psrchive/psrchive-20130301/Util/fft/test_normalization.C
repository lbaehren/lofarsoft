/***************************************************************************
 *
 *   Copyright (C) 2002 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "test_libraries.h"
#include "BoxMuller.h"
#include "Error.h"

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

using namespace std;

double power (unsigned ndat, float* data);

void runtest2 (int ntrans, double powerin, float* fft1, float* data = 0);

void runtest1 (int ndat)
{
  float* data = new float [ndat];
  float* fft1 = new float [ndat + 2];

  fprintf (stderr, "Generating %d random numbers\n", ndat);
  BoxMuller gasdev;

  for (int idat=0; idat<ndat; idat++)
    data[idat] = gasdev();

  int ntrans = ndat/2;

  fprintf (stderr, "Forward R->C FFT:%d\n", ndat);
  FTransform::frc1d (ndat, fft1, data);

  runtest2 (ntrans, power(ndat,data), fft1);

  fprintf (stderr, "Forward C->C FFT:%d\n", ntrans);
  FTransform::fcc1d (ntrans, fft1, data);

  runtest2 (ntrans, power(ndat,data), fft1, data);
}

double power (unsigned ndat, float* data)
{
  double p = 0;
  double mean = 0;
  for (unsigned idat=0; idat<ndat; idat++) {
    p += data[idat]*data[idat];
    mean += data[idat];
  }
  mean /= ndat;
  return p - mean*mean;
}

using namespace FTransform;

double diff (unsigned ndat, float* in, float* out)
{
  double c = 0.0;
  double scale = get_scale (ndat/2, fcc) * get_scale (ndat/2, bcc);

  cerr << "scale=" << scale << endl;

  for (unsigned idat=0; idat<ndat; idat++) {
    double d = scale*in[idat] - out[idat];
    c += d * d;
  }

  return c / ndat / scale;
}

void test (int num, double got, double expect)
{
  fprintf (stderr, "POWER fft%d/in=%lf - expect %lf\n", num, got, expect);
  double diff = fabs(got-expect)/expect;
  double tol = 5e-4;
  if ( diff > tol ) {
    cerr << "%diff = " << diff << "  (tol = " << tol << ")" << endl;
    fprintf (stderr, "normalization test %d failed\n", num);
    exit (-1);
  }
}

void runtest2 (int ntrans, double powerin, float* fft1, float* data)
{
  float* fft2 = new float [ntrans*2];

  fprintf (stderr, "Backward C->C FFT:%d\n", ntrans);
  bcc1d (ntrans, fft2, fft1);

  if (data) {
    double d = diff (ntrans*2, data, fft2);
    fprintf (stderr, "For %d pts: <(out-in)^2> = %lf\n", ntrans, d);
    if (d > 1e-6) {
      fprintf (stderr, "Unacceptable error\n");
      exit(-1);
    }
  }

  double expect1 = 1;
  double expect2 = 1;

  expect1 = get_scale (ntrans, fcc);
  expect2 = expect1*get_scale (ntrans, bcc);
  expect1 *= expect1; // dealing with power
  expect2 *= expect2;

  test (1, power(ntrans*2, fft1) / powerin, expect1);
  test (2, power(ntrans*2, fft2) / powerin, expect2);

  // Now try taking a sub-band...  although the power should be
  // divided evenly into each channel, perform the test for each

  int ntests = 4;
  int nchan[4] = {8, 16, 32, 64};

  for (int j=0; j<ntests; j++) {
    fprintf (stderr, "\nForward:%dpt followed by %d sub-channels: \n",
	     ntrans, nchan[j]);

    int bdat = ntrans / nchan[j];
    fprintf (stderr, "Backward C->C FFT:%d\n", bdat);

    double powerout = 0.0;
    for (int ichan=0; ichan<nchan[j]; ichan++) {

      bcc1d (bdat, fft2, fft1+ichan*bdat*2);
      // find the average multiplicative factor
      double chan_power = power (bdat*2, fft2);
      powerout += chan_power;
    }

    expect1 = get_scale (ntrans, fcc);
    expect2 = expect1*get_scale (bdat, bcc);
    expect1 *= expect1; // dealing with power
    expect2 *= expect2;

    test (j+3, powerout / powerin, expect2);

  }

}

void runtest () try {

  // from 16kpt
  int ndat = 16 * 1024;
    
  for (unsigned i=0; i < 3; i++) {
    runtest1 (ndat);
    ndat *= 4;
  }

  FTransform::clean_plans ();
  
}
catch (Error& error) {
  cerr << "test_normalization error " << error << endl;
  exit (-1);
}


int main (int argc, char** argv) 
{
  FTransform::test_libraries (runtest, "normalization factors");
  return 0;
}

