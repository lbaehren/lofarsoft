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
  
  bool convertVector (double &xTarget,
		      double &yTarget,
		      double &zTarget,
		      CR::CoordinateType const &targetCoordinate,
		      double const &xSource,
		      double const &ySource,
		      double const &zSource,
		      CR::CoordinateType const &sourceCoordinate,
		      bool const &anglesInDegrees)
  {
    bool status (false);

    switch (sourceCoordinate) {
    case CR::AzElHeight:
      break;
      // ---------------------------------------------------
      // Conversion: AzEl,Height (Az,El,R) - Other
      switch (targetCoordinate) {
      case CR::Cartesian:
	// Conversion: AzEl,Height (Az,El,R) - Cartesian (x,y,z)
	return AzElHeight2Cartesian (xTarget,yTarget,zTarget,
				     xSource,ySource,zSource,
				     anglesInDegrees);
	break;
      }
    case CR::AzElRadius:
      break;
      // ---------------------------------------------------
      // Conversion: AzEl,Radius (Az,El,R) - Other
      switch (targetCoordinate) {
      case CR::Cartesian:
	// Conversion: AzEl,Radius (Az,El,R) - Cartesian (x,y,z)
	return AzElRadius2Cartesian (xTarget,yTarget,zTarget,
				     xSource,ySource,zSource,
				     anglesInDegrees);
	break;
      }
    case CR::Cartesian:
      // ---------------------------------------------------
      // Conversion: Cartesian (x,y,z) -> Other
      switch (targetCoordinate) {
      case CR::Cylindrical:
	// Conversion: Cartesian (x,y,z) -> Cylindrical (rho,phi,z)
	return Cartesian2Cylindrical (xTarget,yTarget,zTarget,
				      xSource,ySource,zSource,
				      anglesInDegrees);
	break;
      case CR::Spherical:
	// Conversion: Cartesian (x,y,z) -> Spherical (r,phi,theta)
	return Cartesian2Spherical (xTarget,yTarget,zTarget,
				    xSource,ySource,zSource,
				    anglesInDegrees);
	break;
      }
      break;
    case CR::Cylindrical:
      // ---------------------------------------------------
      // Conversion: Cylindrical (rho,phi,z) -> Other
      switch (targetCoordinate) {
      case CR::Cartesian:
	// Conversion: Cylindrical (rho,phi,z) -> Cartesian (x,y,z)
	return Cylindrical2Cartesian (xTarget,yTarget,zTarget,
				      xSource,ySource,zSource,
				      anglesInDegrees);
	break;
      case CR::Spherical:
	// Conversion: Cylindrical (rho,phi,z) -> Spherical (rho,phi,z)
	return Cylindrical2Spherical (xTarget,yTarget,zTarget,
				      xSource,ySource,zSource,
				      anglesInDegrees);
	break;
      }
      break;
    case CR::Spherical:
      // ---------------------------------------------------
      // Conversion: Spherical (r,phi,theta) -> Other
      switch (targetCoordinate) {
      case CR::Cartesian:
	// Conversion: Spherical (r,phi,theta) -> Cartesian (x,y,z)
	return Spherical2Cartesian (xTarget,yTarget,zTarget,
				    xSource,ySource,zSource,
				    anglesInDegrees);
	break;
      case CR::Cylindrical:
	// Conversion: Spherical (r,phi,theta) -> Spherical (rho,phi,z)
	return Spherical2Cylindrical (xTarget,yTarget,zTarget,
				      xSource,ySource,zSource,
				      anglesInDegrees);
	break;
      }
      break;
    }

    return status;
  }

  bool convertVector (vector<double> &target,
		      CR::CoordinateType const &targetCoordinate,
		      vector<double> &source,
		      CR::CoordinateType const &sourceCoordinate,
		      bool const &anglesInDegrees)
  {
    return convertVector (target[0],
			  target[1],
			  target[2],
			  targetCoordinate,
			  source[0],
			  source[1],
			  source[2],
			  sourceCoordinate,
			  anglesInDegrees);
  }

