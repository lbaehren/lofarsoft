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
using namespace tmf;

/*!
  \brief Convert coordinates from ITRF to local Cartesian.

  \param E easterly
  \param N northerly
  \param U upper
  \param x ITRF x
  \param y ITRF y
  \param z ITRF z
  \param ref_x reference x position in ITRF
  \param ref_y reference y position in ITRF
  \param ref_z reference z position in ITRF
  \param ref_lon reference longitude in radians
  \param ref_lat reference latitude in radians
 */
void tmf::itrf2local(double &E, double &N, double& U,
    const double& x, const double& y, const double& z,
    const double& ref_x, const double& ref_y, const double& ref_z,
    const double& ref_lon, const double& ref_lat)
{
  const double dx=x-ref_x;
  const double dy=y-ref_y;
  const double dz=z-ref_z;

  const double sln = sin(ref_lon);
  const double cln = cos(ref_lon);
  const double slt = sin(ref_lat);
  const double clt = cos(ref_lat);

  E = -sln * dx + cln * dy;
  N = -slt * cln * dx - slt * sln * dy + clt * dz;
  U =  clt * cln * dx + clt * sln * dy + slt * dz;
}

