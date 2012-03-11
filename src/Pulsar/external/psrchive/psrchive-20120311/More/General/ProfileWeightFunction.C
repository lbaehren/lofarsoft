/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/ProfileWeightFunction.h"
#include "Pulsar/PhaseWeight.h"
#include "Pulsar/Profile.h"

Pulsar::ProfileWeightFunction::ProfileWeightFunction ()
{
}

//! Set the Profile from which the PhaseWeight will be derived
void Pulsar::ProfileWeightFunction::set_Profile (const Profile* _profile)
{
  profile = _profile;
}

//! Returns a PhaseWeight with the Profile attribute set
void Pulsar::ProfileWeightFunction::get_weight (PhaseWeight* weight)
{
  calculate (weight);
  weight->set_frequency( profile->get_centre_frequency () );
  weight->set_Profile( profile );
}

// on-pulse estimators
#include "Pulsar/OnPulseThreshold.h"
#include "Pulsar/PeakConsecutive.h"
#include "Pulsar/PeakCumulative.h"

// either-or estimators
#include "Pulsar/ProfileWeightStatic.h"

// baseline estimators
#include "Pulsar/BaselineWindow.h"
#include "Pulsar/ExponentialBaseline.h"
#include "Pulsar/GaussianBaseline.h"

Pulsar::ProfileWeightFunction* 
Pulsar::ProfileWeightFunction::factory (const std::string& name_parse)
{
  std::vector< Reference::To<ProfileWeightFunction> > instances;

  instances.push_back( new OnPulseThreshold );
  instances.push_back( new PeakConsecutive );
  instances.push_back( new PeakCumulative );

  instances.push_back( new ProfileWeightStatic );

  instances.push_back( new ExponentialBaseline );
  instances.push_back( new GaussianBaseline );
  instances.push_back( new BaselineWindow );

  return TextInterface::factory<ProfileWeightFunction> (instances, name_parse);
}