#ifdef HAVE_CASA
  bool convertVector (casa::Vector<double> &target,
		      CR::CoordinateType const &targetCoordinate,
		      casa::Vector<double> &source,
		      CR::CoordinateType const &sourceCoordinate,
		      bool const &anglesInDegrees)
  {
    return convertVector (target(0),
			  target(1),
			  target(2),
			  targetCoordinate,
			  source(0),
			  source(1),
			  source(2),
			  sourceCoordinate,
			  anglesInDegrees);
  }
#endif

#ifdef HAVE_BLITZ
  bool convertVector (blitz::Array<double,1> &target,
		      CR::CoordinateType const &targetCoordinate,
		      blitz::Array<double,1> &source,
		      CR::CoordinateType const &sourceCoordinate,
		      bool const &anglesInDegrees)
  {
    return convertVector (target(0),
			  target(1),
			  target(2),
			  targetCoordinate,
			  source(0),
			  source(1),
			  source(2),
			  sourceCoordinate,
			  anglesInDegrees);
  }
#endif

  // ============================================================================
  // 
  //  Conversion: Azimuth,Elevation -> Other
  //
  // ============================================================================
  
  bool AzEl2Cartesian (double &x,
		       double &y,
		       double &z,
		       double const &az,
		       double const &el,
		       double const &r,
		       bool const &anglesInDegrees,
		       bool const &lastIsRadius)
  {
    bool status (true);
    
    try {
      if (anglesInDegrees) {
	double elRadian = deg2rad(el);
	double azRadian = deg2rad(az);
	//
	x = r*cos(elRadian)*cos(azRadian);
	y = r*cos(elRadian)*sin(azRadian);
	z = r*sin(elRadian);
      } else {
	x = r*cos(el)*cos(az);
	y = r*cos(el)*sin(az);
	z = r*sin(el);
      }
    } catch (std::string message) {
      std::cerr << "[azel2Cartesian] " << message << std::endl;
      status = false;
    }

    return status;
  }

  // ------------------------------------------------------- AzElHeight2Cartesian

  bool AzElHeight2Cartesian (double &x,
			     double &y,
			     double &z,
			     double const &az,
			     double const &el,
			     double const &h,
			     bool const &anglesInDegrees)
  {
    bool status (true);
    
    try {
      if (anglesInDegrees) {
	return AzElHeight2Cartesian (x,
				     y,
				     z,
				     deg2rad(az),
				     deg2rad(el),
				     h,
				     false);
      } else {
	double tanEl (tan(el));
	if (tanEl != 0) {
	  x = h*sin(az)/tanEl;
	  y = h*cos(az)/tanEl;
	  z = h;
	}
      }
    } catch (std::string message) {
      std::cerr << "[AzElHeight2Cartesian]" << message << std::endl;
      status = false;
    }
    
    return true;
  }

  // ------------------------------------------------------- AzElHeight2Cartesian

  bool AzElHeight2Cartesian (vector<double> &Cartesian,
			     vector<double> const &AzElHeight,
			     bool const &anglesInDegrees)
  {
    return AzElHeight2Cartesian (Cartesian[0],
				 Cartesian[1],
				 Cartesian[2],
				 AzElHeight[0],
				 AzElHeight[1],
				 AzElHeight[2],
				 anglesInDegrees);
  }

  // ------------------------------------------------------- AzElHeight2Cartesian

#ifdef HAVE_CASA
  bool AzElHeight2Cartesian (casa::Vector<double> &Cartesian,
			     casa::Vector<double> const &AzElHeight,
			     bool const &anglesInDegrees)
  {
    return AzElHeight2Cartesian (Cartesian(0),
				 Cartesian(1),
				 Cartesian(2),
				 AzElHeight(0),
				 AzElHeight(1),
				 AzElHeight(2),
				 anglesInDegrees);
  }
