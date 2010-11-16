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
  \brief Converts spherical coordinates to cylindrical coordinates
  
  Note that theta (the inclination angle from z-axis) is the same in both
  systems.

  \param r cylindrical radius
  \param h cylindrical height
  \param rho radius
  \param phi azimuth angle from x-axis
 */
void tmf::spherical2cylindrical(double r, double h,
    const double& rho, const double& phi)
{
  r = rho * sin(phi);
  h = rho * cos(phi);
}

/*!
  \brief Converts cylindrical coordinates to spherical coordinates

  Note that theta (the inclination angle from z-axis) is the same in both
  systems.

  \param rho radius
  \param phi azimuth angle from x-axis
  \param r cylindrical radius
  \param h cylindrical height
 */
void tmf::cylindrical2spherical(double& rho, double& phi,
    const double& r, const double& h)
{
  rho = sqrt(r*r + h*h);
  phi = atan(r/h);
}

