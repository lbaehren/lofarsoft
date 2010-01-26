/***************************************************************************
 *
 *   Copyright (C) 1999 by mbritton Matthew Britton
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "FTransform.h"
#include "malloc16.h"

#include <math.h>

using namespace std;

/* Uses the Fourier shift theorem to shift an array */
void FTransform::shift (unsigned npts, float* arr, double shift)
{
  Array16<float> cmplx_arr (2*npts);
  Array16<float> fft_cmplx_arr (2*npts);

  double shiftrad = 2*M_PI*shift/(double)npts;

  for (unsigned i=0; i<npts; ++i)
  {
    cmplx_arr[2*i] = arr[i];
    cmplx_arr[2*i+1] = 0;
  }

  FTransform::fcc1d(npts, &(fft_cmplx_arr[0]), &(cmplx_arr[0]));

  for (unsigned i=1; i<npts/2; ++i)
  {
    double phase = i*shiftrad;
    double cp = cos(phase);
    double sp = sin(phase);
    double tmp = fft_cmplx_arr[2*i]*cp - fft_cmplx_arr[2*i+1]*sp;
    fft_cmplx_arr[2*i+1] = fft_cmplx_arr[2*i]*sp + fft_cmplx_arr[2*i+1]*cp;
    fft_cmplx_arr[2*i] = tmp;
    fft_cmplx_arr[2*npts-2*i] = fft_cmplx_arr[2*i];
    fft_cmplx_arr[2*npts-2*i+1] = -fft_cmplx_arr[2*i+1];
  }

  FTransform::bcc1d(npts, &(cmplx_arr[0]), &(fft_cmplx_arr[0]));

  float norm = 1.0;
  if (FTransform::get_norm() == FTransform::unnormalized)
    norm = 1.0 / (float) npts;

  for (unsigned i=0; i<npts; ++i)
    arr[i] = cmplx_arr[2*i]*norm;

}
