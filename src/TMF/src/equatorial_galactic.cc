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
  \brief Conversion of equatorial coordinates to galactic coordinates.

  Direct implementation of the method described in chapter 13 of
  Astronomical Algorithms by Jean Meeus, second edition, 2005
  Published by: Willman-Bell Inc.
  ISBN 0-943396-61-1

  \param l galactic longitude in radians
  \param b galactic latitutde in radians
  \param alpha right ascension in radians (B1950.0 equinox)
  \param delta declination in radians (B1950.0 equinox)
 */
void tmf::equatorial2galactic(double& l, double& b,
    const double& alpha, const double& delta)
{
  const double sdp = sin(deg2rad(27.4));
  const double cdp = cos(deg2rad(27.4));
  const double sap = sin(deg2rad(192.25) - alpha);
  const double cap = cos(deg2rad(192.25) - alpha);

  l = deg2rad(303.) - atan2(sap, (cap * sdp - tan(delta) * cdp));
  b = asin(sin(delta) * sdp + cos(delta) * cdp * cap);

  // Ensure l is in the range [0,2*pi)
  l = rad2circle(l);
}

/*!
  \brief Conversion of galactic coordinates to equatorial coordinates.

  Direct implementation of the method described in chapter 13 of
  Astronomical Algorithms by Jean Meeus, second edition, 2005
  Published by: Willman-Bell Inc.
  ISBN 0-943396-61-1

  \param alpha right ascension in radians (B1950.0 equinox)
  \param delta declination in radians (B1950.0 equinox)
  \param l galactic longitude in radians
  \param b galactic latitutde in radians
 */
void tmf::galactic2equatorial(double& alpha, double& delta,
    const double& l, const double& b)
{
  const double sdp = sin(deg2rad(27.4));
  const double cdp = cos(deg2rad(27.4));
  const double sap = sin(l - deg2rad(123.));
  const double cap = cos(l - deg2rad(123.));

  alpha = deg2rad(12.25) + atan2(sap, (cap * sdp - tan(b) * cdp));
  delta = asin(sin(b) * sdp + cos(b) * cdp * cap);

  // Ensure alpha is in the range [0,2*pi)
  alpha = rad2circle(alpha);
}

