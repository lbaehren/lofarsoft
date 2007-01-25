/***************************************************************************
 *   Copyright (C) 2007                                                    *
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

/* $Id: template-class.h,v 1.17 2006/11/08 22:07:16 bahren Exp $*/

#include <Imaging/CoordinateConversion.h>

using blitz::Range;

namespace CR { // NAMESPACE CR -- BEGIN
  
  // ============================================================================
  //
  //  Auxilliary vector operations
  //
  // ============================================================================

  // --------------------------------------------------------------------- L1Norm

  double L1Norm (const blitz::Array<double,1> &vec)
  {
    return sum(fabs(vec));
  }

  // --------------------------------------------------------------------- L2Norm

  double L2Norm (const blitz::Array<double,1> &vec)
  {
    return sqrt(sum(pow2(vec)));
  }

  // ============================================================================
  //
  //  Coordinate conversions
  //
  // ============================================================================
  
  // -------------------------------------------------------- cartesian2sphercial

  bool cartesian2spherical (blitz::Array<double,1> &spherical,
			    const blitz::Array<double,1> &cartesian)
  {
    bool status (true);

    return status;
  }

  // -------------------------------------------------------- cartesian2sphercial

  bool cartesian2spherical (blitz::Array<double,2> &spherical,
			    const blitz::Array<double,2> &cartesian)
  {
    bool status (true);

    return status;
  }

  // ------------------------------------------------------ cylindrical2cartesian

  bool cylindrical2cartesian (double &x,
			      double &y,
			      double &z,
			      const double &r,
			      const double &phi,
			      const double &h)
  {
    bool status (true);
    
    try {
      x = r*cos(phi);
      y = r*sin(phi);
      z = h;
    } catch (std::string message) {
      std::cerr << message << std::endl;
      status = false;
    }

    return status;
  }

  // ------------------------------------------------------ cylindrical2cartesian

  bool cylindrical2cartesian (blitz::Array<double,1> &cartesian,
			      const blitz::Array<double,1> &cylindrical)
  {
    bool status (true);
    
    try {
      // check the shape of the arrays first
      uint nofElements (cylindrical.numElements());
      if (nofElements == 3) {
	return cylindrical2cartesian (cartesian(0),
				      cartesian(1),
				      cartesian(2),
				      cylindrical(0),
				      cylindrical(1),
				      cylindrical(2));
      } else {
	std::cerr << "Wrong number of vector elements - must be 3!" << std::endl;
	status = false;
      }
    } catch (std::string message) {
      std::cerr << message << std::endl;
      status = false;
    }
    
    return status;
  }
  
  // -------------------------------------------------------- spherical2cartesian

  bool spherical2cartesian (double &x,
			    double &y,
			    double &z,
			    const double &r,
			    const double &phi,
			    const double &theta,
			    const bool &anglesInDegrees)
  {
    bool status (true);

    // check if we have to convert from degrees to radian
    
    try {
      if (anglesInDegrees) {
	x = r*cos(deg2rad(phi))*sin(deg2rad(theta));
	y = r*sin(deg2rad(phi))*sin(deg2rad(theta));
	z = r*cos(deg2rad(theta));
      } else {
	x = r*cos(phi)*sin(theta);
	y = r*sin(phi)*sin(theta);
	z = r*cos(theta);
      }
    } catch (std::string message) {
      std::cerr << message << std::endl;
      status = false;
    }

    return status;
  }
  
  // -------------------------------------------------------- spherical2cartesian
  
  bool spherical2cartesian (blitz::Array<double,1> &cartesian,
			    const blitz::Array<double,1> &spherical,
			    const bool &anglesInDegrees)
  {
    bool status (true);
    
    
    try {
      // check the shape of the arrays first
      uint nofElements (spherical.numElements());
      if (nofElements == 3) {
	return spherical2cartesian (cartesian(0),
				    cartesian(1),
				    cartesian(2),
				    spherical(0),
				    spherical(1),
				    spherical(2),
				    anglesInDegrees);
      } else {
	std::cerr << "Wrong number of vector elements - must be 3!" << std::endl;
	status = false;
      }
    } catch (std::string message) {
      std::cerr << message << std::endl;
      status = false;
    }
    
    return status;
  }
  
  // ------------------------------------------------------ spherical2cartesian
  
  bool spherical2cartesian (blitz::Array<double,2> &cartesian,
			    const blitz::Array<double,2> &spherical)
  {
    bool status (true);
    
    try {
      // (r,phi,theta) -> (x,y,z)
      cartesian(Range(Range::all()),0) = spherical(Range(Range::all()),0)*cos(spherical(Range(Range::all()),1))*sin(spherical(Range(Range::all()),2));
      cartesian(Range(Range::all()),1) = spherical(Range(Range::all()),0)*sin(spherical(Range(Range::all()),1))*sin(spherical(Range(Range::all()),2));
      cartesian(Range(Range::all()),2) = spherical(Range(Range::all()),0)*cos(spherical(Range(Range::all()),2));
    } catch (std::string message) {
      std::cerr << message << std::endl;
      status = false;
    }
    
    return status;
  }
  
}  // NAMESPACE CR -- END

