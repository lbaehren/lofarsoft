/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/EllAngPlot.h"
#include "Pulsar/Polarization.h"
#include "Pulsar/PolnProfile.h"

Pulsar::EllAngPlot::EllAngPlot ()
{
  set_span (90.0);
  get_frame()->get_y_axis()->set_tick( 30.0 );
  get_frame()->get_y_axis()->set_nsub( 2 );
}

void Pulsar::EllAngPlot::get_angles (const Archive* data)
{
  Reference::To<const PolnProfile> profile = get_Stokes (data,isubint,ichan);
  profile->get_ellipticity (angles, threshold);
}

//! Return the label for the y-axis
std::string Pulsar::EllAngPlot::get_ylabel (const Archive* data)
{
  return "Ellipticity Angle (degrees)";
}
