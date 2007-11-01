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
    case CR::Cartesian:
      // ---------------------------------------------------
      // Conversion: Cartesian (x,y,z) -> Other
      switch (targetCoordinate) {
      case CR::Cylindrical:
	// Conversion: Cartesian (x,y,z) -> Cylindrical (rho,phi,z)
	return cartesian2cylindrical (xTarget,
				      yTarget,
				      zTarget,
				      xSource,
				      ySource,
				      zSource,
				      anglesInDegrees);
	break;
      case CR::Spherical:
	// Conversion: Cartesian (x,y,z) -> Spherical (r,phi,theta)
	return cartesian2spherical (xTarget,
				    yTarget,
				    zTarget,
				    xSource,
				    ySource,
				    zSource,
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
	return cylindrical2cartesian (xTarget,
				      yTarget,
				      zTarget,
				      xSource,
				      ySource,
				      zSource,
				      anglesInDegrees);
	break;
      case CR::Spherical:
	// Conversion: Cylindrical (rho,phi,z) -> Spherical (rho,phi,z)
	return cylindrical2spherical (xTarget,
				      yTarget,
				      zTarget,
				      xSource,
				      ySource,
				      zSource,
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
	return spherical2cartesian (xTarget,
				    yTarget,
				    zTarget,
				    xSource,
				    ySource,
				    zSource,
				    anglesInDegrees);
	break;
      case CR::Cylindrical:
	// Conversion: Spherical (r,phi,theta) -> Spherical (rho,phi,z)
	return spherical2cylindrical (xTarget,
				      yTarget,
				      zTarget,
				      xSource,
				      ySource,
				      zSource,
				      anglesInDegrees);
	break;
      }
      break;
    }

    return status;
  }

  // ============================================================================
  // 
  //  Conversion: Azimuth,Elevation -> Other
  //
  // ============================================================================

  // ------------------------------------------------------------- azel2cartesian

  bool azel2cartesian (double &x,
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
      std::cerr << "[azel2cartesian] " << message << std::endl;
      status = false;
    }
    
    return status;
  }
  
  // ------------------------------------------------------------- azel2cartesian

  bool azel2cartesian (std::vector<double> &cartesian,
		       std::vector<double> const &azel,
		       bool const &anglesInDegrees,
		       bool const &lastIsRadius)
  {
    if (azel.size() == 3) {
      cartesian.resize(3);
      return azel2cartesian (cartesian[0],
			     cartesian[1],
			     cartesian[2],
			     azel[0],
			     azel[1],
			     azel[2],
			     anglesInDegrees,
			     lastIsRadius);
    } else {
      return false;
    }
  }

  // ============================================================================
  // 
  //  Conversion: Cartesian (x,y,z) -> Other
  //
  // ============================================================================

  // ------------------------------------------------------ cartesian2cylindrical

  bool cartesian2cylindrical (double &rho,
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
      std::cerr << "[cartesian2cylindrical] " << message << std::endl;
      status = false;
    }

    return status;
  }

  // ------------------------------------------------------ cartesian2cylindrical

  bool cartesian2cylindrical (std::vector<double> &cylindrical,
			      std::vector<double> const &cartesian,
			      bool const &anglesInDegrees)
  {
    return cartesian2cylindrical (cylindrical[0],
				  cylindrical[1],
				  cylindrical[2],
				  cartesian[0],
				  cartesian[1],
				  cartesian[2],
				  anglesInDegrees);
  }

#ifdef HAVE_CASA
  bool cartesian2cylindrical (casa::Vector<double> &cylindrical,
			      casa::Vector<double> const &cartesian,
			      bool const &anglesInDegrees)
  {
    return cartesian2cylindrical (cylindrical(0),
				  cylindrical(1),
				  cylindrical(2),
				  cartesian(0),
				  cartesian(1),
				  cartesian(2),
				  anglesInDegrees);
  }
#endif
  
#ifdef HAVE_BLITZ
  bool cartesian2cylindrical (blitz::Array<double,1> &cylindrical,
			      blitz::Array<double,1> const &cartesian,
			      bool const &anglesInDegrees)
  {
    return cartesian2cylindrical (cylindrical(0),
				  cylindrical(1),
				  cylindrical(2),
				  cartesian(0),
				  cartesian(1),
				  cartesian(2),
				  anglesInDegrees);
  }
#endif

  // -------------------------------------------------------- cartesian2spherical
  
  bool cartesian2spherical (double &r,
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
      std::cerr << "[cartesian2spherical] " << message << std::endl;
      status = false;
    }

    return status;
  }

  // -------------------------------------------------------- cartesian2spherical
  
  bool cartesian2spherical (std::vector<double> &spherical,
			    std::vector<double> const &cartesian,
			    bool const &anglesInDegrees)
  {
    return cartesian2spherical (spherical[0],
				spherical[1],
				spherical[2],
				cartesian[0],
				cartesian[1],
				cartesian[2],
				anglesInDegrees);
  }
  