#endif

  // ------------------------------------------------------- AzElHeight2Cartesian

#ifdef HAVE_BLITZ
  bool AzElHeight2Cartesian (blitz::Array<double,1> &Cartesian,
			     blitz::Array<double,1> const &AzElHeight,
			     bool const &anglesInDegrees)
  {
    return AzElHeight2Cartesian (Cartesian(0),
				 Cartesian(1),
				 Cartesian(2),
				 AzElHeight(0),
				 AzElHeight(1),
				 AzElHeight(2),
				 anglesInDegrees);
  }
#endif

  // ============================================================================
  // 
  //  Conversion: Cartesian (x,y,z) -> Other
  //
  // ============================================================================

  // ------------------------------------------------------- Cartesian2AzElHeight

  bool Cartesian2AzElHeight (double &az,
			     double &el,
			     double &h,
			     const double &x,
			     const double &y,
			     const double &z,
			     bool const &anglesInDegrees)
  {
    bool status (true);

    try {
      if (anglesInDegrees) {
	az = rad2deg(atan2(x,y));
	el = rad2deg(atan2(z,sqrt(x*x+y*y)));
	h = z;
      } else {
	az = atan2(x,y);
	el = atan2(z,sqrt(x*x+y*y));
	h = z;
      }
    } catch (std::string message) {
      std::cerr << "[Cartesian2AzElRadius] " << message << std::endl;
      status = false;
    }

    return status;
  }

  // ------------------------------------------------------- Cartesian2AzElHeight

  bool Cartesian2AzElHeight (std::vector<double> &AzElHeight,
			     std::vector<double> const &cartesian,
			     bool const &anglesInDegrees)
  {
    return Cartesian2AzElHeight (AzElHeight[0],
				 AzElHeight[1],
				 AzElHeight[2],
				 cartesian[0],
				 cartesian[1],
				 cartesian[2],
				 anglesInDegrees);
  }
  
  // ------------------------------------------------------- Cartesian2AzElHeight

#ifdef HAVE_CASA
  bool Cartesian2AzElHeight (casa::Vector<double> &AzElHeight,
			     casa::Vector<double> const &cartesian,
			     bool const &anglesInDegrees)
  {
    return Cartesian2AzElHeight (AzElHeight(0),
				 AzElHeight(1),
				 AzElHeight(2),
				 cartesian(0),
				 cartesian(1),
				 cartesian(2),
				 anglesInDegrees);
  }
#endif
  
  // ------------------------------------------------------- Cartesian2AzElHeight

#ifdef HAVE_BLITZ
  bool Cartesian2AzElHeight (blitz::Array<double,1> &AzElHeight,
			     blitz::Array<double,1> const &cartesian,
			     bool const &anglesInDegrees)
  {
    return Cartesian2AzElHeight (AzElHeight(0),
				 AzElHeight(1),
				 AzElHeight(2),
				 cartesian(0),
				 cartesian(1),
				 cartesian(2),
				 anglesInDegrees);
  }
#endif

  // ------------------------------------------------------- Cartesian2AzElRadius

  bool Cartesian2AzElRadius (double &az,
			     double &el,
			     double &radius,
			     double const &x,
			     double const &y,
			     double const &z,
			     bool const &anglesInDegrees)
  {
    bool status (true);

    try {
      if (anglesInDegrees) {
	radius  = sqrt(x*x+y*y+z*z);
	az      = rad2deg(atan2(x,y));
	el      = rad2deg(acos(sqrt(sqrt(x*x+y*y))/radius));
      } else {
	radius  = sqrt(x*x+y*y+z*z);
	az      = atan2(x,y);
	el      = acos(sqrt(sqrt(x*x+y*y))/radius);
      }
    } catch (std::string message) {
      std::cerr << "[Cartesian2AzElRadius] " << message << std::endl;
      status = false;
    }

    return status;
  }
  
  // ------------------------------------------------------- Cartesian2AzElRadius

  bool Cartesian2AzElRadius (std::vector<double> &AzElRadius,
			     std::vector<double> const &cartesian,
			     bool const &anglesInDegrees)
  {
    return Cartesian2AzElRadius (AzElRadius[0],
				 AzElRadius[1],
				 AzElRadius[2],
				 cartesian[0],
				 cartesian[1],
				 cartesian[2],
				 anglesInDegrees);
  }
  
  // ------------------------------------------------------- Cartesian2AzElRadius

