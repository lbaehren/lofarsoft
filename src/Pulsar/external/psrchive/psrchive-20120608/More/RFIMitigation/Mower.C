/***************************************************************************
 *
 *   Copyright (C) 2012 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Mower.h"
#include "Pulsar/SmoothMedian.h"

#include "Pulsar/FrequencyIntegrate.h"
#include "Pulsar/PhaseWeight.h"
#include "Pulsar/IntegrationExpert.h"
#include "Pulsar/Profile.h"

#include <math.h>
#include <time.h>
#include <assert.h>
#include <stdlib.h>

using namespace std;

// #define _DEBUG 1

Pulsar::Mower::Mower ()
{
  // risk smoothing out 2% duty cycle profiles
  median_smoothing_turns = 0.02;

  // cutoff threshold
  cutoff_threshold = 4.0;

  broadband = true;

  srand48 (time(0));
}

void Pulsar::Mower::set_threshold (float sigma)
{
  cutoff_threshold = sigma;
}

float Pulsar::Mower::get_threshold () const
{
  return cutoff_threshold;
}

void Pulsar::Mower::set_median_smoothing (float turns)
{
  median_smoothing_turns = turns;
}

float Pulsar::Mower::get_median_smoothing () const
{
  return median_smoothing_turns;
}

void Pulsar::Mower::set_broadband (bool flag)
{
  broadband = flag;
}

void Pulsar::Mower::set_prune (const PhaseWeight* prune_mask)
{
  prune = prune_mask;
}

const Pulsar::PhaseWeight* Pulsar::Mower::get_prune () const
{
  return prune;
}

//! One or more preconditions can be added
void Pulsar::Mower::add_precondition
( Functor< bool(Profile*,PhaseWeight*) > f )
{
  precondition.push_back( f );
}

#define sqr(x) ((x)*(x))

bool Pulsar::Mower::build_mask (Profile* profile)
{
  assert (profile != 0);

  if (median_smoothing_turns <= 0.0)
    throw Error (InvalidState, "Pulsar::Mower::build_mask",
		 "invalid median smoothing window = %f",
		 median_smoothing_turns);

  unsigned nbin = profile->get_nbin();

#ifndef _DEBUG
  if (Profile::verbose)
#endif
    cerr << "Pulsar::Mower::build_mask median smoothing" << endl;
    
  Reference::To<Profile> smoothed = new Profile( *profile );
  SmoothMedian median;
  median.set_turns( median_smoothing_turns );
  median( smoothed );

  Reference::To<Profile> difference = new Profile( *profile );
  difference->diff (smoothed);

  if (!mowed)
    mowed = new PhaseWeight;

  mowed->resize (nbin);

  compute (mowed, difference);

#ifndef _DEBUG
  if (Profile::verbose)
#endif
    cerr << "Pulsar::Mower::build_mask computing zap mask" << endl;

  float* amps = difference->get_amps();

  unsigned total_mowed = 0;
  double sumsq=0;

  for (unsigned ibin=0; ibin<nbin; ibin++)
  {
    if ( (*mowed)[ibin] )
      total_mowed ++;
    else
      sumsq = sqr(amps[ibin]);
  }

#if _DEBUG
  sumsq /= (nbin - total_mowed);
  cerr << "total mowed=" << total_mowed << endl;
  cerr << "rms=" << sqrt(sumsq) << " mad=" << madm << endl;
#endif

  if (!total_mowed)
    return false;

  for (unsigned i=0; i<precondition.size(); i++)
    if (!precondition[i]( profile, mowed ))
      return false;

  return true;
}

void Pulsar::Mower::transform (Integration* subint)
{
#ifndef _DEBUG
  if (Profile::verbose)
#endif
    cerr << "Pulsar::Mower::transform" << endl;

  const unsigned nbin = subint->get_nbin ();

  Reference::To<Integration> total = subint->clone();
  total->expert()->pscrunch();

  if (broadband)
  {
    FrequencyIntegrate integrate;
    integrate.set_dedisperse( false );

    integrate (total);

    if (!build_mask( total->get_Profile(0,0) ) && !prune)
      return;
  }

  SmoothMedian median;
  median.set_turns( median_smoothing_turns );

  for (unsigned ichan=0; ichan < subint->get_nchan(); ichan++) try
  {
#ifdef _DEBUG
    cerr << "ichan=" << ichan << "/" << subint->get_nchan() << endl;
#endif

    if (subint->get_weight(ichan) == 0)
      continue;

    if (!broadband && !build_mask( total->get_Profile (0,ichan) ) && !prune)
      continue;

    for (unsigned ipol=0; ipol < subint->get_npol(); ipol++)
    {
      Reference::To<Profile> profile = subint->get_Profile (ipol, ichan);
      Reference::To<Profile> smoothed = new Profile( *profile );
      median( smoothed );

#ifdef _DEBUG
      cerr << ipol << " " << ichan << " rms=" << rms << endl;
#endif

      float* amps = profile->get_amps();
      float* smamps = smoothed->get_amps();

      for (unsigned i=0; i<nbin; i++)
      {
	if( (prune && (*prune)[i]) || (!prune && (*mowed)[i]) )
	{
	  unsigned count = 0;
	  unsigned ibin = 0;
	  float diff = 0;

	  do
	  {
	    ibin = lrand48() % nbin;
	    count ++;

	    if (count == 4*nbin)
	      throw Error (InvalidState, "Pulsar::Mower::transform",
			   "no baseline points available for replacement");

	    diff = amps[ibin] - smamps[ibin];
	  }
	  while ( (*mowed)[ibin] || (prune && (*prune)[ibin]) );

	  amps[i] = smamps[i] + diff;
	}
      }
    }
  }
  catch (Error& error)
  {
    if (Integration::verbose)
      cerr << "Pulsar::Mower::transform failed on ichan=" << ichan
	   << " " << error.get_message() << endl;
    subint->set_weight (ichan, 0.0);
  }
  
}


//! Get the text interface to the configuration attributes
TextInterface::Parser* Pulsar::Mower::get_interface ()
{
  return new Interface (this);
}

Pulsar::Mower::Interface::Interface (Mower* instance)
{
  if (instance)
    set_instance (instance);

  add( &Mower::get_median_smoothing,
       &Mower::set_median_smoothing,
       "window", "Median smoothing window in turns" );

  add( &Mower::get_threshold,
       &Mower::set_threshold,
       "cutoff", "Cutoff threshold" );
}
