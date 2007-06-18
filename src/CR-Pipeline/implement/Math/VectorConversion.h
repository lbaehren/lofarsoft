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

/* $Id: VectorConversion.h,v 1.4 2007/03/19 13:48:40 bahren Exp $*/

#ifndef VECTORCONVERSION_H
#define VECTORCONVERSION_H

// Standard library header files
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

#ifdef HAVE_BLITZ
#include <blitz/array.h>
#endif

#ifdef HAVE_CASA
#include <casa/aips.h>
#include <casa/Arrays.h>
#include <casa/Exceptions/Error.h>
#include <measures/Measures.h>
using casa::uInt;
#endif

#include <Math/Constants.h>

using std::vector;

namespace CR { // Namespace CR -- begin
  
  /*!
    \file VectorConversion.cc

    \ingroup Math
    
    \brief Functions to handle conversions between vector representations
    
    \author Lars B&auml;hren
    
    \date 2006/11/08
    
    \test tVectorConversion.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
      <li>Some overview of existing conventions for spherical coordinate systems.
    </ul>
    
    <h3>Synopsis</h3>

    In order to provide maximal usability, most of the conversion routines have
    been implemented to work with (a) standard C++ variables and containers of
    the STDL, (b) <a href="http://www.oonumerics.org/blitz">Blitz++</a> arrays
    and (c) <a href="http://casa.nrao.edu">CASA</a> arrays.

    <ul>
      <li>Common source of confusion can be the factor required to convert an angle 
      to its radian measure:
      \f[
      \frac{\alpha [^\circ]}{360 [^\circ]} = \frac{s}{ 2\pi}
      \qquad \longrightarrow \qquad
      \alpha [^\circ] = \frac{180 [^\circ]}{} \cdot s
      \qquad \hbox{and} \qquad
      s = \frac{\pi}{180 [^\circ]} \cdot \alpha [^\circ]
      \f]
      <li>Conversion from spherical to cartesian coordinates:
      \f[
        \left[ \begin{array}{c} x \\ y \\ z \end{array} \right] =
	\left[ \begin{array}{c}
	  r \sin(\phi) \cos(\theta) \\
	  r \sin(\phi) \sin(\theta) \\
	  r \cos(\phi)
	\end{array} \right]
      \f]
      <b>Note:</b> With the azimuth angle defined from South towards East, an
      (Az,Ze) system corresponds to the standard setup for spherical coordinates.
      <li>Conversion from (Az,El) to cartesian coordinates:
      \f[
        \left[ \begin{array}{c} x \\ y \\ z \end{array} \right] =
	\left[ \begin{array}{c}
	  r \cos(El) \cos(Az) \\
	  r \cos(El) \sin(Az) \\
	  r \sin(El)
	\end{array} \right]
      \f]
    </ul>
    
    <h3>Example(s)</h3>

    <ol>
      <li>For the Beamformer to point towards the local zenith:
      \code
      std::vector<double> xyz = azel2xyz (0,90,true);
      \endcode
      The last parameter is used to propagate the information, that the angular
      coordinates are given in degrees, thus requiring conversion before plugging
      the values into the trigonometric functions.
    </ol>
  */  

  // ============================================================================
  //
  //  Structs to defined types of coordinates, etc.
  //
  // ============================================================================
  
  /*!
    \brief List of coordinate systems, in which the positions can be provided
  */
  typedef enum {
    //! Cartesian coordinates, \f$ \vec x = (x,y,z) \f$
    Cartesian,
    //! Spherical coordinates, \f$ \vec x = (r,\phi,\theta) \f$
    Spherical,
    //! Cylindrical coordinates, \f$ \vec x = (r,\phi,h) \f$
    Cylindrical
  } CoordinateTypes;
  

  // ============================================================================
  //
  //  Conversion between angle representations
  //
  // ============================================================================

  //-----------------------------------------------------------------------------
  // [1] degree -> radian

