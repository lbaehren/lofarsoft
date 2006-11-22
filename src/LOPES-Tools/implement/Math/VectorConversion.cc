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

/* $Id: VectorConversion.cc,v 1.1 2006/11/08 17:42:42 bahren Exp $*/

#include <Math/VectorConversion.h>
#include <Math/MathGeometry.h>

namespace LOPES { // Namespace LOPES -- begin
  
  template <class T>
  void azel2xyz (T &x, T &y, T &z,
		 T const &r, T const &az, T const &el)
  {
    x = r*cos(el)*cos(az);
    y = r*cos(el)*sin(az);
    z = r*sin(el);
  }

  // ----------------------------------------------------- template instantiation

  template void azel2xyz (float &x, float &y, float &z,
			  float const &r, float const &az, float const &el);
  template void azel2xyz (double &x, double &y, double &z,
			  double const &r, double const &az, double const &el);
  
  // ============================================================================
  //
  //  Conversion using CASA array classes
  //
  // ============================================================================

#ifdef HAVE_CASA

  // ------------------------------------------------------------- azel2cartesian
  
  template <class T>
  Vector<T> azel2cartesian (const Vector<T>& azel)
  {
    Vector<T> cartesian(3);
    unsigned int shape = azel.nelements();
    T radius = 1.0;
    T az = LOPES::deg2rad(azel(0));
    T el = LOPES::deg2rad(azel(1));
    
    /* Check if a source distance is provided. */
    if (shape == 3 && azel(2) > 0) {
      radius = azel(2);
    }
    
    // Coordinate transformation from (az,el) to cartesian (x,y,z)
    azel2xyz (cartesian(0),cartesian(1),cartesian(2),radius,az,el);
    
    return cartesian;
  }

  // ------------------------------------------------------------ polar2cartesian

  template <class T>
  Vector<T> polar2cartesian (Vector<T> const &polar)
  {
    Vector<T> cartesian(3);
    T radius = 1.0;
    T phi    = LOPES::deg2rad(polar(0));
    T theta  = LOPES::deg2rad(polar(1));
    
    if (polar.nelements() == 3) {
      radius = polar(2);
    }
    
    cartesian(0) = radius*sin(theta)*cos(phi);
    cartesian(1) = radius*sin(theta)*sin(phi);
    cartesian(2) = radius*cos(theta);
    
    return cartesian;
  }

  template Vector<float> azel2cartesian (const Vector<float>& azel);
  template Vector<double> azel2cartesian (const Vector<double>& azel);
  
  template Vector<float> polar2cartesian (Vector<float> const &polar);
  template Vector<double> polar2cartesian (Vector<double> const &polar);

#endif

  // ===========================================================================
  //
  //  Conversion using Blitz++ array classes
  //
  // ===========================================================================

#ifdef HAVE_BLITZ_H
  
  // ------------------------------------------------------------- azel2cartesian

  template <class T>
  blitz::Array<T,1> azel2cartesian (const blitz::Array<T,1>& azel)
  {
    blitz::Array<T,1> cartesian(3);
    uInt shape = azel.numElements();
    T radius = 1.0;
    T az = LOPES::deg2rad(azel(0));
    T el = LOPES::deg2rad(azel(1));
    
    /* Check if a source distance is provided. */
    if (shape == 3 && azel(2) > 0) {
      radius = azel(2);
    }
    
    // Coordinate transformation from (az,el) to cartesian (x,y,z)
    azel2xyz (cartesian(0),cartesian(1),cartesian(2),radius,az,el);

    
    return cartesian;
  }

  // ------------------------------------------------------------ polar2cartesian
  
  template <class T>
  blitz::Array<T,1> polar2cartesian (blitz::Array<T,1> const &polar)
  {
    blitz::Array<T,1> cartesian(3);
    T radius = 1.0;
    T phi    = LOPES::deg2rad(polar(0));
    T theta  = LOPES::deg2rad(polar(1));
    
    if (polar.numElements() == 3) {
      radius = polar(2);
    }
    
    cartesian(0) = radius*sin(theta)*cos(phi);
    cartesian(1) = radius*sin(theta)*sin(phi);
    cartesian(2) = radius*cos(theta);
    
    return cartesian;
  }

  template blitz::Array<float,1> azel2cartesian (const blitz::Array<float,1>& azel);
  template blitz::Array<double,1> azel2cartesian (const blitz::Array<double,1>& azel);

  template blitz::Array<float,1> polar2cartesian (blitz::Array<float,1> const &polar);
  template blitz::Array<double,1> polar2cartesian (blitz::Array<double,1> const &polar);

#endif
  
} // Namespace LOPES -- end
