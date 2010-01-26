/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>

#include "FTransformAgent.h"

#ifdef HAVE_MKL_DFTI
#include "MKL_DFTI_Transform.h"
#endif

#ifdef HAVE_MKL
#include "MKL_Transform.h"
#endif

#ifdef HAVE_FFTW3
#include "FFTW3_Transform.h"
#endif

#ifdef HAVE_FFTW
#include "FFTW_Transform.h"
#endif

#ifdef HAVE_IPP
#include "IPP_Transform.h"
#endif

using namespace std;

// ////////////////////////////////////////////////////////////////////
//
// Internal implementation initialization 
//
// ////////////////////////////////////////////////////////////////////

static int initialise()
{
#ifdef HAVE_MKL
  FTransform::MKL::Agent::enlist ();
#endif

#ifdef HAVE_MKL_DFTI
  FTransform::MKL_DFTI::Agent::enlist ();
#endif

#ifdef HAVE_FFTW3
  FTransform::FFTW3::Agent::enlist ();
#endif

#ifdef HAVE_FFTW
  FTransform::FFTW::Agent::enlist ();
#endif

#ifdef HAVE_IPP
  FTransform::IPP::Agent::enlist ();
#endif

  if (FTransform::Agent::get_num_libraries() == 0) {
    cerr << "\nFTransform: ERROR No FFT libraries installed!\n\n";
    exit(-1);
  }

  return 0;
}

ThreadContext* FTransform::Agent::context = 0;

void FTransform::Agent::add ()
{ 
  libraries.push_back (this);
  if (!current)
    current = this;
}

//! Choose to use a different library
void FTransform::Agent::set_library (const string& name)
{
  for (unsigned ilib=0; ilib<libraries.size(); ilib++){
    if (libraries[ilib]->name == name){
      current = libraries[ilib];
      return;
    }
  }

  string s;

  for (unsigned ilib=0; ilib<get_num_libraries(); ilib++)
    s += "'" + libraries[ilib]->name + "' ";
    
  throw Error (InvalidState, "FTransform::Agent::set_library",
	       "Library '" + name + "' is not in valid libraries: " + s);
}

FTransform::Agent::Agent()
{
}

FTransform::Agent::~Agent()
{
}

Reference::To<FTransform::Agent> FTransform::Agent::current;

vector< Reference::To<FTransform::Agent> > FTransform::Agent::libraries;

static int initialised = initialise();

