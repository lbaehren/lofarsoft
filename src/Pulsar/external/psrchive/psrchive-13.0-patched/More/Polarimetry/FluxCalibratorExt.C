/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/FluxCalibratorExtension.h"
#include "Pulsar/FluxCalibratorData.h"
#include "Pulsar/FluxCalibrator.h"

using namespace std;

//! Construct from a FluxCalibrator instance
Pulsar::FluxCalibratorExtension::FluxCalibratorExtension
(const FluxCalibrator* calibrator) 
  : CalibratorExtension ("FluxCalibratorExtension")
{
  if (!calibrator)
    throw Error (InvalidParam, "Pulsar::FluxCalibratorExtension",
                 "null FluxCalibrator*");

  try {

    if (Archive::verbose > 2)
      cerr << "Pulsar::FluxCalibratorExtension(FluxCalibrator*)" << endl;

    CalibratorExtension::build (calibrator);

    unsigned nchan = calibrator->get_nchan();
    set_nchan (nchan);

    for (unsigned ichan=0; ichan < nchan; ichan++) try {
      calibrator->data[ichan].get (S_sys[ichan], S_cal[ichan]);
    }
    catch (Error& error) {
      if (Archive::verbose > 2)
        cerr << "Pulsar::FluxCalibratorExtension constructor ichan=" << ichan
             << "\n\t" << error.get_message() << endl;
    }
  }
  catch (Error& error) {
    throw error += "Pulsar::FluxCalibratorExtension (FluxCalibrator*)";
  }

}
