/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include <math.h>

#include "Pulsar/AdaptiveSNR.h"
#include "Pulsar/BaselineEstimator.h"
#include "Pulsar/PhaseWeight.h"
#include "Pulsar/Profile.h"

using namespace std;

//! Default constructor
Pulsar::AdaptiveSNR::AdaptiveSNR ()
{
}

//! Destructor
Pulsar::AdaptiveSNR::~AdaptiveSNR ()
{
}


//! Set the threshold below which samples are included in the baseline
void Pulsar::AdaptiveSNR::set_baseline_estimator (BaselineEstimator* function)
{
  baseline_estimator = function;
}


//! Return the signal to noise ratio
float Pulsar::AdaptiveSNR::get_snr (const Profile* profile)
{
  Reference::To<PhaseWeight> baseline;

  if (baseline_estimator)
    baseline = baseline_estimator->baseline (profile);
  else
    baseline = profile->baseline();

  Estimate<double> mean = baseline->get_mean();
  Estimate<double> variance = baseline->get_variance();

  double rms = sqrt (variance.val);
  unsigned nbin = profile->get_nbin();
  double energy = profile->sum() - mean.val * nbin;

  double total = baseline->get_weight_sum();
  double max   = baseline->get_weight_max();

  double on_pulse = nbin * max - total;
  double snr = energy / sqrt(variance.val*on_pulse);

  if (Profile::verbose) {

    cerr << "Pulsar::AdaptiveSNR::get_snr " << total << " out of " << nbin*max
	 << " bins in baseline\n  mean=" << mean << " rms=" << rms 
	 << " energy=" << energy << " S/N=" << snr << endl;

  }

  if (energy < 0)
    return 0.0;

  return snr;

}    

