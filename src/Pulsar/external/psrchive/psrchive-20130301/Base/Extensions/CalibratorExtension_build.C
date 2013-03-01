/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/CalibratorExtension.h"
#include "Pulsar/Calibrator.h"
#include "Pulsar/Integration.h"

using namespace std;

//! Construct from a Calibrator instance
void Pulsar::CalibratorExtension::build (const Calibrator* calibrator)
{
  if (!calibrator)
    throw Error (InvalidParam, "Pulsar::CalibratorExtension::build",
                 "null Calibrator*");

  try {

    if (Archive::verbose == 3)
      cerr << "Pulsar::CalibratorExtension::build" << endl;

    set_type ( calibrator->get_type()  );
    set_epoch( calibrator->get_epoch() );
    set_nchan( calibrator->get_nchan() );

    const Integration* subint = calibrator->get_Archive()->get_Integration(0);

    unsigned nchan = get_nchan();
    for (unsigned ichan=0; ichan < nchan; ichan++) {
      weight[ichan] = subint->get_weight(ichan);
      centre_frequency[ichan] = subint->get_centre_frequency(ichan);
    }

  }
  catch (Error& error) {
    throw error += "Pulsar::CalibratorExtension (Calibrator*)";
  }

}