#ifdef HAVE_CASA
  bool Cartesian2AzElRadius (casa::Vector<double> &AzElRadius,
			     casa::Vector<double> const &cartesian,
			     bool const &anglesInDegrees)
  {
    return Cartesian2AzElRadius (AzElRadius(0),
				 AzElRadius(1),
				 AzElRadius(2),
				 cartesian(0),
				 cartesian(1),
				 cartesian(2),
				 anglesInDegrees);
  }
#endif
  
  // ------------------------------------------------------- Cartesian2AzElRadius

#ifdef HAVE_BLITZ
  bool Cartesian2AzElRadius (blitz::Array<double,1> &AzElRadius,
			     blitz::Array<double,1> const &cartesian,
			     bool const &anglesInDegrees)
  {
    return Cartesian2AzElRadius (AzElRadius(0),
				 AzElRadius(1),
				 AzElRadius(2),
				 cartesian(0),
				 cartesian(1),
				 cartesian(2),
				 anglesInDegrees);
  }
#endif

  // ------------------------------------------------------ Cartesian2Cylindrical

  bool Cartesian2Cylindrical (double &rho,
			      double &phi,
			      double &z_cyl,
			      const double &x,
			      const double &y,
			      const double &z,
			      bool const &anglesInDegrees)
  {
    bool status (true);

    try {
      if (anglesInDegrees) {
	rho   = sqrt(x*x*+y*y);
	phi   = rad2deg(atan2(y,x));
	z_cyl = z;
      } else {
	rho   = sqrt(x*x*+y*y);
	phi   = atan2(y,x);
	z_cyl = z;
      }
    } catch (std::string message) {
      std::cerr << "[Cartesian2Cylindrical] " << message << std::endl;
      status = false;
    }

    return status;
  }

  // ------------------------------------------------------ Cartesian2Cylindrical

  bool Cartesian2Cylindrical (std::vector<double> &cylindrical,
			      std::vector<double> const &cartesian,
			      bool const &anglesInDegrees)
  {
    return Cartesian2Cylindrical (cylindrical[0],
				  cylindrical[1],
				  cylindrical[2],
				  cartesian[0],
				  cartesian[1],
				  cartesian[2],
				  anglesInDegrees);
  }

#ifdef HAVE_CASA
  bool Cartesian2Cylindrical (casa::Vector<double> &cylindrical,
			      casa::Vector<double> const &cartesian,
			      bool const &anglesInDegrees)
  {
    return Cartesian2Cylindrical (cylindrical(0),
				  cylindrical(1),
				  cylindrical(2),
				  cartesian(0),
				  cartesian(1),
				  cartesian(2),
				  anglesInDegrees);
  }
#endif
  
#ifdef HAVE_BLITZ
  bool Cartesian2Cylindrical (blitz::Array<double,1> &cylindrical,
			      blitz::Array<double,1> const &cartesian,
			      bool const &anglesInDegrees)
  {
    return Cartesian2Cylindrical (cylindrical(0),
				  cylindrical(1),
				  cylindrical(2),
				  cartesian(0),
				  cartesian(1),
				  cartesian(2),
				  anglesInDegrees);
  }
