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

  bool azel2xyz (double &x,
		 double &y,
		 double &z,
		 double const &r,
		 double const &az,
		 double const &el,
		 bool const &anglesInDegrees)
  {
    bool status (true);

    try {
      if (anglesInDegrees) {
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
    } catch (std::string message) {
      std::cerr << message << std::endl;
      status = false;
    }

    return status;
  }
  
  vector<double> azel2xyz (vector<double> const &azel,
			   bool const &anglesInDegrees)
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
		anglesInDegrees);      
    } else if (nelem == 2) {
      azel2xyz (xyz[0],
		xyz[1],
		xyz[2],
		1.0,
		azel[1],
		azel[2],
		anglesInDegrees);      
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
		 bool const &anglesInDegrees)
  {
    spherical2cartesian (x,y,z,r,az,ze,anglesInDegrees);
  }
  
  // ------------------------------------------------------------------- azze2xyz
  
  vector<double> azze2xyz (vector<double> const &azze,
			   bool const &anglesInDegrees)
  {
    return spherical2cartesian (azze,anglesInDegrees);
  }
  
  // -------------------------------------------------------- cartesian2spherical
  
  bool cartesian2spherical (double &r,
			    double &phi,
			    double &theta,
			    const double &x,
			    const double &y,
			    const double &z,
			    const bool &anglesInDegrees)
  {
    bool status (true);

    return status;
  }
  
  // -------------------------------------------------------- spherical2cartesian
  
  bool spherical2cartesian (double &x,
			    double &y,
			    double &z,
			    double const &r,
			    double const &phi,
			    double const &theta,
			    bool const &anglesInDegrees)
  {
    bool status (true);

    try {
      if (anglesInDegrees) {
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
    } catch (std::string message) {
      std::cerr << message << std::endl;
      status = false;
    }
    
    return status;
  }

  vector<double> spherical2cartesian (vector<double> const &spherical,
				      bool const &anglesInDegrees)
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
			   anglesInDegrees);      
    } else if (nelem == 2) {
      spherical2cartesian (cartesian[0],
			   cartesian[1],
			   cartesian[2],
			   1.0,
			   spherical[1],
			   spherical[2],
			   anglesInDegrees);      
    } else {
      std::cerr << "[spherical2cartesian] Unable to convert; too few elements!"
		<< std::endl;
    }

    return cartesian;
  }
  
#ifdef HAVE_CASA
  casa::Vector<double> spherical2cartesian (casa::Vector<double> const &spherical,
					    bool const &anglesInDegrees)
  {
    uint nelem (spherical.nelements());
    casa::Vector<double> cartesian (3);

    if (nelem == 3) {
      spherical2cartesian (cartesian(0),
			   cartesian(1),
			   cartesian(2),
			   spherical(0),
			   spherical(1),
			   spherical(2),
			   anglesInDegrees);      
    } else if (nelem == 2) {
      std::cerr << "[spherical2cartesian] Only two input elements; assuming r=1"
		<< std::endl;
      spherical2cartesian (cartesian(0),
			   cartesian(1),
			   cartesian(2),
			   1.0,
			   spherical(1),
			   spherical(2),
			   anglesInDegrees);      
    } else {
      std::cerr << "[spherical2cartesian] Unable to convert; too few elements!"
		<< std::endl;
    }

    return cartesian;
  }
#endif
  
  // ============================================================================
  //
  //  Conversion using CASA array classes
  //
  // ============================================================================

#ifdef HAVE_CASA

  // ------------------------------------------------------------- azel2cartesian
  
  casa::Vector<double> azel2cartesian (const casa::Vector<double>& azel,
				       bool const &anglesInDegrees)
  {
    casa::Vector<double> cartesian(3);
    uint shape = azel.nelements();
    double radius = 1.0;
    double az;
    double el;

    if (anglesInDegrees) {
      az = CR::deg2rad(azel(0));
      el = CR::deg2rad(azel(1));
    } else {
      az = azel(0);
      el = azel(1);
    }
    
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

  casa::Vector<double> polar2cartesian (casa::Vector<double> const &polar)
  {
    casa::Vector<double> cartesian(3);
    double radius = 1.0;
    double phi    = CR::deg2rad(polar(0));
    double theta  = CR::deg2rad(polar(1));
    
    if (polar.nelements() == 3) {
      radius = polar(2);
    }
    
    cartesian(0) = radius*sin(theta)*cos(phi);
    cartesian(1) = radius*sin(theta)*sin(phi);
    cartesian(2) = radius*cos(theta);
    
    return cartesian;
  }

#endif

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
    unsigned int shape = azel.numElements();
    T radius = 1.0;
    T az = CR::deg2rad(azel(0));
    T el = CR::deg2rad(azel(1));
    
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
    T phi    = CR::deg2rad(polar(0));
    T theta  = CR::deg2rad(polar(1));
    
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
  
} // Namespace CR -- end
