/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Horizon.h"

//! Get the azimuth angle in radians
double Horizon::get_azimuth () const
{
  build ();
  return azimuth;
}

//! Get the elevation angle in radians
double Horizon::get_elevation () const
{
  build ();
  return elevation;
}

//! Get the zenith angle in radians
double Horizon::get_zenith () const
{
  build ();
  return 0.5 * M_PI - elevation;
}

//! Get the name of the mount
std::string Horizon::get_name () const
{
  return "Horizon";
}

//! Get the receptor basis in the reference frame of the observatory
Matrix<3,3,double> Horizon::get_basis (const Vector<3,double>& from) const
{
  /*
    angle toward source in horizontal plane.
    'from' points 180 degrees away from the source.
    this form also ensures that 0 < az < 360 degrees.
  */
  azimuth = M_PI + atan2 (from[1], from[0]);

  /*
    radius projected into horizontal plane
  */
  double r = sqrt (from[1]*from[1] + from[0]*from[0]);

  /*
    elevation angle out of horizontal plane
  */
  elevation = atan (from[2] / r);

  double zenith = 0.5*M_PI - elevation;

  /*
    receptor basis in the frame of the observatory

    start with dish pointing toward zenith,
    x receptor toward North, y receptor toward East,

    rotate about zenith so that North is 180 degrees away from source,
    then tip dish down toward source by zenith angle
  */
  return transpose( rotation (Vector<3,double>::basis(2), azimuth - M_PI) *
		    rotation (Vector<3,double>::basis(1), zenith) );
}