#endif

  // -------------------------------------------------------- Cartesian2Spherical
  
  bool Cartesian2Spherical (double &r,
			    double &phi,
			    double &theta,
			    const double &x,
			    const double &y,
			    const double &z,
			    bool const &anglesInDegrees)
  {
    bool status (true);

    try {
      if (anglesInDegrees) {
	r     = sqrt(x*x + y*y + z*z);
 	phi   = rad2deg(atan2(y,x));
	theta = rad2deg(acos(z/r));
      } else {
	r     = sqrt(x*x + y*y + z*z);
 	phi   = atan2(y,x);
	theta = acos(z/r);
      }
    } catch (std::string message) {
      std::cerr << "[Cartesian2Spherical] " << message << std::endl;
      status = false;
    }

    return status;
  }

  // -------------------------------------------------------- Cartesian2Spherical
  
  bool Cartesian2Spherical (std::vector<double> &spherical,
			    std::vector<double> const &cartesian,
			    bool const &anglesInDegrees)
  {
    return Cartesian2Spherical (spherical[0],
				spherical[1],
				spherical[2],
				cartesian[0],
				cartesian[1],
				cartesian[2],
				anglesInDegrees);
  }
  
#ifdef HAVE_CASA
  bool Cartesian2Spherical (casa::Vector<double> &spherical,
			    casa::Vector<double> const &cartesian,
			    bool const &anglesInDegrees)
  {
    return Cartesian2Spherical (spherical(0),
				spherical(1),
				spherical(2),
				cartesian(0),
				cartesian(1),
				cartesian(2),
				anglesInDegrees);
  }
#endif
  
#ifdef HAVE_BLITZ
  bool Cartesian2Spherical (blitz::Array<double,1> &spherical,
			    blitz::Array<double,1> const &cartesian,
			    bool const &anglesInDegrees)
  {
    return Cartesian2Spherical (spherical(0),
				spherical(1),
				spherical(2),
				cartesian(0),
				cartesian(1),
				cartesian(2),
				anglesInDegrees);
  }
#endif

  // ============================================================================
  // 
  //  Conversion: Cylindrical (rho,phi,z) -> Other
  //
  // ============================================================================

  // ------------------------------------------------------ Cylindrical2Cartesian

  bool Cylindrical2Cartesian (double &x,
			      double &y,
			      double &z,
			      double const &rho,
			      double const &phi,
			      double const &z_cyl,
			      bool const &anglesInDegrees)
  {
    bool status (true);

    try {
      if (anglesInDegrees) {
	x = rho*cos(deg2rad(phi));
	y = rho*sin(deg2rad(phi));
	z = z_cyl;
      } else {
	x = rho*cos(phi);
	y = rho*sin(phi);
	z = z_cyl;
      }
    } catch (std::string message) {
      std::cerr << "[Cylindrical2Cartesian] " << message << std::endl;
      status = false;
    }

    return status;
  }
  
  // ------------------------------------------------------ Cylindrical2Cartesian

  bool Cylindrical2Cartesian (std::vector<double> &cartesian,
			      std::vector<double> const &cylindrical,
			      bool const &anglesInDegrees)
  {
    return Cylindrical2Cartesian (cartesian[0],
				  cartesian[1],
				  cartesian[2],
				  cylindrical[0],
				  cylindrical[1],
				  cylindrical[2],
				  anglesInDegrees);
  }

  // ------------------------------------------------------ Cylindrical2Cartesian

#ifdef HAVE_CASA
  bool Cylindrical2Cartesian (casa::Vector<double> &cartesian,
			      casa::Vector<double> const &cylindrical,
			      bool const &anglesInDegrees)
  {
    return Cylindrical2Cartesian (cartesian(0),
				  cartesian(1),
				  cartesian(2),
				  cylindrical(0),
				  cylindrical(1),
				  cylindrical(2),
				  anglesInDegrees);
  }
#endif
  
  // ------------------------------------------------------ Cylindrical2Cartesian

