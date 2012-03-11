/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "test_libraries.h"
#include "BoxMuller.h"

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

using namespace std;

void runtest () try {

  int idat, ndat = 16 * 1024;  // 16kpt set of random, Gaussian noise
  float* data = new float [ndat];
  float* copy = new float [ndat];
  float* fft1 = new float [ndat+2];
  float* back = new float [ndat+2];

  long idum = -1;
  float dc_value = 3.0;

  fprintf (stderr, "Generating %d random numbers\n", ndat);

  BoxMuller gasdev;

  for (idat=0; idat<ndat; idat++)
    copy[idat] = data[idat] = dc_value + gasdev ();

  // ensure that DC and Nyquist are properly set by frc1d
  fft1[0] = fft1[ndat] = 0.0;   // should be set to other than zero
  fft1[1] = fft1[ndat+1] = 3.0; // should be set to zero

  fprintf (stderr, "Forward R->C FFT:%d\n", ndat);
  FTransform::frc1d (ndat, fft1, data);

  for (unsigned j=0; j < ndat; j++)
    if (copy[j] != data[j])  {
      fprintf(stderr,"idat=%d before=%f after=%f\n", j, data[j], copy[j]);
      fprintf(stderr,"Out-of-place transform does not preserve input!\n");
      exit(-1);
    }

  if (fft1[0] == 0.0) {
    fprintf (stderr, "Re[DC] = 0\n");
    exit(-1);
  }
  if (fft1[1] != 0.0) {
    fprintf (stderr, "Im[DC] != 0\n");
    exit(-1);
  }

  if (fft1[ndat] == 0.0) {
    fprintf (stderr, "Re[Nyquist] = 0\n");
    exit(-1);
  }
  if (fft1[ndat+1] != 0.0) {
    fprintf (stderr, "Im[Nyquist] != 0\n");
    exit(-1);
  }

  fprintf (stderr, "DC=%f,%f\n", fft1[0],fft1[1]);
  fprintf (stderr, "Ny=%f,%f\n", fft1[ndat],fft1[ndat+1]);

  fprintf (stderr, "Backward C->R FFT:%d\n", ndat);
  FTransform::bcr1d (ndat, back, fft1);

  for (idat=0; idat<ndat; idat++) {
    if (FTransform::get_norm() == FTransform::unnormalized)
      back[idat] /= ndat;
    float residual = (back[idat]-data[idat])/data[idat];
    if (fabs(residual) > 1e-4) {
      fprintf (stderr, "idat=%d data=%f back=%f diff=%g\n", 
               idat, back[idat], data[idat], residual);
      exit(-1);
    }
  }

  cerr << "test_real_complex PASS" << endl;
}
catch (Error& e) {
  cerr << "test_real_complex ERROR" << e << endl;
  exit(-1);
}

void runtest ();

int main (int argc, char** argv)
{
  FTransform::test_libraries (runtest, "real-to-complex and complex-to-real");
  return 0;
}