#ifdef HAVE_CASA
  bool cartesian2spherical (casa::Vector<double> &spherical,
			    casa::Vector<double> const &cartesian,
			    bool const &anglesInDegrees)
  {
    return cartesian2spherical (spherical(0),
				spherical(1),
				spherical(2),
				cartesian(0),
				cartesian(1),
				cartesian(2),
				anglesInDegrees);
  }
#endif
  
#ifdef HAVE_BLITZ
  bool cartesian2spherical (blitz::Array<double,1> &spherical,
			    blitz::Array<double,1> const &cartesian,
			    bool const &anglesInDegrees)
  {
    return cartesian2spherical (spherical(0),
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

  // ------------------------------------------------------ cylindrical2cartesian

  bool cylindrical2cartesian (double &x,
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
      std::cerr << "[cylindrical2cartesian] " << message << std::endl;
      status = false;
    }

    return status;
  }
  
  // ------------------------------------------------------ cylindrical2cartesian

  bool cylindrical2cartesian (std::vector<double> &cartesian,
			      std::vector<double> const &cylindrical,
			      bool const &anglesInDegrees)
  {
    return cylindrical2cartesian (cartesian[0],
				  cartesian[1],
				  cartesian[2],
				  cylindrical[0],
				  cylindrical[1],
				  cylindrical[2],
				  anglesInDegrees);
  }

  // ------------------------------------------------------ cylindrical2cartesian

#ifdef HAVE_CASA
  bool cylindrical2cartesian (casa::Vector<double> &cartesian,
			      casa::Vector<double> const &cylindrical,
			      bool const &anglesInDegrees)
  {
    return cylindrical2cartesian (cartesian(0),
				  cartesian(1),
				  cartesian(2),
				  cylindrical(0),
				  cylindrical(1),
				  cylindrical(2),
				  anglesInDegrees);
  }
#endif
  
  // ------------------------------------------------------ cylindrical2cartesian

#ifdef HAVE_BLITZ
  bool cylindrical2cartesian (blitz::Array<double,1> &cartesian,
			      blitz::Array<double,1> const &cylindrical,
			      bool const &anglesInDegrees)
  {
    return cylindrical2cartesian (cartesian(0),
				  cartesian(1),
				  cartesian(2),
				  cylindrical(0),
				  cylindrical(1),
				  cylindrical(2),
				  anglesInDegrees);
  }
#endif

  // ------------------------------------------------------ cylindrical2spherical

  bool cylindrical2spherical (double &r,
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
      std::cerr << "[cylindrical2cartesian] " << message << std::endl;
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
	x = r*sin(theta)*cos(phi);
	y = r*sin(theta)*sin(phi);
	z = r*cos(theta); 
      }
    } catch (std::string message) {
      std::cerr << "[spherical2cartesian]" << message << std::endl;
      status = false;
    }
    
    return status;
  }

  
#ifdef HAVE_CASA
  bool spherical2cartesian (casa::Vector<double> &cartesian,
			    casa::Vector<double> const &spherical,
			    bool const &anglesInDegrees)
  {
    bool status (true);

    return status;
  }
#endif

  // --------------------------------------------------- spherical -> cylindrical

  bool spherical2cylindrical (double &rho,
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
	spherical2cylindrical(rho,
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
      std::cerr << "[spherical2cylindrical] " << message << std::endl;
      status = false;
    }

    return status;
  }

  // --------------------------------------------------- spherical -> cylindrical

  bool spherical2cylindrical (std::vector<double> &cylindrical,
			      std::vector<double> const &spherical,
			      bool const &anglesInDegrees)
  {
    return spherical2cylindrical (cylindrical[0],
				  cylindrical[1],
				  cylindrical[2],
				  spherical[0],
				  spherical[1],
				  spherical[2],
				  anglesInDegrees);
  }
  
  // --------------------------------------------------- spherical -> cylindrical

#ifdef HAVE_CASA
  bool spherical2cylindrical (casa::Vector<double> &cylindrical,
			      casa::Vector<double> const &spherical,
			      bool const &anglesInDegrees)
  {
    bool status (true);

    if (anglesInDegrees) {
      spherical2cylindrical (cylindrical(0),
			     cylindrical(1),
			     cylindrical(2),
			     spherical(0),
			     deg2rad(spherical(1)),
			     deg2rad(spherical(2)),
			     false);
    } else {
      spherical2cylindrical (cylindrical(0),
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
    spherical2cartesian (x,y,z,r,az,ze,anglesInDegrees);
  }
  
  // ------------------------------------------------------------------- azze2xyz
  
  vector<double> azze2xyz (vector<double> const &azze,
			   bool const &anglesInDegrees)
  {
    return spherical2cartesian (azze,anglesInDegrees);
  }
  
  // -------------------------------------------------------- spherical2cartesian
  
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
    azel2cartesian (cartesian(0),
		    cartesian(1),
		    cartesian(2),
		    az,
		    el,
		    radius,
		    false,
		    true);
    
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
