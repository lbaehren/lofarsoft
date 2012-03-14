/***************************************************************************
 *
 *   Copyright (C) 2006-2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

using namespace std;

#include "Pulsar/FaradayRotation.h"

#include "Pulsar/Integration.h"
#include "Pulsar/Archive.h"
#include "Pulsar/PolnProfile.h"

#include <iostream>

Pulsar::FaradayRotation::FaradayRotation ()
{
  name = "FaradayRotation";
  val = "RM";
}

double 
Pulsar::FaradayRotation::get_correction_measure (const Integration* data)
{
  if (Archive::verbose > 2)
    cerr << "Pulsar::FaradayRotation::get_correction_measure RM="
         << data->get_dispersion_measure () << endl;

  return data->get_rotation_measure ();
}

bool Pulsar::FaradayRotation::get_corrected (const Integration* data)
{
  return data->get_faraday_corrected();
}

//! Execute the correction for an entire Pulsar::Archive
void Pulsar::FaradayRotation::execute (Archive* arch)
{
  ColdPlasma<Calibration::Faraday,DeFaraday>::execute (arch);
  arch->set_rotation_measure( get_rotation_measure() );
  arch->set_faraday_corrected( true );
}

//! Undo the correction for an entire Pulsar::Archive
void Pulsar::FaradayRotation::revert (Archive* arch)
{
  ColdPlasma<Calibration::Faraday,DeFaraday>::revert (arch);
  arch->set_faraday_corrected( false );
}

void Pulsar::FaradayRotation::apply (Integration* data, unsigned ichan) try
{
  Reference::To<PolnProfile> poln_profile = data->new_PolnProfile (ichan);

  Jones<double> xform = inv(delta * corrector.evaluate());

#ifdef _DEBUG
  cerr << "Pulsar::FaradayRotation::apply ichan=" << ichan 
       << " det(xform)=" << det(corrector.evaluate()) << endl;
#endif

  poln_profile->transform( xform );
}
catch (Error& error) {
  throw error += "Pulsar::FaradayRotation::apply";
}
