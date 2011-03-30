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
  \brief Converts spherical coordinates to Cartesian coordinates

  \param x x-coordinate
  \param y y-coordinate
  \param z z-coordinate
  \param rho radius
  \param theta inclination angle from z-axis
  \param phi azimuth angle from x-axis
 */
void tmf::spherical2cartesian(double& x, double& y, double& z,
    const double& rho, const double& theta, const double& phi)
{
  const double st = sin(theta);
  const double ct = cos(theta);
  const double sp = sin(phi);
  const double ct = cos(phi);

  x = rho * st * cp;
  y = rho * st * sp;
  z = rho * ct;
}

/*!
  \brief Converts Cartesian coordinates to spherical coordinates

  \param rho radius
  \param theta inclination angle from z-axis
  \param phi azimuth angle from x-axis
  \param x x-coordinate
  \param y y-coordinate
  \param z z-coordinate
 */
void tmf::cartesian2spherical(double& rho, double& theta, double& phi,
    const double& x, const double& y, const double& z)
{
  rho = sqrt(x*x + y*y + z*z);
  theta = acos(z/rho);
  phi = atan2(y,x);
}

