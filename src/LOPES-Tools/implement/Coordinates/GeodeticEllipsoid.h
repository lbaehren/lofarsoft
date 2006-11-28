
/* $Id: GeodeticEllipsoid.h,v 1.2 2006/07/05 16:01:08 bahren Exp $ */

#ifndef _GEODETICELLIPSOID_H_
#define _GEODETICELLIPSOID_H_

#include <iostream>

// include the AIPS++ wrapper classes
#include <casa/aips.h>
#include <casa/iostream.h>
#include <casa/string.h>
#include <casa/Arrays.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/Arrays/IPosition.h>
#include <casa/Arrays/Array.h>
#include <casa/Arrays/Vector.h>
#include <casa/Arrays/Matrix.h>
#include <scimath/Mathematics.h>

using std::cerr;
using std::complex;
using std::cout;
using std::endl;

using casa::String;
using casa::Vector;

namespace LOPES {  // Namespace LOPES -- begin
  
  /*!
    \class GeodeticEllipsoid
    
    \ingroup Coordinates
    
    \brief Geodetic coordinate conversions.
    
    \author Lars B&auml;hren
    
    \date 2005/06/29
  */
  
  class GeodeticEllipsoid {
    
    bool flag_p;
    
    //! Identifier of the geocentric system.
    String name_p;
    
    // Translation parameters
    Vector<double> translation_p;
    
    // Rotation parameters.
    Vector<double> rotation_p;
    
    double scale_p;                  // Scaling factor
    
    double majorAxis_p;
    
    double minorAxis_p;
    
    //! Flattening of the ellipsoid
    double flattening_p;
    
  public:
    
    // ---  Construction --------------------------------------------------------
    
    /*!
      \brief Constructor
    */
    GeodeticEllipsoid ();
    
    /*!
      \brief Constructor
    */
    GeodeticEllipsoid (const String& name,
		       double major,
		       double minor,
		       Vector<double> translation,
		       Vector<double> rotation,
		       double scale);
    
  // --- Deconstruction --------------------------------------------------------

  /*!
    \brief Deconstructor
   */
  ~GeodeticEllipsoid ();

  // --- Internal parameters ---------------------------------------------------

  /*!
    \brief Get the identifier of the geocentric system.
  */
  String name ();

  /*!
    \brief Set the identifier of the geocentric system.
  */
  void setName (const String& name);

  double majorAxis ();

  void setMajorAxis (const double& a);

  double minorAxis ();

  void setMinorAxis (const double& b);

  Vector<double> translation ();

  void setTranslation (const Vector<double>& translation);

  Vector<double> rotation ();

  void setRotation (const Vector<double>& rotation);

  double scale ();

  void setScale (const double& scale);

  void setFlattening (const double& flattening);

  /*!
    Computes and stores the flattening, \f$ f \f$, of an ellipsoid with major
    semi-axis \f$ a \f$ and minor semi-axis \f$ b \f$.
  
    \param a -- Major semi-axis of the ellipsoid. [m]
    \param b -- Minor semi-axis of the ellipsoid. [m]
  */
  void setFlattening (const double& a,
		      const double& b);

  // Get variable values
  void print_variables();
  double get_flattening();
  
  // --- Computation routines --------------------------------------------------

  double eccentricity ();

  double eccentricity (const double& a,
		       const double& b);
  
  double primeVerticalRadius (const double& a,
			      const double& b,
			      const double& lat);
  
  void geographic_to_geocentric(const double& a,
				const double& b,
				double lat,
				double lon,
				double h,
				double& x,
				double& y,
				double& z);
  
  void geographic_to_geocentric(const double& a,
				const double& b,
				Vector<double> geogra,
				Vector<double>& geocen);
  
  /*!
    \brief Helmert transformation between geodetic and geocentric coordinates

    \param from    -- 
    \param inverse -- Perform inverse transform? Default is set to false.

    \return to -- 
  */
  Vector<double> helmertTransform (const Vector<double>& from,
				   const bool inverse=false);

 private:

  // --- Initialization --------------------------------------------------------
  
  void init(String name,
	    double major,
	    double minor,
	    Vector<double> translation,
	    Vector<double> rotation,
	    double scale);

  // --- Parameter manipulation ------------------------------------------------

  void setMinorAxis ();
  void setFlattening ();
  
};

}  // Namespace LOPES -- end

#endif
