/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/CalibratorTypes.h"

#include "Pulsar/SingleAxisCalibrator.h"
#include "Pulsar/PolarCalibrator.h"

Pulsar::Calibrator::Type* 
Pulsar::Calibrator::Type::factory (const Calibrator* calibrator)
{
  if ( calibrator->is_a<SingleAxisCalibrator>() )
    return new CalibratorTypes::SingleAxis;

  if ( calibrator->is_a<PolarCalibrator>() )
    return new CalibratorTypes::van02_EqA1;

  throw Error (InvalidState, "Pulsar::Calibrator::Type::factory",
	       "unknown calibrator type");
}
