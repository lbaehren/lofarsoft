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
  mower -> set_threshold( cutoff_threshold );
  mower -> set_allow_negative( true );
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
  Mower::set_threshold( sigma );
}

void Pulsar::LawnMower::compute (PhaseWeight* mask,
				 const Profile* profile)
{
  const unsigned nbin = profile->get_nbin();

  include->resize (nbin);
  include->set_all (1.0);

  /*
    The difference between the profile and its median smoothed
    difference can contain a large number of zeros (where the profile
    happens to equal the median).  These zeros cause the variance to
    be underestimated, which can mess up the IterativeBaseline
    algorithm; therefore, they should be excluded from consideration.
  */
  for (unsigned ibin=0; ibin<nbin; ibin++)
    if (profile->get_amps()[ibin] == 0.0)
      (*include)[ibin] = 0;
    
  mower->set_Profile( profile );
  mower->get_weight( mask );
}
