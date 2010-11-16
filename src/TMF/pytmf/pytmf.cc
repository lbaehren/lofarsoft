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

// SYSTEM INCLUDES
#include <boost/python.hpp>

// PROJECT INCLUDES
#include <tmf.h>

// LOCAL INCLUDES
//

// FORWARD REFERENCES
//

using namespace boost::python;

// Functions which require python specific code
namespace pytmf
{
  tuple deg2hms(const double& deg)
  {
    int h, m;
    double s;

    tmf::deg2hms(h, m, s, deg);

    return make_tuple(h, m, s);
  }

  tuple deg2dms(const double& deg)
  {
    int d, am;
    double as;

    tmf::deg2dms(d, am, as, deg);

    return make_tuple(d, am, as);
  }

  tuple rad2hms(const double& r)
  {
    int h, m;
    double s;

    tmf::rad2hms(h, m, s, r);

    return make_tuple(h, m, s);
  }

  tuple rad2dms(const double& r)
  {
    int d, m;
    double s;

    tmf::rad2dms(d, m, s, r);

    return make_tuple(d, m, s);
  }

  tuple j20002b1950(const double& alpha_J, const double& delta_J)
  {
    double alpha_B, delta_B;

    tmf::j20002b1950(alpha_B, delta_B, alpha_J, delta_J);

    return make_tuple(alpha_B, delta_B);
  }

  tuple b19502j2000(const double& alpha_B, const double& delta_B)
  {
    double alpha_J, delta_J;

    tmf::b19502j2000(alpha_J, delta_J, alpha_B, delta_B);

    return make_tuple(alpha_J, delta_J);
  }

  tuple logpolar2cartesian(const double& rho, const double& theta)
  {
    double x, y;

    tmf::logpolar2cartesian(x, y, rho, theta);

    return make_tuple(x, y);
  }

  tuple cartesian2logpolar(const double& x, const double& y)
  {
    double rho, theta;

    tmf::cartesian2logpolar(rho, theta, x, y);

    return make_tuple(rho, theta);
  }

  tuple polar2cartesian(const double& r, const double& theta)
  {
    double x, y;

    tmf::polar2cartesian(x, y, r, theta);

    return make_tuple(x, y);
  }

  tuple cartesian2polar(const double& x, const double& y)
  {
    double r, theta;

    tmf::cartesian2polar(r, theta, x, y);

    return make_tuple(x, y, r, theta);
  }

  tuple spherical2cartesian(const double& rho, const double& theta, const double& phi)
  {
    double x, y, z;

    tmf::spherical2cartesian(x, y, z, rho, theta, phi);

    return make_tuple(x, y, z);
  }

  tuple cartesian2spherical(const double& x, const double& y, const double& z)
  {
    double rho, theta, phi;

    tmf::cartesian2spherical(rho, theta, phi, x, y, z);

    return make_tuple(rho, theta, phi);
  }

  tuple spherical2cylindrical(const double& rho, const double& phi)
  {
    double r, h;

    tmf::spherical2cylindrical(r, h, rho, phi);

    return make_tuple(r, h);
  }

  tuple cylindrical2spherical(const double& r, const double& h)
  {
    double rho, phi;

    tmf::cylindrical2spherical(rho, phi, r, h);

    return make_tuple(rho, phi);
  }

  tuple equatorial2horizontal(const double& H, const double& delta, const double& phi)
  {
    double A, h;

    tmf::equatorial2horizontal(A, h, H, delta, phi);

    return make_tuple(A, h);
  }

  tuple horizontal2equatorial(const double& A, const double& h, const double& phi)
  {
    double H, delta;

    tmf::horizontal2equatorial(H, delta, A, h, phi);

    return make_tuple(H, delta);
  }

  tuple radec2azel(const double &alpha, const double &delta, const double &utc, const double &ut1_utc, const double &L, const double &phi)
  {
    double A, h;
    
    tmf::radec2azel(A, h, alpha, delta, utc, ut1_utc, L, phi);

    return make_tuple(A, h);
  }

