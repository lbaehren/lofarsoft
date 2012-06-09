/***************************************************************************
 *
 *   Copyright (C) 2003-2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/InstrumentInfo.h"
#include "Pulsar/Instrument.h"

using namespace std;

//! Constructor
Pulsar::InstrumentInfo::InstrumentInfo (const PolnCalibrator* calibrator) :
  BackendFeedInfo (calibrator)
{
  if (Calibrator::verbose)
    cerr << "Pulsar::InstrumentInfo ctor" << endl;

  fixed_orientation = false;

  unsigned nchan = calibrator->get_nchan ();
  
  // find the first valid transformation
  const MEAL::Complex2* xform = 0;
  for (unsigned ichan = 0; ichan < nchan; ichan++)
  {
    if ( calibrator->get_transformation_valid (ichan) )
    {
      xform = calibrator->get_transformation (ichan);
      break;
    }
  }

  if (!xform)
    throw Error (InvalidParam, "Pulsar::InstrumentInfo::InstrumentInfo",
                 "no valid transformation in PolnCalibrator");

  const Calibration::Instrument* instrument;
  instrument = dynamic_cast<const Calibration::Instrument*> (xform);

  if (!instrument)
    throw Error (InvalidParam, "Pulsar::InstrumentInfo::InstrumentInfo",
                 "first valid transformation is not an Instrument");

  if (instrument->get_orientation(0).var == 0)
  {
    // cerr << "Pulsar::InstrumentInfo orientation[0] set to zero" << endl;
    fixed_orientation = true;
  }
}

string Pulsar::InstrumentInfo::get_title () const
{
  return "Parameterization: van Straten (2004) Equation 18";
}

//! Return the name of the specified class
string Pulsar::InstrumentInfo::get_name_feed (unsigned iclass) const
{ 
  switch (iclass)
  {
  case 0:
    return "\\fi\\ge\\dk\\u\\fr (deg.)";
  case 1: {
    if (fixed_orientation)
      return "\\fi\\gh\\fr\\d1\\u (deg.)";
    else
      return "\\fi\\gh\\dk\\u\\fr (deg.)";
  }
  default:
    return "";
  }
}


//! Return the estimate of the specified parameter
Estimate<float> 
Pulsar::InstrumentInfo::get_param_feed (unsigned ichan, unsigned iclass,
					unsigned iparam) const
{
  const MEAL::Complex2* xform = calibrator->get_transformation (ichan);

  const Calibration::Instrument* instrument;
  instrument = dynamic_cast<const Calibration::Instrument*> (xform);

  if (!instrument)
    throw Error (InvalidState, "Pulsar::InstrumentInfo::get_param",
		 "transformation[%d] is not an Instrument", ichan);

  if (iclass < 2)
  {
    Estimate<double> angle;
    if (iclass == 1)
      angle = instrument->get_orientation(iparam);
    else
      angle = instrument->get_ellipticity(iparam);
  
    return 180.0 / M_PI * angle;
  }

  return 0;
}
