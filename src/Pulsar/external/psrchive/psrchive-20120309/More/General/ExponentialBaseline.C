/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/ExponentialBaseline.h"
#include "Pulsar/PhaseWeight.h"
#include "Pulsar/Profile.h"
#include "ExponentialDistribution.h"

#include <iostream>
using namespace std;

// #define _DEBUG 1

Pulsar::ExponentialBaseline::ExponentialBaseline ()
{
  set_threshold (1.0);
}

Pulsar::ExponentialBaseline* Pulsar::ExponentialBaseline::clone () const
{
  return new ExponentialBaseline (*this);
}

//! Set the threshold below which samples are included in the baseline
void Pulsar::ExponentialBaseline::set_threshold (float sigma)
{
  IterativeBaseline::set_threshold (sigma);

  ExponentialDistribution exponential;

  moment_correction = 1.0 / exponential.cumulative_mean (threshold);
#ifndef _DEBUG
  if (Profile::verbose)
#endif
    cerr << "Pulsar::ExponentialBaseline::set_threshold sigma=" << sigma
	 << " correction=" << moment_correction << endl;
}

void Pulsar::ExponentialBaseline::get_bounds (PhaseWeight* weight, 
					      float& lower, float& upper)
{
  weight->set_Profile (profile);

#ifdef _DEBUG
  cerr << "Pulsar::ExponentialBaseline::get_bounds mean=" << weight->get_mean()
       << " sigma=" << sqrt(weight->get_variance()) << endl;
#endif

  lower = 0.0;
  upper = threshold * weight->get_mean().get_value();

  if (!get_initial_bounds())
    upper *= moment_correction;

}

class Pulsar::ExponentialBaseline::Interface 
  : public TextInterface::To<ExponentialBaseline>
{
public:
  Interface (ExponentialBaseline* instance)
  {
    if (instance)
      set_instance (instance);

    add( &ExponentialBaseline::get_threshold,
	 &ExponentialBaseline::set_threshold,
	 "threshold", "cutoff threshold used to avoid outliers" );
  }

  std::string get_interface_name () const { return "exponential"; }
};

//! Return a text interface that can be used to configure this instance
TextInterface::Parser* Pulsar::ExponentialBaseline::get_interface ()
{
  return new Interface (this);
}
