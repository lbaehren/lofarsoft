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
  \brief Conversion of equatorial coordinates to ecliptical coordinates.

  Direct implementation of the method described in chapter 13 of
  Astronomical Algorithms by Jean Meeus, second edition, 2005
  Published by: Willman-Bell Inc.
  ISBN 0-943396-61-1

  \param lambda ecliptical (or celestial) longitude, measured from the
         vernal equinox along the ecliptic in radians
  \param beta ecliptical (or celestial) latitude, positive if north of the
         ecliptic, negative if south in radians
  \param alpha right ascension in radians (B1950.0 equinox)
  \param delta declination in radians (B1950.0 equinox)
  \param jde Julian Date of TD (or equivalently TT)
 */
void tmf::equatorial2ecliptic(double& lambda, double& beta,
    const double& alpha, const double& delta,
    const double& jde)
{
  double Dphi = 0.0;
  double Depsilon = 0.0;

  // Get mean obliquity of the ecliptic
  double epsilon = meanobliquity(jde);

  // Get nutation and deviaton for obliquity of the ecliptic
  nutation(Dphi, Depsilon, jde);

  // Correct for effect of nutation
  epsilon += Depsilon;

  const double se = sin(epsilon);
  const double ce = cos(epsilon);
  const double sa = sin(alpha);
  const double ca = cos(alpha);
  const double sd = sin(delta);
  const double cd = cos(delta);
  const double td = tan(delta);

  lambda = atan2(sa * cd + td * se, ca);
  beta = asin(sd * ca - cd * se * sa);
}

/*!
  \brief Conversion of ecliptical coordinates to equatorial coordinates.

  Direct implementation of the method described in chapter 13 of
  Astronomical Algorithms by Jean Meeus, second edition, 2005
  Published by: Willman-Bell Inc.
  ISBN 0-943396-61-1

  \param alpha right ascension in radians (B1950.0 equinox)
  \param delta declination in radians (B1950.0 equinox)
  \param lambda ecliptical (or celestial) longitude, measured from the
         vernal equinox along the ecliptic in radians
  \param beta ecliptical (or celestial) latitude, positive if north of the
         ecliptic, negative if south in radians
  \param jde Julian Date of TD (or equivalently TT)
 */
void tmf::ecliptic2equatorial(double& alpha, double& delta,
    const double& lambda, const double& beta,
    const double& jde)
{
  double Dphi = 0.0;
  double Depsilon = 0.0;

  // Get mean obliquity of the ecliptic
  double epsilon = meanobliquity(jde);

  // Get nutation and deviaton for obliquity of the ecliptic
  nutation(Dphi, Depsilon, jde);

  // Correct for effect of nutation
  epsilon += Depsilon;

  const double se = sin(epsilon);
  const double ce = cos(epsilon);
  const double sl = sin(lambda);
  const double cl = cos(lambda);
  const double sb = sin(beta);
  const double cb = cos(beta);
  const double tb = tan(beta);

  alpha = atan2(sl * ce - tb * se, cl);
  delta = asin(sb * ce + cb * se * sl);
}

