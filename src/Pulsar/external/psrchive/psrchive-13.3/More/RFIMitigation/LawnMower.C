/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/LawnMower.h"
#include "Pulsar/OnPulseThreshold.h"
#include "Pulsar/GaussianBaseline.h"
#include "Pulsar/BaselineWindow.h"
#include "Pulsar/SmoothMedian.h"

#include "Pulsar/FrequencyIntegrate.h"
#include "Pulsar/BaselineEstimator.h"
#include "Pulsar/PhaseWeight.h"
#include "Pulsar/IntegrationExpert.h"
#include "Pulsar/Profile.h"

#include <math.h>
#include <time.h>
#include <assert.h>
#include <stdlib.h>

using namespace std;

// #define _DEBUG 1

Pulsar::LawnMower::LawnMower ()
{
  // baseline estimator used to find off-pulse mean and variance
  GaussianBaseline* baseline = new GaussianBaseline;

  /* By including points up to 3 sigma, the actual distribution of the
     data will not matter so much.  This is important because the
     difference between the profile and its median smoothed self is
     searched, and the result is not normally distributed. */
  baseline->set_threshold( 3.0 );

  BaselineWindow* window = new BaselineWindow;
  window->set_smooth( new SmoothMedian );
  window->set_median_cut( 4.0 );
  window->get_smooth()->set_turns( Profile::default_duty_cycle );

  baseline->set_initial_baseline( window );

  /* Also, a relatively large number of samples in the median smoothed
     difference profile will be equal to zero.  It is important to 
     remove these points from the computation of the variance. */
  baseline->set_include( include = new PhaseWeight );

  mower = new OnPulseThreshold;

  mower -> set_baseline_estimator( baseline );
  mower -> set_threshold( 4.0 );
  mower -> set_allow_negative( true );

  // risk smoothing out 2% duty cycle profiles
  median_smoothing_turns = 0.02;

  broadband = true;

  srand48 (time(0));
}

Pulsar::LawnMower::~LawnMower ()
{
}

void Pulsar::LawnMower::set_baseline_estimator (BaselineEstimator* b)
{
  mower->set_baseline_estimator (b);
}

Pulsar::BaselineEstimator* Pulsar::LawnMower::get_baseline_estimator () const
{
  return mower->get_baseline_estimator();
}

void Pulsar::LawnMower::set_threshold (float sigma)
{
  mower->set_threshold( sigma );
}

float Pulsar::LawnMower::get_threshold () const
{
  return mower->get_threshold();
}

void Pulsar::LawnMower::set_median_smoothing (float turns)
{
  median_smoothing_turns = turns;
}

float Pulsar::LawnMower::get_median_smoothing () const
{
  return median_smoothing_turns;
}

void Pulsar::LawnMower::set_broadband (bool flag)
{
  broadband = flag;
}

//! One or more preconditions can be added
void Pulsar::LawnMower::add_precondition
( Functor< bool(Profile*,PhaseWeight*) > f )
{
  precondition.push_back( f );
}

bool Pulsar::LawnMower::build_mask (Profile* profile)
{
  assert (profile != 0);

  unsigned nbin = profile->get_nbin();
  include->resize (nbin);
  include->set_all (1.0);

  if (median_smoothing_turns)
  {
#ifndef _DEBUG
    if (Profile::verbose)
#endif
      cerr << "Pulsar::LawnMower::build_mask median smoothing" << endl;
    
    Reference::To<Profile> smoothed = new Profile( *profile );
    SmoothMedian median;
    median.set_turns( median_smoothing_turns );
    median( smoothed );

    Reference::To<Profile> difference = new Profile( *profile );
    difference->diff (smoothed);

    /*
      The difference between the profile and its median smoothed
      difference can contain a large number of zeros (where the profile
      happens to equal the median).  These zeros cause the variance to
      be underestimated, which can mess up the IterativeBaseline
      algorithm; therefore, they should be excluded from consideration.
    */
    for (unsigned ibin=0; ibin<nbin; ibin++)
      if (difference->get_amps()[ibin] == 0.0)
	(*include)[ibin] = 0;
    
    mower->set_Profile( difference );
  }

  else
    mower->set_Profile( profile );

  if (!mowed)
    mowed = new PhaseWeight;

#ifndef _DEBUG
  if (Profile::verbose)
#endif
    cerr << "Pulsar::LawnMower::build_mask computing zap mask" << endl;
  
  mower->get_weight( mowed );

  unsigned total_mowed = 0;
  for (unsigned ibin=0; ibin<nbin; ibin++)
    if ( (*mowed)[ibin] )
      total_mowed ++;

  if (!total_mowed)
    return false;

  for (unsigned i=0; i<precondition.size(); i++)
    if (!precondition[i]( profile, mowed ))
      return false;

  return true;
}

