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

// External libraries
#ifdef HAVE_BLITZ
#include <blitz/array.h>
#endif

// AIPS++/CASA header files
#include <casa/aips.h>
#include <casa/Arrays.h>
#include <casa/Exceptions/Error.h>
#include <measures/Measures.h>

using std::vector;
using casa::Vector;
using casa::uInt;

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
      <li>[start filling in your text here]
    </ul>
    
    <h3>Synopsis</h3>

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
      vector<double> xyz = azel2xyz (0,90,true);
      \endcode
      The last parameter is used to propagate the information, that the angular
      coordinates are given in degrees, thus requiring conversion before plugging
      the values into the trigonometric functions.
    </ol>
  */  

  // ============================================================================
  //
  // Conversion between angle representations
  //
  // ============================================================================

  /*!
    \brief Convert radian to degrees

    \param deg -- Angle in degrees

    \return rad -- Angle in radian
  */
  inline double deg2rad (double const &deg) {
    return deg*casa::C::pi/180.0;
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

    \param rad -- Angle in radian

    \return deg -- Angle in degrees
  */
  inline double rad2deg (double const &rad) {
    return rad*180/casa::C::pi;
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

    \param r             -- Radius
    \param az            -- Azimuth angle
    \param el            -- Elevation angle
    \param inputInDegree -- Are the angles in the input vector are provided in
                            degrees? If yes, then an additional conversion step
			    is performed.
  */
  void azel2xyz (double &x,
		 double &y,
		 double &z,
		 double const &r,
		 double const &az,
		 double const &el,
		 bool const &inputInDegrees);
  
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
			   bool const &inputInDegrees);
  
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
			bool const &inputInDegrees) {
    xyz = azel2xyz (azel,
		    inputInDegrees);
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
		 bool const &inputInDegrees);
  
  /*!
    \brief Convert position in Azimuth-Elevation to Cartesian coordinates
  */
  vector<double> azze2xyz (vector<double> const &azze,
			   bool const &inputInDegrees);

  /*!
    \brief Convert position in Azimuth-Elevation to Cartesian coordinates
  */
  inline void azze2xyz (vector<double> &xyz,
			vector<double> const &azze,
			bool const &inputInDegrees) {
    xyz = azze2xyz (azze,
		    inputInDegrees);
  }
  
  // -------------------------------------------------------- spherical2cartesian
  
  void spherical2cartesian (double &x,
			    double &y,
			    double &z,
			    double const &r,
			    double const &phi,
			    double const &theta,
			    bool const &inputInDegrees);
  
  /*!
    \brief Convert position in Spherical to Cartesian coordinates

    Converts 3D vector representation from \f$ (r,\phi,\theta) \f$ to
    \f$ (x,y,z) \f$
    
    \param spherical      -- 
    \param inputInDegrees -- 

    \return cartesian -- Representation of the vector in cartesian coordinates,
                         \f$ (x,y,z) \f$
  */
  vector<double> spherical2cartesian (vector<double> const &spherical,
				      bool const &inputInDegrees);
  
  /*!
    \brief Convert position in Spherical to Cartesian coordinates

    Converts 3D vector representation from \f$ (r,\phi,\theta) \f$ to
    \f$ (x,y,z) \f$

    \retval cartesian -- Representation of the vector in cartesian coordinates,
                         \f$ (x,y,z) \f$
    
    \param spherical      -- 
    \param inputInDegrees -- 
  */
  inline void spherical2cartesian (vector<double> &cartesian,
				   vector<double> const &spherical,
				   bool const &inputInDegrees) {
    cartesian = spherical2cartesian (spherical,
				     inputInDegrees);
  }
  
  // ============================================================================
  //
  //  Conversion using CASA array classes
  //
  // ============================================================================
  
  //! Convert angular positions from degree to radian
  inline Vector<double> deg2rad (const Vector<double>& deg) {
    Vector<double> rad = deg*casa::C::pi/180.0;
    return rad;
  }
  
  //! Convert angular position from radian to degree
  inline Vector<double> rad2deg (const Vector<double>& rad) {
    Vector<double> deg = rad*180.0/casa::C::pi;
    return deg;
  }
  
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
  Vector<double> azel2cartesian (const Vector<double>& azel);  
  
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
  Vector<double> polar2cartesian (Vector<double> const &polar);
  
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
  