  tuple azel2radec(const double &A, const double &h, const double &utc, const double &ut1_utc, const double &L, const double &phi)
  {
    double alpha, delta;

    tmf::azel2radec(alpha, delta, A, h, utc, ut1_utc, L, phi);

    return make_tuple(alpha, delta);
  }

  tuple jd2date(const double& jd)
  {
    int y, m;
    double d;

    tmf::jd2date(y, m, d, jd);

    return make_tuple(y, m, d);
  }

  tuple nutation(const double& jde)
  {
    double Dphi, Depsilon;

    tmf::nutation(Dphi, Depsilon, jde);

    return make_tuple(Dphi, Depsilon);
  }

  tuple equatorial2galactic(const double& alpha, const double& delta)
  {
    double l, b;

    tmf::equatorial2galactic(l, b, alpha, delta);

    return make_tuple(l, b, alpha, delta);
  }

  tuple galactic2equatorial(const double& l, const double& b)
  {
    double alpha, delta;

    tmf::galactic2equatorial(alpha, delta, l, b);

    return make_tuple(alpha, delta);
  }

  tuple itrf2local(const double& x, const double& y, const double& z,
    const double& ref_x, const double& ref_y, const double& ref_z,
    const double& ref_lon, const double& ref_lat)
  {
    double E, N, U;

    tmf::itrf2local(E, N, U, x, y, z, ref_x, ref_y, ref_z, ref_lon, ref_lat);

    return make_tuple(E, N, U);
  }
}

BOOST_PYTHON_MODULE(pytmf)
{
    def("deg2rad", tmf::deg2rad);
    def("rad2deg", tmf::rad2deg);
    def("jd2mjd", tmf::jd2mjd);
    def("mjd2jd", tmf::mjd2jd);
    def("hms2deg", tmf::hms2deg);
    def("dms2deg", tmf::dms2deg);
    def("deg2hms", pytmf::deg2hms);
    def("deg2dms", pytmf::deg2dms);
    def("hms2rad", tmf::hms2rad);
    def("dms2rad", tmf::dms2rad);
    def("rad2hms", pytmf::rad2hms);
    def("rad2dms", pytmf::rad2dms);
    def("rad2circle", tmf::rad2circle);
    def("deg2circle", tmf::deg2circle);
    def("rad2hmsrepr", tmf::rad2hmsrepr);
    def("rad2dmsrepr", tmf::rad2hmsrepr);
    def("j20002b1950", pytmf::j20002b1950);
    def("b19502j2000", pytmf::b19502j2000);
    def("logpolar2cartesian", pytmf::logpolar2cartesian);
    def("cartesian2logpolar", pytmf::cartesian2logpolar);
    def("spherical2cartesian", pytmf::spherical2cartesian);
    def("cartesian2spherical", pytmf::cartesian2spherical);
    def("equatorial2horizontal", pytmf::equatorial2horizontal);
    def("horizontal2equatorial", pytmf::horizontal2equatorial);
    def("radec2azel", pytmf::radec2azel);
    def("azel2radec", pytmf::azel2radec);
    def("gregoriandate2jd", tmf::gregoriandate2jd);
    def("juliandate2jd", tmf::juliandate2jd);
    def("date2jd", tmf::date2jd);
    def("jd2date", pytmf::jd2date);
    def("nutation", pytmf::nutation);
    def("meanobliquity", tmf::meanobliquity);
    def("gmst", tmf::gmst);
    def("gast", tmf::gast);
    def("last", tmf::last);
    def("equatorial2galactic", pytmf::equatorial2galactic);
    def("galactic2equatorial", pytmf::galactic2equatorial);
    def("tai_utc", tmf::tai_utc);
    def("tt_utc", tmf::tt_utc);
    def("itrf2local", tmf::itrf2local);
}

