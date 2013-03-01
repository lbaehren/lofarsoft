/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

using namespace std;

/*

See threads related documentation at:
http://www.intel.com/support/performancetools/libraries/mkl/sb/cs-017177.htm

*/

#include "MKL_DFTI_Transform.h"
#include "Error.h"
#include "malloc16.h"

#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>

//#define _DEBUG 1

/* ***********************************************************************

   One-dimensional interface

   *********************************************************************** */

FTransform::MKL_DFTI::Plan::Plan (size_t n_fft, type t)
{
#ifdef _DEBUG
  cerr << "FTransform::MKL_DFTI::Plan nfft=" << n_fft
       << " type='" << t << "'" << endl;
#endif

  long status;

  long n = n_fft;

  plan = 0;

  if (t & real)
  {
#ifdef _DEBUG
    cerr << "FTransform::MKL_DFTI::Plan DFTI_REAL" << endl;
#endif
    status = DftiCreateDescriptor( &plan, DFTI_SINGLE, DFTI_REAL, 1, n );
  }
  else
  {
#ifdef _DEBUG
    cerr << "FTransform::MKL_DFTI::Plan DFTI_COMPLEX" << endl;
#endif
    status = DftiCreateDescriptor( &plan, DFTI_SINGLE, DFTI_COMPLEX, 1, n );
  }

  if (! DftiErrorClass(status, DFTI_NO_ERROR))
    throw Error (InvalidState, "FTransform::MKL_DFTI::Plan ctor",
		 "fail DftiCreateDescriptor\n\t%s",
		 DftiErrorMessage(status));

#ifdef _DEBUG
  cerr << "FTransform::MKL_DFTI::Plan::Plan plan=" << plan << endl;
#endif

  /*

    http://www.intel.com/software/products/mkl/docs/webhelp/fft/fft_NumberOfThreads.html

    You create threads in the application yourself after initializing
    the only DFT descriptor. This implies that threading is employed
    for parallel DFT computation only, and the descriptor is released
    upon return from the parallel region. In this case, each thread
    uses the same descriptor.

    You must use the DftiSetValue() function to set the
    DFTI_NUMBER_OF_USER_THREADS to the actual number of DFT
    computation threads, because multiple threads will be using the
    same descriptor. If this setting is not done, your program will
    work incorrectly or fail, since the descriptor contains individual
    data for each thread.

   */

  status = DftiSetValue( plan, DFTI_NUMBER_OF_USER_THREADS, nthread );
  if (! DftiErrorClass(status, DFTI_NO_ERROR))
    throw Error (InvalidState, "FTransform::MKL_DFTI::Plan ctor",
                 "fail DftiSetValue DFTI_NUMBER_OF_USER_THREADS %d\n\t%s",
                 nthread, DftiErrorMessage(status));

  status = DftiSetValue( plan, DFTI_PLACEMENT, DFTI_NOT_INPLACE );
  if (! DftiErrorClass(status, DFTI_NO_ERROR))
    throw Error (InvalidState, "FTransform::MKL_DFTI::Plan ctor",
		 "fail DftiSetValue DFTI_PLACEMENT\n\t%s",
		 DftiErrorMessage(status));

  status = DftiCommitDescriptor( plan );
  if (! DftiErrorClass(status, DFTI_NO_ERROR))
    throw Error (InvalidState, "FTransform::MKL_DFTI::Plan ctor",
		 "fail DftiCommitDescriptor\n\t%s",
		 DftiErrorMessage(status));

  this->nfft = n_fft;
  this->call = t;
  this->optimized = optimize;
}

FTransform::MKL_DFTI::Plan::~Plan()
{
#ifdef _DEBUG
  cerr << "FTransform::MKL_DFTI::Plan::~Plan plan=" << plan << endl;
#endif

  if (plan)
    DftiFreeDescriptor(&plan);
}

#define CHECK_ALIGN(x) assert ( ( ((uintptr_t)x) & 15 ) == 0 )

#define MAKE_ALIGN(x,n) \
  float* temp = 0; \
  if ( ( ((uintptr_t)x) & 15 ) != 0 ) \
  { temp = (float*) malloc16 (n * sizeof(float)); \
    memcpy (temp, x, n * sizeof(float)); \
    x = temp; }

#define FREE_ALIGN if (temp) free (temp);

void FTransform::MKL_DFTI::Plan::frc1d (size_t nfft, float* dest, 
					const float* src)
{
  MAKE_ALIGN(src,nfft);
  CHECK_ALIGN(dest);

#ifdef _DEBUG
  cerr << "FTransform::MKL_DFTI::Plan::frc1d plan=" << plan << endl;
#endif
  long status = DftiComputeForward( plan, (void*) src, dest );
  if (! DftiErrorClass(status, DFTI_NO_ERROR))
    throw Error (InvalidState, "FTransform::MKL_DFTI::Plan::frc1d",
		 "fail DftiComputeForward\n\t%s",
		 DftiErrorMessage(status));

  FREE_ALIGN;
}

