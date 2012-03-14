/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/LastHarmonic.h"

#include "Pulsar/ExponentialBaseline.h"
#include "Pulsar/PhaseWeight.h"

#include "Pulsar/Profile.h"

#include <iostream>

using namespace std;

//! Default constructor
Pulsar::LastHarmonic::LastHarmonic ()
{
  bin_rise = bin_fall = 0;
  built = false;

  baseline_estimator = new ExponentialBaseline;

  set_threshold (3.0);
  set_consecutive (3);
}

Pulsar::LastHarmonic* Pulsar::LastHarmonic::clone () const
{
  return new LastHarmonic (*this);
}

void Pulsar::LastHarmonic::set_Profile (const Profile* p)
{
  profile = p;
  built = false;
}

void Pulsar::LastHarmonic::set_threshold (float threshold)
{
  // add one to threshold because mean = rms in exponential distribution
  threshold += 1.0;

  if (threshold != significant.get_threshold())
  {
    built = false;
    significant.set_threshold (threshold);
  }
}

float Pulsar::LastHarmonic::get_threshold () const
{
  return significant.get_threshold();
}

void Pulsar::LastHarmonic::set_consecutive (unsigned consecutive)
{
  if (consecutive != significant.get_consecutive())
  {
    built = false;
    significant.set_consecutive (consecutive);
  }
}

unsigned Pulsar::LastHarmonic::get_consecutive () const
{
  return significant.get_consecutive();
}

//
unsigned Pulsar::LastHarmonic::get_last_harmonic () const
{
  return significant.get();
}

//! Set the BaselineEstimator used to find the off-pulse phase bins
void Pulsar::LastHarmonic::set_baseline_estimator (BaselineEstimator* b)
{
  baseline_estimator = b;
  built = false;
}

//! Get the BaselineEstimator used to find the off-pulse phase bins
const Pulsar::BaselineEstimator* 
Pulsar::LastHarmonic::get_baseline_estimator () const
{
  return baseline_estimator;
}

Pulsar::BaselineEstimator*
Pulsar::LastHarmonic::get_baseline_estimator ()
{
  return baseline_estimator;
}

const Pulsar::PhaseWeight* Pulsar::LastHarmonic::get_baseline () const
{
  return baseline;
}

void Pulsar::LastHarmonic::build ()
{
  if (!profile)
    throw Error (InvalidState, "Pulsar::LastHarmonic::build",
		 "Profile not set");

  if (!baseline_estimator)
    throw Error (InvalidState, "Pulsar::LastHarmonic::build",
		 "Baseline estimator not set");

  Reference::To<PhaseWeight> baseline = baseline_estimator->baseline (profile);

  Estimate<double> mean = baseline->get_mean ();
  Estimate<double> var = baseline->get_variance ();
  Estimate<double> rms = sqrt(var);

#ifdef _DEBUG
  cerr << "Pulsar::LastHarmonic::build mean=" << mean
       << " rms=" << rms << endl;
#endif

  significant.find (profile, rms.get_value());

  // skip the DC term
  bin_rise = 1;
  bin_fall = significant.get();
}


/*!
  \retval rise bin at which the cumulative power last remains above threshold
  \retval fall bin at which the cumulative power first falls below threshold
*/
void Pulsar::LastHarmonic::get_indeces (int& rise, int& fall) const
{
  if (!built)
    const_cast<LastHarmonic*>(this)->build ();

  rise = bin_rise;
  fall = bin_fall;
}

class Pulsar::LastHarmonic::Interface : public TextInterface::To<LastHarmonic>
{
public:
  Interface (LastHarmonic* instance)
  {
    if (instance)
      set_instance (instance);

    add( &LastHarmonic::get_threshold,
	 &LastHarmonic::set_threshold,
	 "threshold", "threshold above which consecutive points must fall" );

    add( &LastHarmonic::get_consecutive,
	 &LastHarmonic::set_consecutive,
	 "consecutive", "consecutive points required above threshold" );
  }
};

//! Return a text interface that can be used to configure this instance
TextInterface::Parser* Pulsar::LastHarmonic::get_interface ()
{
  return new Interface (this);
}
