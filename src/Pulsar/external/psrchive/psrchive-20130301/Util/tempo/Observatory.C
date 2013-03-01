
/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Observatory.h"
#include "tempo++.h"
#include "tempo_impl.h"

#include <math.h>
#include <string.h>

using namespace std;

Tempo::Observatory::Observatory ()
{
  code = 0;
}

char Tempo::Observatory::get_code () const
{
  return code;
}

void Tempo::Observatory::set_code (char c)
{
  code = c;
}

std::string Tempo::Observatory::get_itoa_code () const
{
  return itoa_code;
}

void Tempo::Observatory::set_itoa_code (const std::string& c)
{
  if (c.length() != 2)
    throw Error (InvalidParam, "Tempo::Observatory::set_itoa_code",
		 "code=%s length != 2", c.c_str());

  itoa_code = c;
}

std::string Tempo::Observatory::get_name () const
{
  return name;
}

void Tempo::Observatory::set_name (const std::string& n)
{
  name = n;
}

Tempo::ObservatoryITRF::ObservatoryITRF (double _x, double _y, double _z)
{
  x = _x; y = _y; z = _z;
}


void Tempo::ObservatoryITRF::get_xyz (double& _x, double& _y, double& _z) const
{
  _x = x; _y = y; _z = z;
}

//! Get the latitude and longitude in radians
void Tempo::ObservatoryITRF::get_sph (double& lat,
				      double& lon,
				      double& rad) const
{
#ifdef _DEBUG
  cerr << "Tempo::ObservatoryITRF::get_sph"
          " x=" << x << " y=" << y << " z=" << z << endl;
#endif

  rad = sqrt(x*x + y*y + z*z);
  lat = asin (z/rad);
  lon = atan2 (y, x);
}


/*!
  \param alat geodetic latitude in radians
  \param alon geodetic longitude in radians
  \param elev geodetic elevation in metres
*/
Tempo::ObservatoryIAU1976::ObservatoryIAU1976 (double alat,
					       double alon,
					       double elev)
{
  /*
    Code taken from tempo/src/setup.f

    IAU 1976 flattening f, equatorial radius a
  */

  double aa_f = 1.0/298.257;
  double aa_a = 6378140.0;

  double aa_c = 1.0/sqrt(1.0+(-2.0+aa_f)*aa_f*sin(alat)*sin(alat));
  double aa_arcf = (aa_a*aa_c+elev)*cos(alat);
  double aa_arsf = (aa_a*(1.0-aa_f)*(1.0-aa_f)*aa_c+elev)*sin(alat);

  lat = atan2 (aa_arsf,aa_arcf);
  lon = alon;
  rad = sqrt(aa_arcf*aa_arcf+aa_arsf*aa_arsf);
};

//! Get the geocentric XYZ coordinates in metres
void Tempo::ObservatoryIAU1976::get_xyz (double& x, double& y, double& z) const
{
  x = rad * cos(lon) * cos(lat);
  y = rad * cos(lon) * sin(lat);
  z = rad * sin(lon);
}

//! Get the latitude and longitude in radians
void Tempo::ObservatoryIAU1976::get_sph (double& _lat,
					 double& _lon,
					 double& _rad) const
{
  _lat = lat; _lon = lon; _rad = rad;
};


const Tempo::Observatory*
Tempo::observatory (const string& telescope_name)
{
  load_obsys ();

  // if the name is a single character, then it is likely a tempo site number
  if (telescope_name.length() == 1)
    for (unsigned i=0; i < antennae.size(); i++)
      if (antennae[i]->get_code() == telescope_name[0])
	return antennae[i];

  for (unsigned i=0; i < antennae.size(); i++)
    if (strcasestr( antennae[i]->get_name().c_str(), telescope_name.c_str() ))
      return antennae[i];

  string itoa = itoa_code (telescope_name);

  for (unsigned i=0; i < antennae.size(); i++)
    if (antennae[i]->get_itoa_code() == itoa)
      return antennae[i];

  throw Error (InvalidParam, "Tempo::observatory",
               "no antennae named '" + telescope_name + "'");
}

