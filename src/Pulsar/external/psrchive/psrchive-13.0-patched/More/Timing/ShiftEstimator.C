/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/ShiftEstimator.h"

#include "Pulsar/FourierDomainFit.h"
#include "Pulsar/GaussianShift.h"
#include "Pulsar/ParIntShift.h"
#include "Pulsar/PhaseGradShift.h"
#include "Pulsar/SincInterpShift.h"
#include "Pulsar/ZeroPadShift.h"

bool Pulsar::ShiftEstimator::verbose = false;

//! Construct a new ShiftEstimator from a string
Pulsar::ShiftEstimator* 
Pulsar::ShiftEstimator::factory (const std::string& name_parse)
{
  std::vector< Reference::To<ShiftEstimator> > instances;

  instances.push_back( new FourierDomainFit );
  instances.push_back( new GaussianShift );
  instances.push_back( new ParIntShift );
  instances.push_back( new PhaseGradShift );
  instances.push_back( new SincInterpShift );
  instances.push_back( new ZeroPadShift );

  // instances.push_back( new  );

  return TextInterface::factory<ShiftEstimator> (instances, name_parse);
}