  /*!
    \brief Convert radian to degrees

    \param deg -- Angle in degrees

    \return rad -- Angle in radian
  */
  inline double deg2rad (double const &deg) {
    return deg*CR::pi/180.0;
  }

  /*!
    \brief Convert radian to degrees
    
    \retval rad -- Angle in radian
    \param deg -- Angle in degrees
  */
  inline void deg2rad (double &rad,
		       double const &deg) {
    rad = deg2rad (rad);
  }
  
  /*!
    \brief Convert radian to degrees

    \param deg -- Vector with angles given in degrees

    \return rad -- Vector with angles given in radian
  */
  inline vector<double> deg2rad (vector<double> const &deg) {
    vector<double> rad (deg.size());
    for (unsigned int n(0); n<deg.size(); n++) {
      rad[n] = deg2rad(deg[n]);
    }
    return rad;
  }

#ifdef HAVE_CASA
  /*!
    \brief Convert angular positions from degree to radian

    \param deg -- Vector with angles given in degrees

    \return rad -- Vector with angles given in radian
  */
  inline casa::Vector<double> deg2rad (const casa::Vector<double>& deg) {
    casa::Vector<double> rad = deg*casa::C::pi/180.0;
    return rad;
  }
#endif
  
  //-----------------------------------------------------------------------------
  // [2] radian -> degree

  /*!
    \brief Convert radian to degrees

    \param rad -- Angle in radian

    \return deg -- Angle in degrees
  */
  inline double rad2deg (double const &rad) {
    return rad*180/CR::pi;
  }

  /*!
    \brief Convert radian to degrees
    
    \retval deg -- Angle in degrees
    \param rad -- Angle in radian
  */
  inline void rad2deg (double &deg,
		       double const &rad) {
    deg = rad2deg (rad);
  }
  
  /*!
    \brief Convert radian to degrees

    \param rad -- Vector with angles given in radian

    \return deg -- Vector with angles given in degree
  */
  inline vector<double> rad2deg (vector<double> const &rad) {
    vector<double> deg (rad.size());
    for (unsigned int n(0); n<rad.size(); n++) {
      deg[n] = rad2deg(rad[n]);
    }
    return deg;
  }

#ifdef HAVE_CASA
  /*!
    \brief Convert angular position from radian to degree

    \param rad -- Vector with angles given in radian

    \return deg -- Vector with angles given in degree
  */
  inline casa::Vector<double> rad2deg (const casa::Vector<double>& rad) {
    casa::Vector<double> deg = rad*180.0/casa::C::pi;
    return deg;
  }
#endif
  
  // ============================================================================
  //
  //  Elementary conversion function, only using functionality of the standard
  //  library.
  //
  // ============================================================================

  /*!
    \brief Convert position in Azimuth-Elevation to Cartesian coordinates
    
    \retval x -- x-component of the vector in cartesian coordinates
    \retval y -- y-component of the vector in cartesian coordinates
    \retval z -- z-component of the vector in cartesian coordinates

    \param r               -- Radius
    \param az              -- Azimuth angle
    \param el              -- Elevation angle
    \param anglesInDegrees -- Are the angles in the input vector are provided in
                              degrees? If yes, then an additional conversion step
			      is performed.
  */
  bool azel2xyz (double &x,
		 double &y,
		 double &z,
		 double const &r,
		 double const &az,
		 double const &el,
		 bool const &anglesInDegrees);
  
  /*!
    \brief Convert position in Azimuth-Elevation to Cartesian coordinates

    Internal check is performed on the completeness of the provides (az,el) 
    coordinates; if only two elements can be found we assume to be provided
    (az,el) whereas for three elements we assume (r,az,el).

    \param azel          -- \f$ (r,\phi,\theta) \f$
    \param inputInDegree -- Are the angles in the input vector are provided in
                            degrees? If yes, then an additional conversion step
			    is performed.

    \return xyz -- \f$ (x,y,z) \f$
  */
  vector<double> azel2xyz (vector<double> const &azel,
			   bool const &anglesInDegrees);
  