void FTransform::MKL_DFTI::Plan::fcc1d (size_t nfft, float* dest,
					const float* src)
{
  MAKE_ALIGN(src,nfft*2);
  CHECK_ALIGN(dest);

#ifdef _DEBUG
  cerr << "FTransform::MKL_DFTI::Plan::fcc1d" << endl;
#endif
  long status = DftiComputeForward( plan, (void*) src, dest );
  if (! DftiErrorClass(status, DFTI_NO_ERROR))
    throw Error (InvalidState, "FTransform::MKL_DFTI::Plan::frc1d",
		 "fail DftiComputeForward\n\t%s",
		 DftiErrorMessage(status));

  FREE_ALIGN;
}

void FTransform::MKL_DFTI::Plan::bcc1d (size_t nfft, float* dest,
					const float* src)
{
  MAKE_ALIGN(src,nfft*2);
  CHECK_ALIGN(dest);

#ifdef _DEBUG
  cerr << "FTransform::MKL_DFTI::Plan::bcc1d" << endl;
#endif
  long status = DftiComputeBackward( plan, (void*) src, dest );
  if (! DftiErrorClass(status, DFTI_NO_ERROR))
    throw Error (InvalidState, "FTransform::MKL_DFTI::Plan::frc1d",
		 "fail DftiComputeBackward\n\t%s",
		 DftiErrorMessage(status));

  FREE_ALIGN;
}

void FTransform::MKL_DFTI::Plan::bcr1d (size_t nfft, float* dest,
					const float* src)
{
  MAKE_ALIGN(src,nfft*2);
  CHECK_ALIGN(dest);

#ifdef _DEBUG
  cerr << "FTransform::MKL_DFTI::Plan::bcr1d" << endl;
#endif
  long status = DftiComputeBackward( plan, (void*) src, dest );
  if (! DftiErrorClass(status, DFTI_NO_ERROR))
    throw Error (InvalidState, "FTransform::MKL_DFTI::Plan::frc1d",
		 "fail DftiComputeBackward\n\t%s",
		 DftiErrorMessage(status));

  FREE_ALIGN;
}

/* ***********************************************************************

   Two-dimensional interface

   *********************************************************************** */


FTransform::MKL_DFTI::Plan2::Plan2 (size_t n_x, size_t n_y, type t)
{
  long n[2];
  n[0] = n_x;
  n[1] = n_y;

  int status;

  if (t & real)
    status = DftiCreateDescriptor( &plan, DFTI_SINGLE, DFTI_REAL, 1, n );
  else
    status = DftiCreateDescriptor( &plan, DFTI_SINGLE, DFTI_COMPLEX, 1, n );

  if (! DftiErrorClass(status, DFTI_NO_ERROR))
    throw Error (InvalidState, "FTransform::MKL_DFTI::Plan2 ctor",
		 "fail DftiCreateDescriptor");

#if 0

  // this code is in the double precision version in
  // interfaces/fftw3xc/wrappers
  if (t & real) {
    status = DftiSetValue
      ( plan, DFTI_CONJUGATE_EVEN_STORAGE, DFTI_COMPLEX_COMPLEX );

    if (! DftiErrorClass(status, DFTI_NO_ERROR))
      throw Error (InvalidState, "FTransform::MKL_DFTI::Plan2 ctor",
		   "fail DftiSetValue DFTI_CONJUGATE_EVEN_STORAGE");
  }

#endif

  status = DftiSetValue( plan, DFTI_PLACEMENT, DFTI_NOT_INPLACE );
  if (! DftiErrorClass(status, DFTI_NO_ERROR))
    throw Error (InvalidState, "FTransform::MKL_DFTI::Plan2 ctor",
		 "fail DftiSetValue DFTI_PLACEMENT");

  status = DftiCommitDescriptor( plan );
  if (! DftiErrorClass(status, DFTI_NO_ERROR))
    throw Error (InvalidState, "FTransform::MKL_DFTI::Plan2 ctor",
		 "fail DftiCommitDescriptor");

  this->nx = n_x;
  this->ny = n_y;
  this->call = t;
}

FTransform::MKL_DFTI::Plan2::~Plan2 ()
{
  if (plan)
    DftiFreeDescriptor(&plan);
}

void FTransform::MKL_DFTI::Plan2::fcc2d (size_t nx, size_t ny,
					 float* dest, const float* src)
{
  DftiComputeForward( plan, (void*) src, dest );
}


void FTransform::MKL_DFTI::Plan2::bcc2d (size_t nx, size_t ny,
					 float* dest, const float* src)
{
  DftiComputeBackward( plan, (void*) src, dest );
}

