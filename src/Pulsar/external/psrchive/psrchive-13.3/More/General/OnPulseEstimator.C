/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/OnPulseThreshold.h"
#include "Pulsar/PeakConsecutive.h"
#include "Pulsar/PeakCumulative.h"

//! Construct a new BaselineEstimator from a string
Pulsar::OnPulseEstimator* 
Pulsar::OnPulseEstimator::factory (const std::string& name_parse)
{
  std::vector< Reference::To<OnPulseEstimator> > instances;

  instances.push_back( new OnPulseThreshold );
  instances.push_back( new PeakConsecutive );
  instances.push_back( new PeakCumulative );

  return TextInterface::factory<OnPulseEstimator> (instances, name_parse);
}
