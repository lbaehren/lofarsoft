/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/ConvertMJD.h"

//! Default constructor
Calibration::ConvertMJD::ConvertMJD ()
{
  scale = 1.0;
}

//! Set the epoch
void Calibration::ConvertMJD::set_epoch (const MJD& epoch)
{
  double value = (epoch - reference_epoch).in_days() * scale;
  set_value (value);
}

//! Set the reference MJD
void Calibration::ConvertMJD::set_reference_epoch (const MJD& mjd)
{
  reference_epoch = mjd;
}

MJD Calibration::ConvertMJD::get_reference_epoch () const
{
  return reference_epoch;
}

//! Set the scale to be used during conversion
void Calibration::ConvertMJD::set_scale (double _scale)
{
  scale = _scale;
}
