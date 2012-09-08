/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/OnPulseThreshold.h"
#include "Pulsar/BaselineEstimator.h"
#include "Pulsar/PhaseWeight.h"
#include "Pulsar/Profile.h"

#include <iostream>
using namespace std;

// defined in Profile.C
void nbinify (int& istart, int& iend, int nbin);

//! Default constructor
Pulsar::OnPulseThreshold::OnPulseThreshold ()
{
  threshold = 3.0; // sigma
  allow_negative = false;

  bin_start = bin_end = 0;
  range_specified = false;
}

Pulsar::OnPulseThreshold* Pulsar::OnPulseThreshold::clone () const
{
  return new OnPulseThreshold (*this);
}

//! Set the duty cycle
void Pulsar::OnPulseThreshold::set_threshold (float _threshold)
{
  threshold = _threshold;
}

float Pulsar::OnPulseThreshold::get_threshold () const
{
  return threshold;
}

//! Allow negative on-pulse phase bins
void Pulsar::OnPulseThreshold::set_allow_negative (bool flag)
{
  allow_negative = flag;
}

//! Set the BaselineEstimator used to find the off-pulse phase bins
void Pulsar::OnPulseThreshold::set_baseline_estimator (BaselineEstimator* b)
{
  baseline_estimator = b;
}

//! Get the BaselineEstimator used to find the off-pulse phase bins
const Pulsar::BaselineEstimator* 
Pulsar::OnPulseThreshold::get_baseline_estimator () const
{
  return baseline_estimator;
}

Pulsar::BaselineEstimator*
Pulsar::OnPulseThreshold::get_baseline_estimator ()
{
  return baseline_estimator;
}

//! Set the start and end bins of the search
void Pulsar::OnPulseThreshold::set_range (int start, int end)
{
  bin_start = start;
  bin_end = end;
  range_specified = true;
}

//! Retrieve the PhaseWeight
void Pulsar::OnPulseThreshold::calculate (PhaseWeight* weight) try
{
  if (!profile)
    throw Error (InvalidState, "Pulsar::OnPulseThreshold::calculate",
		 "Profile not set");

  Reference::To<PhaseWeight> baseline;

  if (baseline_estimator)
    baseline = baseline_estimator->baseline (profile);
  else
    baseline = profile->baseline();

  Estimate<double> mean = baseline->get_mean ();
  Estimate<double> rms  = sqrt(baseline->get_variance ());

  if (Profile::verbose)
    cerr << "Pulsar::OnPulseThreshold::calculate baseline"
      " mean=" << mean << " rms=" << rms << endl;

  unsigned nbin = profile->get_nbin();

  weight->resize( nbin );
  weight->set_all( 0.0 );

  int start = 0;
  int stop = nbin;

  if (range_specified)
  {
    start = bin_start;
    stop  = bin_end;
    nbinify (start, stop, nbin);
  }

  const float* amps = profile->get_amps();

  float cutoff = threshold * rms.get_value();

  for (int ibin=start; ibin<stop; ibin++)
  {
    float diff = amps[ibin % nbin] - mean.get_value();
    if (allow_negative)
      diff = fabs(diff);

    if ( diff > cutoff )
      (*weight)[ibin] = 1.0;
  }
}
catch (Error& error)
{
  throw error += "Pulsar::OnPulseThreshold::calculate";
}

class Pulsar::OnPulseThreshold::Interface 
  : public TextInterface::To<OnPulseThreshold>
{
public:
  Interface (OnPulseThreshold* instance)
  {
    if (instance)
      set_instance (instance);

    add( &OnPulseThreshold::get_threshold,
	 &OnPulseThreshold::set_threshold,
	 "threshold", "threshold above which points must fall" );
  }

  std::string get_interface_name () const { return "above"; }  
};

//! Return a text interface that can be used to configure this instance
TextInterface::Parser* Pulsar::OnPulseThreshold::get_interface ()
{
  return new Interface (this);
}
