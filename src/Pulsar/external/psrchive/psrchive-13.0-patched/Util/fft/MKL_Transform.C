/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#if HAVE_MKL

#include "MKL_Transform.h"
#include <inttypes.h>

#include <assert.h>

using namespace std;

extern "C" { 
  // complex to complex
  void cfft1d_(float * data, int * ndat, int * isign, float * wsave);
  // real to complex
  void scfft1d_(float * data, int * ndat, int * isign, float * wsave);
  // complex to real
  void csfft1d_(float * data, int * ndat, int * isign, float * wsave);
}

FTransform::MKL::Plan::~Plan()
{
  if (mkl_plan)
    delete [] mkl_plan;

  // cerr << "MKL::Plan destroyed" << endl;
}

FTransform::MKL::Plan::Plan (size_t n_fft, type t)
{
#ifdef _DEBUG
  cerr << "FTransform::MKL::Plan nfft=" << n_fft
       << " call='" << fft_call << "'" << endl;
#endif

  int e;
  if ( frexp((double) n_fft, &e) != 0.5 )
    throw Error (InvalidParam, "FTransform::MKL::Plan",
                 "Transform length %d is not a power of two", n_fft);

  if( t & real )
    mkl_plan = new float[2*n_fft+2];
  else
    mkl_plan = new float[n_fft*4];
  assert( mkl_plan != 0 );

  int signed_ndat = n_fft;

  if( int64_t(uint64_t(n_fft)) != signed_ndat )
    throw Error(InvalidState,"FTransform::MKL::Plan::MKL::Plan",
		"Could not convert nfft="UI64" to an integer",
		uint64_t(n_fft));

  int isign = 0;
  if( t == frc )
    scfft1d_(mkl_plan, &signed_ndat, &isign, mkl_plan);
  else if( t == bcr )
    csfft1d_(mkl_plan, &signed_ndat, &isign, mkl_plan);
  else
    cfft1d_(mkl_plan, &signed_ndat, &isign, mkl_plan);

  nfft = n_fft;
  call = t;
  optimized = false;

}

void FTransform::MKL::Plan::frc1d (size_t nfft, float* dest, const float* src)
{
  int isign = -1;
  int signed_nfft = nfft;

  memcpy (dest, src, nfft*sizeof(float));
  scfft1d_(dest, &signed_nfft, &isign, mkl_plan);
}

void FTransform::MKL::Plan::fcc1d (size_t nfft, float* dest, const float* src)
{
  int isign = -1;
  int signed_nfft = nfft;

  memcpy (dest, src, nfft*2*sizeof(float));
  cfft1d_(dest, &signed_nfft, &isign, mkl_plan);
}

void FTransform::MKL::Plan::bcc1d (size_t nfft, float* dest, const float* src)
{
  int isign = 1;
  int signed_nfft = nfft;

  memcpy (dest, src, nfft*2*sizeof(float));
  cfft1d_(dest, &signed_nfft, &isign, mkl_plan);
}

void FTransform::MKL::Plan::bcr1d (size_t nfft, float* dest, const float* src)
{
  int isign = -1;
  int signed_nfft = nfft;

  memcpy (dest, src, (nfft+2)*sizeof(float));
  csfft1d_(dest, &signed_nfft, &isign, mkl_plan);
}


#endif
