/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "FTransformAgent.h"
#include "Error.h"

#include <stdlib.h>
#include <math.h>
#include <string.h>

using namespace std;

bool FTransform::optimize = false;
bool FTransform::simd = false;
unsigned FTransform::nthread = 1;

// ////////////////////////////////////////////////////////////////////
//
// One-dimensional FFT plan management
//
// ////////////////////////////////////////////////////////////////////

static FTransform::Plan* last_frc1d = 0;
static FTransform::Plan* last_fcc1d = 0;
static FTransform::Plan* last_bcc1d = 0;
static FTransform::Plan* last_bcr1d = 0;

// Use of this macro decreases the margin for error
#define FT_1D(TYPE) \
  if (!(last_ ## TYPE ## 1d && last_ ## TYPE ## 1d->matches(nfft, TYPE))) \
    last_## TYPE ## 1d = Agent::current->get_plan (nfft, TYPE); \
  last_## TYPE ## 1d -> TYPE ## 1d (nfft, into, from)

//! Forward real-to-complex FFT 
void FTransform::frc1d (size_t nfft, float* into, const float* from)
{
  FT_1D(frc);
}

//! Backward complex-to-real FFT 
void FTransform::bcr1d (size_t nfft, float* into, const float* from)
{
  FT_1D(bcr);
}

//! Forward complex-to-complex FFT
void FTransform::fcc1d (size_t nfft, float* into, const float* from)
{
  FT_1D(fcc);
}

//! Backward complex-to-complex FFT
void FTransform::bcc1d (size_t nfft, float* into, const float* from)
{
  FT_1D(bcc);
}

// ////////////////////////////////////////////////////////////////////
//
// Two-dimensional FFT library interface
//
// ////////////////////////////////////////////////////////////////////

static FTransform::Plan2* last_fcc2d = 0;
static FTransform::Plan2* last_bcc2d = 0;

// Use of this macro decreases the margin for error
#define FT_2D(TYPE) \
  if (!(last_ ## TYPE ## 2d && last_ ## TYPE ## 2d->matches(nx, ny, TYPE))) \
    last_## TYPE ## 2d = Agent::current->get_plan2 (nx, ny, TYPE); \
  last_## TYPE ## 2d -> TYPE ## 2d (nx, ny, into, from)

//! Forward complex-to-complex FFT
void FTransform::fcc2d (size_t nx, size_t ny, float* into, const float* from)
{
  FT_2D(fcc);
}

//! Backward complex-to-complex FFT
void FTransform::bcc2d (size_t nx, size_t ny, float* into, const float* from)
{
  FT_2D(bcc);
}

// ////////////////////////////////////////////////////////////////////
//
// General FFT library interface
//
// ////////////////////////////////////////////////////////////////////

//! Returns currently selected library
string FTransform::get_library()
{
  return Agent::current->name;
}

//! Returns currently selected normalization
FTransform::normalization FTransform::get_norm()
{
  return Agent::current->norm;
}

// ////////////////////////////////////////////////////////////////////////
//!
double FTransform::get_scale (size_t ntrans, type t)
{
  double dim = 1.0;
  if (t & real)
    dim = 0.5;

  if (get_norm() == unnormalized)
    return sqrt(double(ntrans)*dim);

  if (get_norm() == normalized) {
    if (t & forward)
      return sqrt(double(ntrans)*dim);
    else
      return 1.0/sqrt(double(ntrans)*dim);
  }

  throw Error (InvalidState, "FTransform::get_scale",
	       "unsupported normalization for " + get_library());
}

//! Clears out the memory associated with the plans
void FTransform::clean_plans()
{
  for (unsigned ilib=0; ilib < FTransform::Agent::libraries.size(); ilib++)
    FTransform::Agent::libraries[ilib]->clean_plans ();

  last_frc1d = 0;
  last_bcr1d = 0;

  last_fcc1d = 0;
  last_bcc1d = 0;

  last_fcc2d = 0;
  last_bcc2d = 0;
}

//! Choose to use a different library
void FTransform::set_library (const string& name)
{
  FTransform::Agent::set_library (name);
}

//! Get the number of available libraries
unsigned FTransform::get_num_libraries ()
{
  return FTransform::Agent::get_num_libraries ();
}

//! Get the name of the ith available library
std::string FTransform::get_library_name (unsigned i)
{
  return FTransform::Agent::get_library_name (i);
}

//! Inplace wrapper-function- performs a memcpy after FFTing
int FTransform::inplace_frc1d (size_t ndat, float* srcdest)
{
  static vector<float> dest;
  if( dest.size() != ndat+2 )
    dest.resize( ndat+2 );

  float* pdest = &*dest.begin();

  frc1d(ndat,pdest,srcdest);
  memcpy(srcdest,pdest,(ndat+2)*sizeof(float));

  return 0;
}

//! Inplace wrapper-function- performs a memcpy after FFTing
int FTransform::inplace_fcc1d (size_t ndat, float* srcdest)
{
  static vector<float> dest;
  if( dest.size() != ndat*2 )
    dest.resize( ndat*2 );

  float* pdest = &*dest.begin();

  fcc1d(ndat,pdest,srcdest);
  memcpy(srcdest,pdest,ndat*2*sizeof(float));

  return 0;
}

//! Inplace wrapper-function- performs a memcpy after FFTing
int FTransform::inplace_bcc1d (size_t ndat, float* srcdest)
{
  static vector<float> dest;
  if( dest.size() != ndat*2 )
    dest.resize( ndat*2 );

  float* pdest = &*dest.begin();

  bcc1d(ndat,pdest,srcdest);
  memcpy(srcdest,pdest,ndat*2*sizeof(float));

  return 0;
}

//! Inplace wrapper-function- performs a memcpy after FFTing
int FTransform::inplace_bcr1d (size_t ndat, float* srcdest)
{
  static vector<float> dest;
  if( dest.size() != ndat*2 )
    dest.resize( ndat*2 );

  float* pdest = &*dest.begin();

  bcr1d(ndat,pdest,srcdest);
  memcpy(srcdest,pdest,ndat*2*sizeof(float));

  return 0;
}

FTransform::Plan::Plan()
{
}

FTransform::Plan::~Plan()
{
}

