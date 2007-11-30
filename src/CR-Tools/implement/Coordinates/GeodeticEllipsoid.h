/*-------------------------------------------------------------------------*
| $Id: template-class.h,v 1.20 2007/06/13 09:41:37 bahren Exp           $ |
*-------------------------------------------------------------------------*
***************************************************************************
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

#ifndef _GEODETICELLIPSOID_H_
#define _GEODETICELLIPSOID_H_

#include <stdio.h>
using namespace std;

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

using casa::Complex;
using casa::String;
using casa::Vector;

namespace CR { // Namespace CR -- begin
  
  /*!
    \class GeodeticEllipsoid
    
    \ingroup CR_Coordinates
    
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
    
    // ---  Construction ---------------------------------------------------------
    
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
    inline String name () {
      return name_p;
    }
    
    /*!
      \brief Set the identifier of the geocentric system.
    */
    void setName (const String& name);
    
    /*!
      \brief Get the length of the major axis

      \return majorAxis -- The length of the major axis
    */
    inline double majorAxis () {
      return majorAxis_p;
    }
    
    void setMajorAxis (const double& a);
    
    /*!
      \brief Get the length of the minor axis

      \return minorAxis -- The length of the minor axis
    */
    inline double minorAxis () {
      return minorAxis_p;
    }
    
    void setMinorAxis (const double& b);
    
    /*!
      \brief Get the translation of the reference frame

      \return translation
    */
    inline Vector<double> translation () {
      return translation_p;
    }
    
    void setTranslation (const Vector<double>& translation);
    
    inline Vector<double> rotation () {
      return rotation_p;
    }
    
    void setRotation (const Vector<double>& rotation);
    
    inline double scale () {
      return scale_p;
    }
    
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

// =============================================================================

} // Namespace CR -- end

#endif