#ifdef HAVE_BLITZ
  bool Cylindrical2Cartesian (blitz::Array<double,1> &cartesian,
			      blitz::Array<double,1> const &cylindrical,
			      bool const &anglesInDegrees)
  {
    return Cylindrical2Cartesian (cartesian(0),
				  cartesian(1),
				  cartesian(2),
				  cylindrical(0),
				  cylindrical(1),
				  cylindrical(2),
				  anglesInDegrees);
  }
#endif

  // ------------------------------------------------------ Cylindrical2Spherical

  bool Cylindrical2Spherical (double &r,
			      double &phi,
			      double &theta,
			      double const &rho,
			      double const &phi_cyl,
			      double const &z,
			      bool const &anglesInDegrees)
  {
    bool status (true);

    try {
      if (anglesInDegrees) {
	r = sqrt(rho*rho+z*z);
	phi = phi_cyl;
	theta = rad2deg(asin(rho/(r)));
      } else {
	r = sqrt(rho*rho+z*z);
	phi = phi_cyl;
	theta = asin(rho/(r));
      }
    } catch (std::string message) {
      std::cerr << "[Cylindrical2Cartesian] " << message << std::endl;
      status = false;
    }

    return status;
  }

  // ============================================================================
  // 
  //  Conversion: Spherical (r,phi,theta) -> other
  //
  // ============================================================================

  // ----------------------------------------------------- spherical -> cartesian

  bool Spherical2Cartesian (double &x,
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
	Spherical2Cartesian (x,
			     y,
			     z,
			     r,
			     deg2rad(phi),
			     deg2rad(theta),
			     false);
      } else {
	x = r*sin(theta)*cos(phi);
	y = r*sin(theta)*sin(phi);
	z = r*cos(theta); 
      }
    } catch (std::string message) {
      std::cerr << "[Spherical2Cartesian]" << message << std::endl;
      status = false;
    }
    
    return status;
  }

  
#ifdef HAVE_CASA
  bool Spherical2Cartesian (casa::Vector<double> &cartesian,
			    casa::Vector<double> const &spherical,
			    bool const &anglesInDegrees)
  {
    bool status (true);

    return status;
  }
#endif

  // --------------------------------------------------- spherical -> cylindrical

  bool Spherical2Cylindrical (double &rho,
			      double &phi_cyl,
			      double &z,
			      double const &r,
			      double const &phi,
			      double const &theta,
			      bool const &anglesInDegrees)
  {
    bool status (true);
    
    try {
      if (anglesInDegrees) {
	// conversion: deg -> rad
	Spherical2Cylindrical(rho,
			      phi_cyl,
			      z,
			      r,
			      deg2rad(phi),
			      deg2rad(theta),
			      false);
      } else {
	rho     = r*sin(theta);
	phi_cyl = phi;
	z       = r*cos(theta);
      }
    } catch (std::string message) {
      std::cerr << "[Spherical2Cylindrical] " << message << std::endl;
      status = false;
    }

    return status;
  }

  // --------------------------------------------------- spherical -> cylindrical

  bool Spherical2Cylindrical (std::vector<double> &cylindrical,
			      std::vector<double> const &spherical,
			      bool const &anglesInDegrees)
  {
    return Spherical2Cylindrical (cylindrical[0],
				  cylindrical[1],
				  cylindrical[2],
				  spherical[0],
				  spherical[1],
				  spherical[2],
				  anglesInDegrees);
  }
  
  // --------------------------------------------------- spherical -> cylindrical

#ifdef HAVE_CASA
  bool Spherical2Cylindrical (casa::Vector<double> &cylindrical,
			      casa::Vector<double> const &spherical,
			      bool const &anglesInDegrees)
  {
    bool status (true);

    if (anglesInDegrees) {
      Spherical2Cylindrical (cylindrical(0),
			     cylindrical(1),
			     cylindrical(2),
			     spherical(0),
			     deg2rad(spherical(1)),
			     deg2rad(spherical(2)),
			     false);
    } else {
      Spherical2Cylindrical (cylindrical(0),
			     cylindrical(1),
			     cylindrical(2),
			     spherical(0),
			     spherical(1),
			     spherical(2),
			     false);
    }

    return status;
  }