  /*!
    \brief Convert position in Azimuth-Elevation to Cartesian coordinates

    \retval xyz -- \f$ (x,y,z) \f$

    \param azel          -- \f$ (r,\phi,\theta) \f$
    \param inputInDegree -- Are the angles in the input vector are provided in
                            degrees? If yes, then an additional conversion step
			    is performed.
  */
  inline void azel2xyz (vector<double> &xyz,
			vector<double> const &azel,
			bool const &anglesInDegrees) {
    xyz = azel2xyz (azel,
		    anglesInDegrees);
  }
  
  /*!
    \brief Convert position in Azimuth-Zenith to Cartesian coordinates
    
    \retval x -- x-component of the vector in cartesian coordinates
    \retval y -- y-component of the vector in cartesian coordinates
    \retval z -- z-component of the vector in cartesian coordinates

    \param r             -- Radius
    \param az            -- Azimuth angle
    \param ze            -- Zenith angle
    \param inputInDegree -- Are the angles in the input vector are provided in
                            degrees? If yes, then an additional conversion step
			    is performed.
  */
  void azze2xyz (double &x,
		 double &y,
		 double &z,
		 double const &r,
		 double const &az,
		 double const &ze,
		 bool const &anglesInDegrees);
  
  /*!
    \brief Convert position in Azimuth-Elevation to Cartesian coordinates
  */
  vector<double> azze2xyz (vector<double> const &azze,
			   bool const &anglesInDegrees);

  /*!
    \brief Convert position in Azimuth-Elevation to Cartesian coordinates
  */
  inline void azze2xyz (vector<double> &xyz,
			vector<double> const &azze,
			bool const &anglesInDegrees) {
    xyz = azze2xyz (azze,
		    anglesInDegrees);
  }
  
  // -------------------------------------------------------- cartesian2spherical
  
  /*!
    \brief Conversion from cartesian to spherical coordinates

    \retval r     -- \f$r\f$-component of the vector in spherical coordinates
    \retval phi   -- \f$\phi\f$-component of the vector in spherical coordinates
    \retval theta -- \f$\theta\f$-component of the vector in spherical coordinates
    \param x      -- \f$x\f$-component of the vector in cartesian coordinates
    \param y      -- \f$y\f$-component of the vector in cartesian coordinates
    \param z      -- \f$z\f$-component of the vector in cartesian coordinates
    \param anglesInDegrees -- Are the angles given in units of degrees? If
                    <i>yes</i> angles will be converted to radians before the 
		    conversion.
    
    \return status -- Set to <i>false</i> if an error was encountered.
  */
  bool cartesian2spherical (double &r,
			    double &phi,
			    double &theta,
			    const double &x,
			    const double &y,
			    const double &z,
			    const bool &anglesInDegrees=false);
  
  // -------------------------------------------------------- spherical2cartesian
  
  /*!
    \brief Conversion from cartesian to spherical coordinates

    \retval x    -- \f$x\f$-component of the vector in cartesian coordinates
    \retval y    -- \f$y\f$-component of the vector in cartesian coordinates
    \retval z    -- \f$z\f$-component of the vector in cartesian coordinates
    \param r     -- \f$r\f$-component of the vector in spherical coordinates
    \param phi   -- \f$\phi\f$-component of the vector in spherical coordinates
    \param theta -- \f$\theta\f$-component of the vector in spherical coordinates
    \param anglesInDegrees -- Are the angles given in units of degrees? If
                    <i>yes</i> angles will be converted to radians before the 
		    conversion.
    
    \return status -- Set to <i>false</i> if an error was encountered.
  */
  bool spherical2cartesian (double &x,
			    double &y,
			    double &z,
			    const double &r,
			    const double &phi,
			    const double &theta,
			    const bool &anglesInDegrees=false);
  
