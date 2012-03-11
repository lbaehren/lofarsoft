/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Parallactic.h"
#include "Directional.h"

using namespace std;

//! Default constructor
Calibration::Parallactic::Parallactic ()
  : Rotation1( Vector<3,double>::basis(2) )
{
  // the rotation angle is not a free parameter
  set_infit (0, false);
}

//! Set the directional antenna
void Calibration::Parallactic::set_directional (Directional* d)
{
  directional = d;
  if (directional)
    set_phi( directional->get_vertical() );
}

//! Get the directional antenna
Directional* Calibration::Parallactic::get_directional ()
{
  return directional;
}

//! Get the directional antenna
const Directional* Calibration::Parallactic::get_directional () const
{
  return directional;
}

void Calibration::Parallactic::set_epoch (const MJD& epoch)
{
  if (verbose)
    cerr << "Calibration::Parallactic::set_epoch " << epoch << endl;

  directional->set_epoch (epoch);
  set_phi( directional->get_vertical() );
}

MJD Calibration::Parallactic::get_epoch () const
{
  return directional->get_epoch ();
}

//! Set the hour angle in radians
void Calibration::Parallactic::set_hour_angle (double radians)
{
  if (verbose)
    cerr << "Calibration::Parallactic::set_hour_angle " << radians << endl;

  directional->set_hour_angle (radians);

  set_phi( directional->get_vertical() );

  if (verbose)
    cerr << "Calibration::Parallactic::set_hour_angle"
      " phi=" << get_phi() << endl;

}

//! Get the hour angle in radians
double Calibration::Parallactic::get_hour_angle () const
{
  return directional->get_hour_angle ();
}
