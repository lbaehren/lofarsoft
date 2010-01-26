/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Mount.h"

//#define _DEBUG

#ifdef _DEBUG
#include <iostream>
using namespace std;
#endif

static float unset = -333.0;

Mount::Mount ()
{
  declination = right_ascension = 0;
  longitude = latitude = 0;
  lst = unset;
  hour_angle = unset;
}

Mount::~Mount ()
{
}

void Mount::set_source_coordinates (const sky_coord& pos)
{
  declination = pos.dec().getRadians();
  right_ascension = pos.ra().getRadians();

  lst = unset;
}

void Mount::set_observatory_latitude (double lat)
{
  latitude = lat;

  lst = unset;
}

double Mount::get_observatory_latitude () const
{
  return latitude;
}

void Mount::set_observatory_longitude (double lon)
{
  longitude = lon;

  lst = unset;
}

double Mount::get_observatory_longitude () const
{
  return longitude;
}

void Mount::set_epoch (const MJD& _epoch)
{
  if (epoch == _epoch)
    return;

  epoch = _epoch;

  lst = unset;
  hour_angle = unset;
}

MJD Mount::get_epoch () const
{
  return epoch;
}

//! Set the hour_angle in radians
void Mount::set_hour_angle (double rad)
{
  hour_angle = rad;
  lst = unset;
}

//! Get the hour_angle in radians
double Mount::get_hour_angle () const
{
  build ();
  return hour_angle;
}

//! Set the hour_angle in radians
void Mount::set_local_sidereal_time (double rad)
{
  lst = rad;
  hour_angle = unset;
}

//! Get the LST in radians
double Mount::get_local_sidereal_time () const
{
  build ();
  return lst;
}

void Mount::build () const
{
  if (get_built())
    return;

  if (hour_angle == unset)
  {
    double long_deg = longitude * 180/M_PI;

#ifdef _DEBUG
    cerr << "Mount::build epoch=" << epoch << endl
	 << "Mount::build longitude=" << long_deg << " deg" << endl;
#endif

    if (epoch != MJD::zero)
      // MJD::LST receives longitude in degrees and returns LST in hours
      lst = epoch.LST (long_deg) * M_PI/12.0;

    // compute hour angle in radians
    hour_angle = lst - right_ascension;
  }
  else
  {
#ifdef _DEBUG
    cerr << "Mount::build hour_angle=" << hour_angle << endl;
#endif
    lst = hour_angle + right_ascension;
  }

#ifdef _DEBUG
  cerr << "Mount::build LST=" << lst << " latitude=" << latitude
       << endl
       << "Mount::build RA=" << right_ascension << " Dec=" << declination
       << endl;
#endif

  // Basis pointing to source in celestial reference frame
  source_basis =
    transpose( rotation (Vector<3,double>::basis(0), right_ascension) *
	       rotation (Vector<3,double>::basis(1), -declination) );

  // Basis pointing to observatory in celestial reference frame
  observatory_basis = 
    transpose( rotation (Vector<3,double>::basis(0), lst) *
	       rotation (Vector<3,double>::basis(1), -latitude) );
}

bool Mount::get_built () const
{
  return lst != unset && hour_angle != unset;
}
