/**************************************************************************
 *  This file is part of the Transmogrify library.                        *
 *  Copyright (C) 2010 Pim Schellart <P.Schellart@astro.ru.nl>            *
 *                                                                        *
 *  This library is free software: you can redistribute it and/or modify  *
 *  it under the terms of the GNU General Public License as published by  *
 *  the Free Software Foundation, either version 3 of the License, or     *
 *  (at your option) any later version.                                   *
 *                                                                        * 
 *  This library is distributed in the hope that it will be useful,       *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *  GNU General Public License for more details.                          *
 *                                                                        *
 *  You should have received a copy of the GNU General Public License     *
 *  along with this library. If not, see <http://www.gnu.org/licenses/>.  *
 **************************************************************************/

#ifndef TMF_H
#define TMF_H

// SYSTEM INCLUDES
#include <exception>
#include <cmath>
#include <string>

// PROJECT INCLUDES
//

// LOCAL INCLUDES
//

// FORWARD REFERENCES
//

namespace tmf
{
  /* exception handling */
  class Error : public std::exception {};

  class InputError : public Error {};

  /* constants */
  const int SPEED_OF_LIGHT = 299792458;
  const int SECONDS_PER_DAY = 24 * 3600;
  
  /* inlines (for increased speed) */
  inline double deg2rad(const double& a) { return a * M_PI / 180; };

  inline double rad2deg(const double& a) { return a * 180 / M_PI; };

  inline double jd2mjd(const double& jd) { return jd - 2400000.5; };

  inline double mjd2jd(const double& mjd) { return mjd + 2400000.5; };

  /* angles.cc */
  double hms2deg(const int& h, const int& m, const double& s);

  double dms2deg(const int& d, const int& am, const double& as);

  void deg2hms(int& h, int& m, double& s, const double& d);

  void deg2dms(int& d, int& m, double& s, const double& deg);

  double hms2rad(const int& h, const int& m, const double& s);
  
  double dms2rad(const int& d, const int& am, const double& as);

  void rad2hms(int& h, int& m, double& s, const double& r);

  void rad2dms(int& d, int& m, double& s, const double& r);

  double rad2circle(const double& phi);

  double deg2circle(const double& phi);

  std::string rad2hmsrepr(const double& phi, int prec = 4);

  std::string rad2dmsrepr(const double& phi, int prec = 4);

  /* epoch.cc */
  void j20002b1950(double& alpha_B, double& delta_B, const double& alpha_J, const double& delta_J);

  void b19502j2000(double& alpha_J, double& delta_J, const double& alpha_B, const double& delta_B);

  /* cartesian_logpolar.cc */
  void logpolar2cartesian(double& x, double& y, const double& rho, const double& theta);

  void cartesian2logpolar(double& rho, double& theta, const double& x, const double& y);

  /* cartesian_polar.cc */
  void polar2cartesian(double& x, double& y, const double& r, const double& theta);

  void cartesian2polar(double& r, double& theta, const double& x, const double& y);

  /* cartesian_spherical.cc */
  void spherical2cartesian(double& x, double& y, double& z, const double& rho, const double& theta, const double& phi);

  void cartesian2spherical(double& rho, double& theta, double& phi, const double& x, const double& y, const double& z);

  /* cylindrical_spherical.cc */
  void spherical2cylindrical(double r, double h, const double& rho, const double& phi);

  void cylindrical2spherical(double& rho, double& phi, const double& r, const double& h);

  /* equatorial_horizontal.cc */
  void equatorial2horizontal(double& A, double& h, const double& H, const double& delta, const double& phi);

  void horizontal2equatorial(double& H, double& delta, const double& A, const double& h, const double& phi);

  void radec2azel(double &A, double &h, const double &alpha, const double &delta, const double &utc, const double &ut1_utc, const double &L, const double &phi);

  void azel2radec(double &alpha, double &delta, const double &A, const double &h, const double &utc, const double &ut1_utc, const double &L, const double &phi);

  /* jd_date.cc */
  double gregoriandate2jd(const int& y, const int& m, const double& d);

  double juliandate2jd(const int& y, const int& m, const double& d);

  double date2jd(const int& y, const int& m, const double& d);

  void jd2date(int& y, int& m, double& d, const double& jd);

  /* nutation.cc */
  void nutation(double& Dphi, double& Depsilon, const double& jde);

  double meanobliquity(const double& jde);

  /* sidereal_time.cc */
  double gmst(const double& jd);

  double gast(const double& jd, const double& jde);

  double last(const double& jd, const double& jde, const double& L);

  /* equatorial_galactic.cc */
  void equatorial2galactic(double& l, double& b, const double& alpha, const double& delta);

  void galactic2equatorial(double& alpha, double& delta, const double& l, const double& b);

  /* utc.cc */
  int tai_utc(const double& utc);

  double tt_utc(const double& utc);

  /* equatorial_ecliptic.cc */
  void equatorial2ecliptic(double& lambda, double& beta, const double& alpha, const double& delta, const double& jde);

  void ecliptic2equatorial(double& alpha, double& delta, const double& lambda, const double& beta, const double& jde);

  /* itrf_local.cc */
  void itrf2local(double &E, double &N, double& U, const double& x, const double& y, const double& z, const double& ref_x, const double& ref_y, const double& ref_z, const double& ref_lon, const double& ref_lat);
} // End tmf

#endif // TMF_H