#endif

  // ============================================================================
  //
  //  Various - not yet properly sorted - conversion functions
  //
  // ============================================================================

  // ------------------------------------------------------------------- azze2xyz

  void azze2xyz (double &x,
		 double &y,
		 double &z,
		 double const &r,
		 double const &az,
		 double const &ze,
		 bool const &anglesInDegrees)
  {
    Spherical2Cartesian (x,y,z,r,az,ze,anglesInDegrees);
  }
  
  // ------------------------------------------------------------------- azze2xyz
  
  vector<double> azze2xyz (vector<double> const &azze,
			   bool const &anglesInDegrees)
  {
    return Spherical2Cartesian (azze,anglesInDegrees);
  }
  
  // -------------------------------------------------------- Spherical2Cartesian
  
  vector<double> Spherical2Cartesian (vector<double> const &spherical,
				      bool const &anglesInDegrees)
  {
    uint nelem (spherical.size());
    vector<double> cartesian (3);

    if (nelem == 3) {
      Spherical2Cartesian (cartesian[0],
			   cartesian[1],
			   cartesian[2],
			   spherical[0],
			   spherical[1],
			   spherical[2],
			   anglesInDegrees);      
    } else if (nelem == 2) {
      Spherical2Cartesian (cartesian[0],
			   cartesian[1],
			   cartesian[2],
			   1.0,
			   spherical[1],
			   spherical[2],
			   anglesInDegrees);      
    } else {
      std::cerr << "[Spherical2Cartesian] Unable to convert; too few elements!"
		<< std::endl;
    }

    return cartesian;
  }
  
#ifdef HAVE_CASA
  casa::Vector<double> Spherical2Cartesian (casa::Vector<double> const &spherical,
					    bool const &anglesInDegrees)
  {
    uint nelem (spherical.nelements());
    casa::Vector<double> cartesian (3);

    if (nelem == 3) {
      Spherical2Cartesian (cartesian(0),
			   cartesian(1),
			   cartesian(2),
			   spherical(0),
			   spherical(1),
			   spherical(2),
			   anglesInDegrees);      
    } else if (nelem == 2) {
      std::cerr << "[Spherical2Cartesian] Only two input elements; assuming r=1"
		<< std::endl;
      Spherical2Cartesian (cartesian(0),
			   cartesian(1),
			   cartesian(2),
			   1.0,
			   spherical(1),
			   spherical(2),
			   anglesInDegrees);      
    } else {
      std::cerr << "[Spherical2Cartesian] Unable to convert; too few elements!"
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

  // ------------------------------------------------------------- azel2Cartesian
  
  casa::Vector<double> AzEl2Cartesian (const casa::Vector<double>& azel,
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
    AzEl2Cartesian (cartesian(0),
		    cartesian(1),
		    cartesian(2),
		    az,
		    el,
		    radius,
		    false,
		    true);
    
    return cartesian;
  }

  // ------------------------------------------------------------ polar2Cartesian

  casa::Vector<double> polar2Cartesian (casa::Vector<double> const &polar)
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
  
  // ------------------------------------------------------------- azel2Cartesian

  template <class T>
  blitz::Array<T,1> azel2Cartesian (const blitz::Array<T,1>& azel)
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

  // ------------------------------------------------------------ polar2Cartesian
  
  template <class T>
  blitz::Array<T,1> polar2Cartesian (blitz::Array<T,1> const &polar)
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

  template blitz::Array<float,1> azel2Cartesian (const blitz::Array<float,1>& azel);
  template blitz::Array<double,1> azel2Cartesian (const blitz::Array<double,1>& azel);

  template blitz::Array<float,1> polar2Cartesian (blitz::Array<float,1> const &polar);
  template blitz::Array<double,1> polar2Cartesian (blitz::Array<double,1> const &polar);

#endif
  
} // Namespace CR -- end