  /*!
    \brief Convert position in Spherical to Cartesian coordinates

    Converts 3D vector representation from \f$ (r,\phi,\theta) \f$ to
    \f$ (x,y,z) \f$
    
    \param spherical      -- The 3D position in spherical coordinates,
                             \f$ (r,\phi,\theta) \f$; if only two elements
			     are provided, we consider them to be the two angles
			     and perform the conversion for a position on a
			     unit sphere (i.e. \f$ r=1 \f$).
    \param anglesInDegrees -- 

    \return cartesian -- Representation of the vector in cartesian coordinates,
                         \f$ (x,y,z) \f$
  */
  vector<double> spherical2cartesian (vector<double> const &spherical,
				      bool const &anglesInDegrees);
#ifdef HAVE_CASA
  casa::Vector<double> spherical2cartesian (casa::Vector<double> const &spherical,
					    bool const &anglesInDegrees);
#endif
  
  /*!
    \brief Convert position in Spherical to Cartesian coordinates

    Converts 3D vector representation from \f$ (r,\phi,\theta) \f$ to
    \f$ (x,y,z) \f$

    \retval cartesian -- Representation of the vector in cartesian coordinates,
                         \f$ (x,y,z) \f$
    
    \param spherical      -- 
    \param anglesInDegrees -- 
  */
  inline void spherical2cartesian (vector<double> &cartesian,
				   vector<double> const &spherical,
				   bool const &anglesInDegrees) {
    cartesian = spherical2cartesian (spherical,
				     anglesInDegrees);
  }
  
  // ============================================================================
  //
  //  Conversion using CASA array classes
  //
  // ============================================================================
  
  /*!
    \brief Conversion from (AZ,EL) to (x,y,z)

    Conversion of a source position from azimuth-elevation coordinates, 
    \f$ (\phi, \theta) \f$, to cartesian coordinates, \f$ (x, y, z) \f$:
    \f[
      \left[ \begin{array}{c} x \\ y \\ z \end{array} \right] =
      \left[ \begin{array}{c}
        -\cos(\theta) \cos(\phi) \\
	-\cos(\theta) \sin(\phi) \\
	\sin(\theta)
      \end{array} \right]
    \f]
    If the source is located within the near-field of the array, we have 
    to take into account the distance:
    \f[
      \left[ \begin{array}{c} x \\ y \\ z \end{array} \right] =
      \left[ \begin{array}{c}
        - r\, \cos(\theta) \cos(\phi) \\
	- r\, \cos(\theta) \sin(\phi) \\
	  r\, \sin(\theta)
      \end{array} \right]
    \f]

    \param azel -- (az,el) coordinates

    \return xyz -- Vector in cartesian coordinates
   */
  casa::Vector<double> azel2cartesian (const casa::Vector<double>& azel,
				       bool const &anglesInDegrees=false);  
  
  /*!
    \brief Convert polar spherical coordinates to cartesian coordinates

    Convert polar spherical coordinates, \f$ (\phi,\theta,r) \f$, to cartesian
    coordinates, \f$ (x,y,z) \f$.

    \f[ 
      x = \rho\, \sin(\theta)\, \cos(\phi) \,, \quad
      y = \rho\, \sin(\theta)\, \sin(\phi) \,, \quad
      z = \rho\, \cos(\theta)
    \f]
  */
  casa::Vector<double> polar2cartesian (casa::Vector<double> const &polar);
  
  // ============================================================================
  //
  //  Conversion using Blitz++ array classes
  //
  // ============================================================================
  
#ifdef HAVE_BLITZ
  
  /*!
    \brief Conversion from (AZ,EL) to (x,y,z)

    Conversion of a vector of (az,el) coordinates to cartesian coordinates

    \param azel -- (az,el) coordinates

    \return xyz -- Vector in cartesian coordinates
   */
  template <class T>
    blitz::Array<T,1> azel2cartesian (const blitz::Array<T,1>& azel);  

  template <class T>
    blitz::Array<T,1> polar2cartesian (blitz::Array<T,1> const &polar);

#endif
  
} // Namespace CR -- end

#endif /* VECTORCONVERSION_H */
  
