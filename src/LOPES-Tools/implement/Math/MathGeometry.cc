/***************************************************************************
 *   Copyright (C) 2006                                                    *
 *   Lars B"ahren (bahren@astron.nl)                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <Math/MathGeometry.h>

namespace LOPES { // Namespace LOPES -- begin
  
  // ============================================================================
  //
  //  Units conversion
  //
  // ============================================================================
  
  // -------------------------------------------------------------------- deg2rad

  double deg2rad (double const &deg)
  {
    return deg*casa::C::pi/180.0;
  }
  
  // -------------------------------------------------------------------- deg2rad

  Vector<double> deg2rad (const Vector<double>& deg)
  {
    Vector<double> rad = deg*casa::C::pi/180.0;
    return rad;
  }
  
  // -------------------------------------------------------------------- rad2deg

  double rad2deg (double const &rad)
  {
    return rad*180.0/casa::C::pi;
  }
  
  // -------------------------------------------------------------------- rad2deg

  Vector<double> rad2deg (const Vector<double>& rad)
  {
    Vector<double> deg = rad*180.0/casa::C::pi;
    return deg;
  }
  
  
  // ============================================================================
  //
  //  Coordinate conversions
  //
  // ============================================================================

  // ------------------------------------------------------------- zeaz2cartesian
  
  Vector<double> zeaz2cartesian (double const &ze,
				 double const &az)
  {
    Vector<double> cartesian(2);
    double r = ze/90.0;
    
    cartesian(0) = r*sin(deg2rad(az));
    cartesian(1) = r*cos(deg2rad(az));
    
    return cartesian;
  }
  
  // ------------------------------------------------------------- zeaz2cartesian

  Vector<double> zeaz2cartesian (const Vector<double>& zeaz)
  {
    return zeaz2cartesian (zeaz(0),zeaz(1));
  }
  
} // Namespace LOPES -- end
