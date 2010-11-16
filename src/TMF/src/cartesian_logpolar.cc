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
  \brief Converts logpolarho coordinates to Cartesian coordinates

  \param x x-coordinate
  \param y y-coordinate
  \param rho radius
  \param theta angle with x-axis
 */
void tmf::logpolar2cartesian(double& x, double& y,
    const double& rho, const double& theta)
{
  x = exp(rho) * cos(theta);
  y = exp(rho) * sin(theta);
}

/*!
  \brief Converts Cartesian coordinates to logpolarho coordinates

  \param rho radius
  \param theta angle with x-axis
  \param x x-coordinate
  \param y y-coordinate
 */
void tmf::cartesian2logpolar(double& rho, double& theta,
    const double& x, const double& y)
{
  rho = log(sqrt(x*x + y*y));
  theta = atan(x / y);
}

