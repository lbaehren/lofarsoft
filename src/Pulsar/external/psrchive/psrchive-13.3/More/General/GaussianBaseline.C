/***************************************************************************
 *
 *   Copyright (C) 2005-2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/GaussianBaseline.h"
#include "Pulsar/PeakConsecutive.h"
#include "Pulsar/PhaseWeight.h"
#include "Pulsar/Profile.h"
#include "Pulsar/SmoothMean.h"
#include "NormalDistribution.h"

#include <assert.h>
#include <iostream>

using namespace std;

// #define _DEBUG 1

Pulsar::GaussianBaseline::GaussianBaseline ()
{
  set_threshold (1.0);
  smooth_bins = 4;
}

Pulsar::GaussianBaseline* Pulsar::GaussianBaseline::clone () const
{
  return new GaussianBaseline (*this);
}

void Pulsar::GaussianBaseline::set_smoothing (unsigned nbins)
{
  smooth_bins = nbins;
}

//! Set the threshold below which samples are included in the baseline
void Pulsar::GaussianBaseline::set_threshold (float sigma)
{
  IterativeBaseline::set_threshold (sigma);

  NormalDistribution normal;

  moment_correction = 1.0 / normal.moment2 (-threshold, threshold);
#ifndef _DEBUG
  if (Profile::verbose)
#endif
    cerr << "Pulsar::GaussianBaseline::set_threshold sigma=" << sigma
	 << " correction=" << moment_correction << endl;
}

void Pulsar::GaussianBaseline::get_bounds (PhaseWeight* weight, 
					   float& lower, float& upper)
{
  double mean, var, var_mean;

  weight->stats (profile, &mean, &var, &var_mean);

  // note that stats computes an unbiased estimate of the variance
  double effective_variance = var;

  if (!get_initial_bounds())
    effective_variance *= moment_correction;

#ifndef _DEBUG
  if (Profile::verbose)
#endif
    cerr << "Pulsar::GaussianBaseline::get_bounds"
	 << " baseline var=" << var << " var_mean=" << var_mean
	 << " effective=" << effective_variance << endl;

  double cutoff = threshold * sqrt (effective_variance);

#ifndef _DEBUG
  if (Profile::verbose)
#endif
    cerr << "Pulsar::GaussianBaseline::get_bounds"
	 << " baseline mean=" << mean << " cutoff=" << cutoff
	 << " = " << threshold << " sigma" << endl;

  last_mean = mean;
  last_lower = lower = mean - cutoff;
  last_upper = upper = mean + cutoff;
}

void Pulsar::GaussianBaseline::postprocess (PhaseWeight* weight, 
					    const Profile* profile)
{
  IterativeBaseline::postprocess (weight, profile);

  if (!smooth_bins)
    return;

  assert (weight != 0);
  assert (profile != 0);

  std::vector<unsigned> on_transitions;
  std::vector<unsigned> off_transitions;

  unsigned nbin = weight->get_nbin();
  unsigned consecutive = unsigned( 0.01 * nbin );
  if (!consecutive)
    consecutive = 1;

  float transition = 0.5;

#ifdef _DEBUG
  cerr << "Pulsar::GaussianBaseline::postprocess find regions; consecutive="
       << consecutive << endl;
#endif

  regions( weight->get_nbin(), weight->get_weights(), 0, nbin,
	   consecutive, transition, on_transitions, off_transitions );

  if (off_transitions.size() == 0)
    return;

#ifdef _DEBUG
  cerr << "Pulsar::GaussianBaseline::postprocess smooth bins=" << smooth_bins
       << endl;
#endif

  SmoothMean smoother;
  smoother.set_bins (smooth_bins);
  
  Profile smoothed (profile);
  smoother (&smoothed);

#ifdef _DEBUG
  cerr << "before peel mean=" << weight->get_mean() 
       << " last_mean=" << last_mean 
       << " transitions=" << off_transitions.size() << endl;
#endif

  
  for (unsigned ioff=0; ioff < off_transitions.size(); ioff++)
  {
    unsigned ibin=off_transitions[ioff];
    while ( smoothed.get_amps()[ibin%nbin] > last_mean )
    {
#ifdef _DEBUG
      cerr << "right peel " << ibin%nbin << endl;
#endif
      (*weight)[ibin%nbin] = 0.0;
      ibin ++;
      if (ibin > off_transitions[ioff] + nbin)
        throw Error (InvalidState, "Pulsar::GaussianBaseline::postprocess",
                     "right peel failure on %u transitions",
		      off_transitions.size() );
    }
  }

  for (unsigned ion=0; ion < on_transitions.size(); ion++)
  {
    unsigned ibin=on_transitions[ion] + nbin;
    while ( smoothed.get_amps()[ibin%nbin] > last_mean )
    {
#ifdef _DEBUG
      cerr << "left peel " << ibin%nbin << endl;
#endif
      (*weight)[ibin%nbin] = 0.0;
      ibin --;
      if (ibin < on_transitions[ion] + 1)
        throw Error (InvalidState, "Pulsar::GaussianBaseline::postprocess",
                     "right peel failure on %u transitions",
                     on_transitions.size() );
    }
  }

  weight->set_Profile (profile);

#ifdef _DEBUG
  cerr << "after peel mean=" << weight->get_mean() << endl;
#endif

}

class Pulsar::GaussianBaseline::Interface 
  : public TextInterface::To<GaussianBaseline>
{
public:
  Interface (GaussianBaseline* instance)
  {
    if (instance)
      set_instance (instance);

    add( &GaussianBaseline::get_threshold,
	 &GaussianBaseline::set_threshold,
	 "threshold", "cutoff threshold used to avoid outliers" );

  }
  
  std::string get_interface_name () const { return "normal"; }
};

//! Return a text interface that can be used to configure this instance
TextInterface::Parser* Pulsar::GaussianBaseline::get_interface ()
{
  return new Interface (this);
}
