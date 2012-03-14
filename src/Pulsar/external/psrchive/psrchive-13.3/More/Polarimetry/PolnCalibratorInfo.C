/***************************************************************************
 *
 *   Copyright (C) 2003-2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/CalibratorTypes.h"

#include "Pulsar/SingleAxisCalibrator.h"
#include "Pulsar/PolarCalibrator.h"
#include "Pulsar/InstrumentInfo.h"
#include "Pulsar/BrittonInfo.h"

using namespace std;

Pulsar::PolnCalibrator::Info* 
Pulsar::PolnCalibrator::Info::create (const Pulsar::PolnCalibrator* calibrator)
{
  if (verbose > 2)
    cerr << "Pulsar::PolnCalibrator::Info::create type="
	 << calibrator->get_type()->get_name() << endl;

  if (calibrator->get_type()->is_a<CalibratorTypes::SingleAxis>())
    return new SingleAxisCalibrator::Info (calibrator);

  if (calibrator->get_type()->is_a<CalibratorTypes::van02_EqA1>() ||
      calibrator->get_type()->is_a<CalibratorTypes::van09_Eq>())
    return new PolarCalibrator::Info (calibrator);

  if (calibrator->get_type()->is_a<CalibratorTypes::van04_Eq18>())
    return new InstrumentInfo (calibrator);

  if (calibrator->get_type()->is_a<CalibratorTypes::bri00_Eq19>())
    return new BrittonInfo (calibrator);

  return new PolnCalibrator::Info (calibrator);
}