void Pulsar::LawnMower::transform (Integration* subint)
{
#ifndef _DEBUG
  if (Profile::verbose)
#endif
    cerr << "Pulsar::LawnMower::transform" << endl;

  Reference::To<Integration> total = subint->clone();
  total->expert()->pscrunch();

  if (broadband)
  {
    FrequencyIntegrate integrate;
    integrate.set_dedisperse( false );

    integrate (total);

    if (!build_mask( total->get_Profile(0,0) ))
      return;
  }

  GaussianBaseline baseline;
  SmoothMedian median;
  median.set_turns( median_smoothing_turns );

  for (unsigned ichan=0; ichan < subint->get_nchan(); ichan++) try
  {
#ifdef _DEBUG
    cerr << "ichan=" << ichan << "/" << subint->get_nchan() << endl;
#endif

    if (subint->get_weight(ichan) == 0)
      continue;

    if (!broadband && !build_mask( total->get_Profile (0,ichan) ))
      continue;

    Reference::To<PhaseWeight> base = 
      baseline( subint->get_Profile (0,ichan) );

    for (unsigned ipol=0; ipol < subint->get_npol(); ipol++)
    {
      Reference::To<Profile> profile = subint->get_Profile (ipol, ichan);

      base->set_Profile( profile );
      double rms = sqrt( base->get_variance().get_value() );

      Reference::To<Profile> smoothed = new Profile( *profile );
      median( smoothed );

#ifdef _DEBUG
      cerr << ipol << " " << ichan << " rms=" << rms << endl;
#endif

      float* amps = profile->get_amps();
      float* smamps = smoothed->get_amps();

      unsigned nbin = subint->get_nbin ();

      // 2.5 sigma should get most valid baseline samples
      for (unsigned i=0; i<nbin; i++)
      {
	if ( (*mowed)[i] )
	{
	  unsigned count = 0;
	  unsigned ibin = 0;
	  float diff = 0;

	  do
	  {
	    ibin = lrand48() % nbin;
	    count ++;

	    if (count == nbin)
	      throw Error (InvalidState, "Pulsar::LawnMower::transform",
			   "no baseline points available for replacement");

	    diff = amps[ibin] - smamps[ibin];
	  }
	  while ( (*mowed)[ibin] || fabs(diff) > rms * 2.5 );

	  amps[i] = smamps[i] + diff;
	}
      }
    }
  }
  catch (Error& error)
  {
    if (Integration::verbose)
      cerr << "Pulsar::LawnMower::transform failed on ichan=" << ichan
	   << " " << error.get_message() << endl;
    subint->set_weight (ichan, 0.0);
  }
  
}


//! Get the text interface to the configuration attributes
TextInterface::Parser* Pulsar::LawnMower::get_interface ()
{
  return new Interface (this);
}

Pulsar::LawnMower::Interface::Interface (LawnMower* instance)
{
  if (instance)
    set_instance (instance);

  add( &LawnMower::get_median_smoothing,
       &LawnMower::set_median_smoothing,
       "window", "Median smoothing window in turns" );

  add( &LawnMower::get_threshold,
       &LawnMower::set_threshold,
       "cutoff", "Cutoff threshold" );
}
