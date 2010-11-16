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
#include <cmath>

// PROJECT INCLUDES
#include <tmf.h>

// LOCAL INCLUDES
//

// FORWARD REFERENCES
//

using namespace std;

/*!
  \brief Convert equatorial coordinates from epoch J2000 to B1950

  Rotation matrix derived from equation 14.55 and the problem 14.6 in
  Spherical Astronomy by Robin M. Green
  Cambridge University Press 1985
  ISBN 0-521-31779-7

  \param alpha_B right ascension reffered to the B1950 standard epoch
  \param delta_B declination reffered to the B1950 standard epoch
  \param alpha_J right ascension reffered to the J2000 standard epoch
  \param delta_J declination reffered to the J2000 standard epoch
 */
void tmf::j20002b1950(double& alpha_B, double& delta_B,
    const double& alpha_J, const double& delta_J)
{
  // Rotation matrix J2000 -> B1950
  const double Rjb[3][3] = {
    { 9.99925421e-01, 1.11805983e-02, 4.85870406e-03},
    {-1.11813727e-02, 9.99937199e-01,-2.69609221e-05},
    {-4.85874763e-03,-2.72312417e-05, 9.99987696e-01}
  };

  // Calculate sines and cosines for increased speed and clarity
  const double sa = sin(alpha_J);
  const double ca = cos(alpha_J);
  const double sd = sin(delta_J);
  const double cd = cos(delta_J);

  /* Calculate new vector:
     (cos(alpha_B)*cos(delta_B), sin(alpha_B)*cos(delta_B), sin(delta_B) */
  const double s[3]= {
    Rjb[0][0] * ca * cd + \
    Rjb[0][1] * sa * cd + \
    Rjb[0][2] * sd,

    Rjb[1][0] * ca * cd + \
    Rjb[1][1] * sa * cd + \
    Rjb[1][2] * sd,

    Rjb[2][0] * ca * cd + \
    Rjb[2][1] * sa * cd + \
    Rjb[2][2] * sd
  };

  // Extract right ascension and declination
  alpha_B = rad2circle(atan2(s[1], s[0]));
  delta_B = asin(s[2]);
}

/*!
  \brief Convert equatorial coordinates from epoch B1950 to J2000

  Rotation matrix derived from equation 14.55 and the problem 14.6 in
  Spherical Astronomy by Robin M. Green
  Cambridge University Press 1985
  ISBN 0-521-31779-7

  \param alpha_J right ascension reffered to the J2000 standard epoch
  \param delta_J declination reffered to the J2000 standard epoch
  \param alpha_B right ascension reffered to the B1950 standard epoch
  \param delta_B declination reffered to the B1950 standard epoch
 */
void tmf::b19502j2000(double& alpha_J, double& delta_J,
    const double& alpha_B, const double& delta_B)
{
  // Rotation matrix B1950 -> J2000
  const double Rbj[3][3] = {
    { 9.99925951e-01,-1.11806049e-02,-4.85870550e-03},
    { 1.11813779e-02, 9.99937782e-01,-2.73680983e-05},
    { 4.85875211e-03,-2.70945233e-05, 9.99988696e-01}
  };

  // Calculate sines and cosines for increased speed and clarity
  const double sa = sin(alpha_B);
  const double ca = cos(alpha_B);
  const double sd = sin(delta_B);
  const double cd = cos(delta_B);

  /* Calculate new vector:
     (cos(alpha_J)*cos(delta_J), sin(alpha_J)*cos(delta_J), sin(delta_J) */
  const double s[3]= {
    Rbj[0][0] * ca * cd + \
    Rbj[0][1] * sa * cd + \
    Rbj[0][2] * sd,

    Rbj[1][0] * ca * cd + \
    Rbj[1][1] * sa * cd + \
    Rbj[1][2] * sd,

    Rbj[2][0] * ca * cd + \
    Rbj[2][1] * sa * cd + \
    Rbj[2][2] * sd
  };

  // Extract right ascension and declination
  alpha_J = rad2circle(atan2(s[1], s[0]));
  delta_J = asin(s[2]);
}

