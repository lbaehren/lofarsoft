/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/BaselineEstimator.h"
#include "Pulsar/PhaseWeight.h"
#include "Pulsar/Profile.h"

#include <iostream>
using namespace std;

Pulsar::Option<float> default_median_cut
(
 "Baseline::median_cut", 0.0,

 "Remove outliers from baseline using median",

 "Large outliers can skew the estimate of the off-pulse baseline mean and \n"
 "standard deviation.  This parameter can be used to omit any phase bins \n"
 "that are more than median_cut times the median difference from the \n"
 "median from the median. [not a typo]"
);

Pulsar::BaselineEstimator::BaselineEstimator ()
{
  /*
   * cannot set to default in constructor, as Option<float> default_median_cut
   * may not yet have been constructed.
   */
  
  median_cut = -1;
}

void Pulsar::BaselineEstimator::set_median_cut (float cut)
{
  median_cut = cut;
}

Pulsar::PhaseWeight*
Pulsar::BaselineEstimator::baseline (const Profile* profile)
{
  Reference::To<PhaseWeight> weight = new PhaseWeight;
  set_Profile( profile );
  get_weight( weight );

  if (median_cut == -1)
    median_cut = default_median_cut;

  if (median_cut)
  {
    float median = weight->get_median();
    float median_diff = weight->get_median_difference();

    unsigned nbin = profile->get_nbin();
    const float* amps = profile->get_amps();

    for (unsigned ibin=0; ibin<nbin; ibin++)
    {
      if ( (*weight)[ibin] )
      {
	float diff = fabs( amps[ibin] - median );
	if ( diff > median_cut * median_diff )
	  (*weight)[ibin] = 0;
      }
    }
  }

  return weight.release();
}

Pulsar::PhaseWeight*
Pulsar::BaselineEstimator::operator () (const Profile* profile)
{
  return baseline (profile);
}

//! Include only the specified phase bins for consideration
void Pulsar::BaselineEstimator::set_include (PhaseWeight* _include)
{
  include = _include;
}


#include "Pulsar/ExponentialBaseline.h"
#include "Pulsar/GaussianBaseline.h"
#include "Pulsar/BaselineWindow.h"

//! Construct a new BaselineEstimator from a string
Pulsar::BaselineEstimator* 
Pulsar::BaselineEstimator::factory (const std::string& name_parse)
{
  std::vector< Reference::To<BaselineEstimator> > instances;

  instances.push_back( new ExponentialBaseline );
  instances.push_back( new GaussianBaseline );
  instances.push_back( new BaselineWindow );

  return TextInterface::factory<BaselineEstimator> (instances, name_parse);
}
