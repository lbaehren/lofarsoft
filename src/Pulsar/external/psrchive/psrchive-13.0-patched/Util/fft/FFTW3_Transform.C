/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#if HAVE_FFTW3

#include "FFTW3_Transform.h"
#include "Error.h"
#include "tostring.h"

#include <fftw3.h>
#include <assert.h>
#include <stdint.h>

using namespace std;

// #define _DEBUG 1

#define CHECK_ALIGN(x) assert ( ( ((uintptr_t)x) & 15 ) == 0 )

/* ***********************************************************************

   One-dimensional interface

   *********************************************************************** */

FTransform::FFTW3::Plan::Plan (size_t n_fft, type t)
{
#ifdef _DEBUG
  cerr << "FTransform::FFTW3::Plan nfft=" << n_fft << " t=" << t << endl;
#endif

  int direction_flags = 0;
  if( t & forward )
    direction_flags |= FFTW_FORWARD;
  else
    direction_flags |= FFTW_BACKWARD;

  int flags = 0;

  if (!simd)
    flags = FFTW_UNALIGNED;
#ifdef _DEBUG
  else
    cerr << "FFTW3: Building for SIMD" << endl;
#endif

  if (optimize)
    flags |= FFTW_MEASURE;
  else
    flags |= FFTW_ESTIMATE;

  int floats_req = n_fft;
  if( t & analytic )
    floats_req *= 2;
  else
    floats_req += 2;

  float* in = new float[floats_req];
  float* out = new float[floats_req];

  if (simd) {
    CHECK_ALIGN(in);
    CHECK_ALIGN(out);
  }

  if( !in || !out )
    throw Error(InvalidState, "FTransform::FFTW3::Plan::FFTW3::Plan",
		"Failed to allocate array of size " + tostring(n_fft));

  if( t == frc ) {
    plan = fftwf_plan_dft_r2c_1d (n_fft, in, (fftwf_complex*)out, flags);
  }
  else if( t == bcr ) {
    plan = fftwf_plan_dft_c2r_1d (n_fft, (fftwf_complex*)in, out, flags);
  }
  else {
    plan = fftwf_plan_dft_1d (n_fft, (fftwf_complex*)in, (fftwf_complex*)out,
			      direction_flags, flags);
  }

  this->nfft = n_fft;
  this->call = t;
  optimized = optimize;

  delete [] in;
  delete [] out;

#ifdef _DEBUG
  cerr << "FTransform::FFTW3::Plan ctor done" << endl;
#endif
}

FTransform::FFTW3::Plan::~Plan()
{
  if (plan)
    fftwf_destroy_plan ((fftwf_plan)plan);
}

void FTransform::FFTW3::Plan::frc1d (size_t nfft,
				     float* dest, const float* src)
{
#ifdef _DEBUG
  cerr << "FTransform::FFTW3::Plan::frc1d" << endl;
#endif

  if (simd) {
    CHECK_ALIGN(dest);
    CHECK_ALIGN(src);
  }

  fftwf_execute_dft_r2c ((fftwf_plan)plan,
			 (float*)src, (fftwf_complex*)dest);

#ifdef _DEBUG
  cerr << "FTransform::FFTW3::Plan::frc1d done" << endl;
#endif
}

void FTransform::FFTW3::Plan::fcc1d (size_t nfft,
				     float* dest, const float* src)
{
#ifdef _DEBUG
  cerr << "FTransform::FFTW3::Plan::fcc1d" << endl;
#endif

  if (simd) {
    CHECK_ALIGN(dest);
    CHECK_ALIGN(src);
  }

  fftwf_execute_dft ((fftwf_plan)plan,
		     (fftwf_complex*) src, (fftwf_complex*) dest);
#ifdef _DEBUG
  cerr << "FTransform::FFTW3::Plan::fcc1d done" << endl;
#endif
}

void FTransform::FFTW3::Plan::bcc1d (size_t nfft,
				     float* dest, const float* src)
{
#ifdef _DEBUG
  cerr << "FTransform::FFTW3::Plan::bcc1d" << endl;
#endif

  if (simd) {
    CHECK_ALIGN(dest);
    CHECK_ALIGN(src);
  }

  fftwf_execute_dft ((fftwf_plan)plan, 
		     (fftwf_complex*) src, (fftwf_complex*) dest);
#ifdef _DEBUG
  cerr << "FTransform::FFTW3::Plan::bcc1d done" << endl;
#endif
}

void FTransform::FFTW3::Plan::bcr1d (size_t nfft,
				     float* dest, const float* src)
{
#ifdef _DEBUG
  cerr << "FTransform::FFTW3::Plan::bcr1d" << endl;
#endif

  if (simd) {
    CHECK_ALIGN(dest);
    CHECK_ALIGN(src);
  }

  fftwf_execute_dft_c2r ((fftwf_plan)plan, (fftwf_complex*)src, dest);

#ifdef _DEBUG
  cerr << "FTransform::FFTW3::Plan::bcr1d done" << endl;
#endif
}



/* ***********************************************************************

   Two-dimensional interface

   *********************************************************************** */


FTransform::FFTW3::Plan2::Plan2 (size_t n_x, size_t n_y, type t)
{
  int direction_flags = 0;
  if( t & forward )
    direction_flags |= FFTW_FORWARD;
  else
    direction_flags |= FFTW_BACKWARD;

  int flags = FFTW_UNALIGNED;
  if (optimized)
    flags |= FFTW_MEASURE;
  else
    flags |= FFTW_ESTIMATE;

  fftwf_complex* in = new fftwf_complex[n_x*n_y];
  fftwf_complex* out = new fftwf_complex[n_x*n_y];

  plan = fftwf_plan_dft_2d (n_x, n_y, in, out, direction_flags, flags);
  nx = n_x;
  ny = n_y;
  call = t;
  optimized = optimize;

  delete [] in;
  delete [] out;
}

FTransform::FFTW3::Plan2::~Plan2 ()
{
  fftwf_destroy_plan ((fftwf_plan)plan);
}

void FTransform::FFTW3::Plan2::fcc2d (size_t nx, size_t ny,
				     float* dest, const float* src)
{
  fftwf_execute_dft ((fftwf_plan)plan,
		     (fftwf_complex*)src, (fftwf_complex*)dest);
}


void FTransform::FFTW3::Plan2::bcc2d (size_t nx, size_t ny,
				     float* dest, const float* src)
{
  fftwf_execute_dft ((fftwf_plan)plan,
		     (fftwf_complex*)src, (fftwf_complex*)dest);
}


#endif
