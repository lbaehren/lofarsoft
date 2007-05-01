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

/* $Id: VectorConversion.cc,v 1.4 2007/03/19 13:48:40 bahren Exp $*/

#include <Math/VectorConversion.h>

namespace CR { // Namespace CR -- begin
  
  // ============================================================================
  //
  //  Elementary conversion function, only using functionality of the standard
  //  library.
  //
  // ============================================================================

  // ------------------------------------------------------------------- azel2xyz

  void azel2xyz (double &x,
		 double &y,
		 double &z,
		 double const &r,
		 double const &az,
		 double const &el,
		 bool const &inputInDegrees)
  {
    if (inputInDegrees) {
      spherical2cartesian (x,
			   y,
			   z,
			   r,
			   deg2rad(az),
			   deg2rad(el),
			   false);
    } else {
      x = r*cos(el)*cos(az);
      y = r*cos(el)*sin(az);
      z = r*sin(el);
    }
  }
  
  vector<double> azel2xyz (vector<double> const &azel,
			   bool const &inputInDegrees)
  {
    uint nelem (azel.size());
    vector<double> xyz (3);

    if (nelem == 3) {
      azel2xyz (xyz[0],
		xyz[1],
		xyz[2],
		azel[0],
		azel[1],
		azel[2],
		inputInDegrees);      
    } else if (nelem == 2) {
      azel2xyz (xyz[0],
		xyz[1],
		xyz[2],
		1.0,
		azel[1],
		azel[2],
		inputInDegrees);      
    } else {
      std::cerr << "[azel2xyz] Unable to convert; too few elements!"
		<< std::endl;
    }

    return xyz;
  }
  
  // ------------------------------------------------------------------- azze2xyz

  void azze2xyz (double &x,
		 double &y,
		 double &z,
		 double const &r,
		 double const &az,
		 double const &ze,
		 bool const &inputInDegrees)
  {
    spherical2cartesian (x,y,z,r,az,ze,inputInDegrees);
  }
  
  // ------------------------------------------------------------------- azze2xyz
  
  vector<double> azze2xyz (vector<double> const &azze,
			   bool const &inputInDegrees)
  {
    return spherical2cartesian (azze,inputInDegrees);
  }
  
  // -------------------------------------------------------- spherical2cartesian

  void spherical2cartesian (double &x,
			    double &y,
			    double &z,
			    double const &r,
			    double const &phi,
			    double const &theta,
			    bool const &inputInDegrees)
  {
    if (inputInDegrees) {
      spherical2cartesian (x,
			   y,
			   z,
			   r,
			   deg2rad(phi),
			   deg2rad(theta),
			   false);
    } else {
      x = r*sin(phi)*cos(theta);
      y = r*sin(phi)*sin(theta);
      z = r*cos(phi); 
    }
  }

  vector<double> spherical2cartesian (vector<double> const &spherical,
				      bool const &inputInDegrees)
  {
    uint nelem (spherical.size());
    vector<double> cartesian (3);

    if (nelem == 3) {
      spherical2cartesian (cartesian[0],
			   cartesian[1],
			   cartesian[2],
			   spherical[0],
			   spherical[1],
			   spherical[2],
			   inputInDegrees);      
    } else if (nelem == 2) {
      spherical2cartesian (cartesian[0],
			   cartesian[1],
			   cartesian[2],
			   1.0,
			   spherical[1],
			   spherical[2],
			   inputInDegrees);      
    } else {
      std::cerr << "[spherical2cartesian] Unable to convert; too few elements!"
		<< std::endl;
    }

    return cartesian;
  }

  // ============================================================================
  //
  //  Conversion using CASA array classes
  //
  // ============================================================================

  // ------------------------------------------------------------- azel2cartesian
  
  Vector<double> azel2cartesian (const Vector<double>& azel)
  {
    Vector<double> cartesian(3);
    uint shape = azel.nelements();
    double radius = 1.0;
    double az = LOPES::deg2rad(azel(0));   //  both angles are provided in deg
    double el = LOPES::deg2rad(azel(1));   //
    
    /* Check if a source distance is provided. */
    if (shape == 3 && azel(2) > 0) {
      radius = azel(2);
    }
    
    // Coordinate transformation from (az,el) to cartesian (x,y,z)
    azel2xyz (cartesian(0),
	      cartesian(1),
	      cartesian(2),
	      radius,
	      az,
	      el,
	      false);
    
    return cartesian;
  }

  // ------------------------------------------------------------ polar2cartesian

  Vector<double> polar2cartesian (Vector<double> const &polar)
  {
    Vector<double> cartesian(3);
    double radius = 1.0;
    double phi    = LOPES::deg2rad(polar(0));
    double theta  = LOPES::deg2rad(polar(1));
    
    if (polar.nelements() == 3) {
      radius = polar(2);
    }
    
    cartesian(0) = radius*sin(theta)*cos(phi);
    cartesian(1) = radius*sin(theta)*sin(phi);
    cartesian(2) = radius*cos(theta);
    
    return cartesian;
  }

  // ===========================================================================
  //
  //  Conversion using Blitz++ array classes
  //
  // ===========================================================================

#ifdef HAVE_BLITZ
  
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
    cartesian = radius*cos(el)*cos(az), radius*cos(el)*sin(az), radius*sin(el);
    
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

  template blitz::Array<Float,1> azel2cartesian (const blitz::Array<Float,1>& azel);
  template blitz::Array<Double,1> azel2cartesian (const blitz::Array<Double,1>& azel);

  template blitz::Array<Float,1> polar2cartesian (blitz::Array<Float,1> const &polar);
  template blitz::Array<Double,1> polar2cartesian (blitz::Array<Double,1> const &polar);

#endif
  
} // Namespace CR -- end
