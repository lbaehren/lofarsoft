/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
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
    \file VectorConversion.h
    \ingroup Math
    
    \brief Functions to handle conversions between vector representations
    
    \author Lars B&auml;hren
    
    \date 2006/11/08
    
    \test tVectorConversion.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
      <li>Basic knowledge of analytical geometry
      <li>Some overview of existing conventions for spherical coordinate systems.
    </ul>
    
    <h3>Synopsis</h3>

    The <b>spherical coordinate</b> system represents points as a tuple of three
    components. For use in physical sciences and technology, the recommended
    international standard notation is \f$ r,\vartheta,\phi \f$ for distance,
    zenith and azimuth (ISO 31-11). (In the standard \f$\vartheta\f$ is preferred
    to \f$\theta\f$, although it is the same Greek letter). Otherwise, in America
    the components are typically notated as \f$(\rho,\phi,\theta)\f$ for
    distance, zenith and azimuth, while elsewhere the notation is reversed for
    zenith and azimuth as \f$(\rho,\theta,\phi)\f$. The former has the advantage
    of being most compatible with the notation for the two-dimensional polar
    coordinate system and the three-dimensional cylindrical coordinate system,
    while the latter has the broader acceptance geographically. The notation
    convention of the author of any work pertaining to spherical coordinates
    should always be checked before using the formulas and equations of that
    author. [from Wikipedia, english version]

    We here use the three coordinates \f$(r,\phi,\theta)\f$ according to the
    following convention:
    <ul>
      <li>\f$r\f$ (radius) is the distance of a point P from the origin O of the
      coordinate system, i.e. the length of the vector \f$\vec r\f$;
      <li>\f$\phi\f$ (azimuth angle) is the angle between the positive x-axis and
      \f$\vec r_{xy}\f$ (i.e. the projection of the vector \f$\vec r\f$ onto the
      x-y-plane), with values running from 0 to \f$2\pi\f$ (\f$0^\circ\f$ to
      \f$360^\circ\f$) counted counter-clockwise
      <li>\f$\theta\f$ (polar angle) is the angle between the positive z-axis
      and the vector \f$\vec r\f$, with values running from 0 to \f$\pi\f$
      (\f$0^\circ\f$ to \f$180^\circ\f$)
    </ul>

    For <b>cylindrical coordinates</b> we closely follow the above convention,
    such that the azmiuthal angle \f$\phi\f$ in the x-y-plane is the same as for
    the case of spherical coordinates (and thereby reflection the relation of
    the cylindrical coordinates to polar coordinates in the plain).

    <h3>Conversion relations/functions</h3>

    <center>
      <table>
        <tr>
	  <td class="indexkey">Target</td>
	  <td class="indexkey">... as function of</td>
	  <td class="indexkey">Function</td>
	  <td class="indexkey">Relation</td>
	  <td class="indexkey">Inversion</td>
	</tr>
	<tr>
          <td>Cartesian <br> \f$ (x,y,z) \f$</td>
	  <td>Spherical <br> \f$ (r,\phi,\theta) \f$</td>
	  <td>CR::Spherical2Cartesian</td>
	  <td>\f$ \left[ \begin{array}{c} x \\ y \\ z \end{array} \right] =
	  \left[ \begin{array}{l}
	    r \sin(\theta) \cos(\phi) \\
	    r \sin(\theta) \sin(\phi) \\
	    r \cos(\theta) \end{array} \right] \f$</td>
	  <td>\f$ \left[ \begin{array}{l} r \\ \phi \\ \theta \end{array} \right]
	  = \left[ \begin{array}{l}
	  \sqrt{x^2 + y^2 + z^2} \\
	  \mathrm{atan} (y/x) \\ 
	  \mathrm{acos} \left( \frac{z}{\sqrt{x^2 + y^2 + z^2}} \right)
	  \end{array} \right] \f$</td>
	</tr>
	<tr>
          <td>Cartesian <br> \f$ (x,y,z) \f$</td>
	  <td>cylindrical <br> \f$ (\rho,\phi,z) \f$</td>
	  <td>CR::Cylindrical2Cartesian</td>
	  <td>\f$ \left[ \begin{array}{c} x \\ y \\ z \end{array} \right] =
	    \left[ \begin{array}{l}
	  \rho \cos(\phi) \\
	  \rho \sin(\phi) \\
	  z \end{array} \right] \f$</td>
	  <td>\f$ \left[ \begin{array}{l} \rho \\ \phi \\ z \end{array} \right]
	    = \left[ \begin{array}{l}
	    \sqrt{x^2 + y^2} \\
	    \mathrm{atan} (y/x) \\
	    z \end{array} \right] \f$</td>
	</tr>
	<tr>
	  <td>Cartesian <br> \f$ (x,y,z) \f$</td>
          <td>AzElHeight <br> \f$ (Az,El,H) \f$</td>
	  <td>CR::AzElHeight2Cartesian</td>
	  <td></td>
	  <td></td>
	</tr>
	<tr>
	  <td>Cartesian <br> \f$ (x,y,z) \f$</td>
          <td>AzElRadius <br> \f$ (Az,El,R) \f$</td>
	  <td>CR::AzElRadius2Cartesian</td>
	  <td>\f$ \left[ \begin{array}{l} x \\ y \\ z \end{array} \right]
	    = \left[ \begin{array}{l}
	    R \cos(El) \sin(Az) \\
	    R \cos(El) \cos(Az) \\
	    R \sin(El)
	    \end{array} \right] \f$</td>
	  <td>\f$ \left[ \begin{array}{l} Az \\ El \\ R \end{array} \right]
	    = \left[ \begin{array}{l}
	    \mathrm{atan} \left( x/y \right) \\
	    \mathrm{acos} \left( \sqrt{\frac{x^2+y^2}{x^2+y^2+z^2}} \right) \\
	    \sqrt{x^2+y^2+z^2}
	    \end{array} \right] \f$</td>
	</tr>
	<tr>
	  <td>Cartesian <br> \f$ (x,y,z) \f$</td>
          <td>NorthEastHeight <br> \f$ (N,E,H) \f$</td>
	  <td>CR::NorthEastHeight2Cartesian</td>
	  <td>\f$ \left[ \begin{array}{c} x \\ y \\ z \end{array} \right] =
	  \left[ \begin{array}{l} E \\ N \\ H \end{array} \right] \f$</td>
	  <td>\f$ \left[ \begin{array}{c} N \\ E \\ H \end{array} \right] =
	  \left[ \begin{array}{l} y \\ x \\ z \end{array} \right] \f$</td>
	</tr>
        <tr>
	  <td class="indexkey">Target</td>
	  <td class="indexkey">... as function of</td>
	  <td class="indexkey">Function</td>
	  <td class="indexkey">Relation</td>
	  <td class="indexkey">Inversion</td>
	</tr>
	<tr>
	  <td>Spherical <br> \f$ (r,\phi,\theta) \f$</td>
          <td>Cartesian <br> \f$ (x,y,z) \f$</td>
	  <td>CR::Cartesian2Spherical</td>
	  <td>\f$ \left[ \begin{array}{l} r \\ \phi \\ \theta \end{array} \right]
	  = \left[ \begin{array}{l}
	  \sqrt{x^2 + y^2 + z^2} \\
	  \mathrm{atan} (y/x) \\ 
	  \mathrm{acos} \left( \frac{z}{\sqrt{x^2 + y^2 + z^2}} \right)
	  \end{array} \right] \f$</td>
	  <td>\f$ \left[ \begin{array}{c} x \\ y \\ z \end{array} \right] =
	  \left[ \begin{array}{l}
	    r \sin(\theta) \cos(\phi) \\
	    r \sin(\theta) \sin(\phi) \\
	    r \cos(\theta) \end{array} \right] \f$</td>
	</tr>
	<tr>
	  <td>Spherical <br> \f$ (r,\phi,\theta) \f$</td>
          <td>cylindrical <br> \f$ (\rho,\phi,z) \f$</td>
	  <td>CR::Cylindrical2Spherical</td>
	  <td>\f$ \left[ \begin{array}{l} r \\ \phi \\ \theta \end{array} \right]
	    = \left[ \begin{array}{l} 
	    \sqrt{\rho^2 + z^2} \\
	    \phi \\
	    \mathrm{asin} \left( \frac{\rho}{\sqrt{\rho^2 + z^2}} \right)
	    \end{array} \right] \f$</td>
	  <td>\f$ \left[ \begin{array}{l} \rho \\ \phi \\ z \end{array} \right]
	    = \left[ \begin{array}{l} 
	    r \sin(\theta) \\ \phi \\ r \cos(\theta)
	    \end{array} \right] \f$</td>
	</tr>
        <tr>
	  <td class="indexkey">Target</td>
	  <td class="indexkey">... as function of</td>
	  <td class="indexkey">Function</td>
	  <td class="indexkey">Relation</td>
	  <td class="indexkey">Inversion</td>
	</tr>
	<tr>
	  <td>cylindrical <br> \f$ (\rho,\phi,z) \f$</td>
          <td>Cartesian <br> \f$ (x,y,z) \f$</td>
	  <td>CR::Cartesian2Cylindrical</td>
	  <td>\f$ \left[ \begin{array}{l} \rho \\ \phi \\ z \end{array} \right]
	    = \left[ \begin{array}{l}
	    \sqrt{x^2 + y^2} \\
	    \mathrm{atan} (y/x) \\
	    z \end{array} \right] \f$</td>
	  <td>\f$ \left[ \begin{array}{c} x \\ y \\ z \end{array} \right] =
	    \left[ \begin{array}{l}
	  \rho \cos(\phi) \\
	  \rho \sin(\phi) \\
	  z \end{array} \right] \f$</td>
	</tr>
	<tr>
          <td>cylindrical <br> \f$ (\rho,\phi,z) \f$</td>
	  <td>Spherical <br> \f$ (r,\phi,\theta) \f$</td>
	  <td>CR::Spherical2Cylindrical</td>
	  <td>\f$ \left[ \begin{array}{l} \rho \\ \phi \\ z \end{array} \right]
	    = \left[ \begin{array}{l} 
	    r \sin(\theta) \\ \phi \\ r \cos(\theta)
	    \end{array} \right] \f$</td>
	  <td>\f$ \left[ \begin{array}{l} r \\ \phi \\ \theta \end{array} \right]
	    = \left[ \begin{array}{l} 
	    \sqrt{\rho^2 + z^2} \\
	    \phi \\
	    \mathrm{asin} \left( \frac{\rho}{\sqrt{\rho^2 + z^2}} \right)
	    \end{array} \right] \f$</td>
	</tr>
        <tr>
	  <td class="indexkey">Target</td>
	  <td class="indexkey">... as function of</td>
	  <td class="indexkey">Function</td>
	  <td class="indexkey">Relation</td>
	  <td class="indexkey">Inversion</td>
	</tr>
	<tr>
	  <td>AzElRadius <br> \f$ (Az,El,R) \f$</td>
          <td>AzElHeight <br> \f$ (Az,El,H) \f$</td>
	  <td>CR::AzElHeight2AzElRadius</td>
	  <td></td>
	  <td></td>
	</tr>
        <tr>
	  <td class="indexkey">Target</td>
	  <td class="indexkey">... as function of</td>
	  <td class="indexkey">Function</td>
	  <td class="indexkey">Relation</td>
	  <td class="indexkey">Inversion</td>
	</tr>
	<tr>
	  <td>AzElHeight <br> \f$ (Az,El,H) \f$</td>
          <td>AzElRadius <br> \f$ (Az,El,R) \f$</td>
	  <td>CR::AzElRadius2AzElHeight</td>
	  <td></td>
	  <td></td>
	</tr>
        <tr>
	  <td class="indexkey">Target</td>
	  <td class="indexkey">... as function of</td>
	  <td class="indexkey">Function</td>
	  <td class="indexkey">Relation</td>
	  <td class="indexkey">Inversion</td>
	</tr>
	<tr>
	  <td>NorthEastHeight <br> \f$ (N,E,H) \f$</td>
          <td>Cartesian <br> \f$ (x,y,z) \f$</td>
	  <td>CR::Cartesian2NorthEastHeight</td>
	  <td>\f$ \left[ \begin{array}{c} N \\ E \\ H \end{array} \right] =
	  \left[ \begin{array}{l} y \\ x \\ z \end{array} \right] \f$</td>
	  <td>\f$ \left[ \begin{array}{c} x \\ y \\ z \end{array} \right] =
	  \left[ \begin{array}{l} E \\ N \\ H \end{array} \right] \f$</td>
	</tr>
	<tr>
	  <td>NorthEastHeight <br> \f$ (N,E,H) \f$</td>
          <td>AzElRadius <br> \f$ (Az,El,R) \f$</td>
	  <td>CR::AzElRadius2NorthEastHeight</td>
	  <td>\f$ \left[ \begin{array}{l} N \\ E \\ H \end{array} \right]
	    = \left[ \begin{array}{l}
	    R \cos(El) \cos(Az) \\
	    R \cos(El) \sin(Az) \\
	    R \sin(El)
	    \end{array} \right] \f$</td>
	  <td>\f$ \left[ \begin{array}{l} Az \\ El \\ R \end{array} \right]
	    = \left[ \begin{array}{l}
	    \mathrm{atan} \left( E/N \right) \\
	    \mathrm{acos} \left( \sqrt{\frac{N^2+E^2}{N^2+E^2+H^2}} \right) \\
	    \sqrt{N^2+E^2+H^2}
	    \end{array} \right] \f$</td>
	</tr>
      </table>
    </center>

    <ul>
      <li>Common source of confusion can be the factor required to convert an
      angle to its radian measure:
      \f[
      \frac{\alpha [^\circ]}{360 [^\circ]} = \frac{s}{ 2\pi}
      \qquad \longrightarrow \qquad
      \alpha [^\circ] = \frac{180 [^\circ]}{} \cdot s
      \qquad \hbox{and} \qquad
      s = \frac{\pi}{180 [^\circ]} \cdot \alpha [^\circ]
      \f]
      <li>With the azimuth angle defined from South towards East, an
      (Az,Ze) system corresponds to the standard setup for spherical
      coordinates.
    </ul>

    <h3>Implementation</h3>

    In order to provide maximal usability, most of the conversion routines
    have been implemented to work with:
    <ol>
      <li>Standard C++ variables
      \code
      bool Cartesian2Spherical (double &r,
                                double &phi,
				double &theta,
				const double &x,
				const double &y,
				const double &z,
				bool const &anglesInDegrees=false);
      \endcode
      <li>Vectors of the C++ STDL
      \code
      bool Cartesian2Spherical (std::vector<double> &spherical,
                                std::vector<double> const &cartesian,
				bool const &anglesInDegrees=false);
      \endcode
      <li>Blitz++ arrays 
      \code
      #ifdef HAVE_BLITZ
      bool Cartesian2Spherical (blitz::Array<double,1> &spherical,
                                blitz::Array<double,1> const &cartesian,
				bool const &anglesInDegrees=false);
      #endif
      \endcode
      <li>CASA arrays
      \code
      #ifdef HAVE_CASA
      bool Cartesian2Spherical (casa::Vector<double> &spherical,
                                casa::Vector<double> const &cartesian,
				bool const &anglesInDegrees=false);
      #endif
      \endcode
    </ol>
    
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
    Cylindrical,
    //! Azimuth-Elevation-Height
    AzElHeight,
    //! Azimuth-Elevation-Radius
    AzElRadius,
    //! North-East-Height
    NorthEastHeight
  } CoordinateType;
  

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
  inline double deg2rad (double const &deg)
 {
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
  //  Conversion: generic interface
  //
  // ============================================================================

  /*!
    \brief Convert vector from one coordinate system to another

    \retval xTarget -- First component of the vector in the target coordinate
                       system
    \retval yTarget -- Second component of the vector in the target coordinate
                       system
    \retval zTarget -- Third component of the vector in the target coordinate
                       system
    \param targetCoordinate -- CR::CoordinateType of the target coordinate system
    \param xSource -- First component of the vector in the source coordinate
                      system
    \param ySource -- Second component of the vector in the source coordinate
                      system
    \param zSource -- Third component of the vector in the source coordinate
                      system
    \param sourceCoordinate -- CR::CoordinateType of the source coordinate system
    \param anglesInDegrees  -- Are the angles given in units of degrees? If
                               <i>true</i> angles will be converted to radians
			       before the conversion.
  */
  bool convertVector (double &xTarget,
		      double &yTarget,
		      double &zTarget,
		      CR::CoordinateType const &targetCoordinate,
		      double const &xSource,
		      double const &ySource,
		      double const &zSource,
		      CR::CoordinateType const &sourceCoordinate,
		      bool const &anglesInDegrees=false);

  // ============================================================================
  // 
  //  Conversion: Azimuth,Elevation -> Other
  //
  // ============================================================================

  bool AzElRadius2Cartesian (double &x,
			     double &y,
			     double &z,
			     double const &az,
			     double const &el,
			     double const &r,
			     bool const &anglesInDegrees=false);

  /*!
    \brief Conversion from Azimuth,Elevetion to cartesian coordinates

    \retval x  -- \f$x\f$-component of the vector in cartesian coordinates
    \retval y  -- \f$y\f$-component of the vector in cartesian coordinates
    \retval z  -- \f$z\f$-component of the vector in cartesian coordinates
    \param az  -- 
    \param el  -- 
    \param r   -- 
    \param anglesInDegrees -- Are the angles given in units of degrees? If
           <i>true</i> angles will be converted to radians before the 
	   conversion.
    \param lastIsRadius    -- As there are two different flavors of
           Azimuth,Elevation coordinates, this switch can be used for further
	   specification. If <tt>lastIsRadius=true</tt> the origin vector is
	   considered as (Az,El,Radius), thereby redirecting the actual
	   evaluation to AzElRadius2Cartesian.
  */
  bool azel2Cartesian (double &x,
		       double &y,
		       double &z,
		       double const &az,
		       double const &el,
		       double const &r,
		       bool const &anglesInDegrees=false,
		       bool const &lastIsRadius=true);
  
  bool azel2Cartesian (std::vector<double> &cartesian,
		       std::vector<double> const &azel,
		       bool const &anglesInDegrees=false,
		       bool const &lastIsRadius=true);

#ifdef HAVE_CASA  
  bool azel2Cartesian (casa::Vector<double> &cartesian,
		       casa::Vector<double> const &azel,
		       bool const &anglesInDegrees=false,
		       bool const &lastIsRadius=true);
#endif
  
  // ============================================================================
  // 
  //  Conversion: Cartesian (x,y,z) -> Other
  //
  // ============================================================================

  // ----------------------------------------------------------------------------
  // Cartesian (x,y,z) -> Cylindrical (rho,phi,z)

  /*!
    \brief Conversion from cartesian to cylindrical coordinates

    \retval rho   -- \f$\rho\f$-component of the vector in cylindrical coordinates
    \retval phi   -- \f$\phi\f$-component of the vector in cylindrical coordinates
    \retval z     -- \f$z\f$-component of the vector in cylindrical coordinates
    \param x      -- \f$x\f$-component of the vector in cartesian coordinates
    \param y      -- \f$y\f$-component of the vector in cartesian coordinates
    \param z      -- \f$z\f$-component of the vector in cartesian coordinates
    \param anglesInDegrees -- Are the angles given in units of degrees? If
                    <i>yes</i> angles will be converted to radians before the 
		    conversion.
    
    \return status -- Set to <i>false</i> if an error was encountered.
  */
  bool Cartesian2Cylindrical (double &rho,
			      double &phi,
			      double &z_cyl,
			      const double &x=0.0,
			      const double &y=0.0,
			      const double &z=0.0,
			      bool const &anglesInDegrees=false);

  /*!
    \brief Conversion from cartesian to cylindrical coordinates

    \retval cylindrical -- Vector in cylindrical coordinates,
                           \f$ (\rho,\phi,z) \f$
    \param cartesian    -- Vector in cartesian coordinates, \f$ (x,y,z) \f$
    \param anglesInDegrees -- Are the angles given in units of degrees? If
                    <i>yes</i> angles will be converted to radians before the 
		    conversion.
    
    \return status -- Set to <i>false</i> if an error was encountered.
  */
  bool Cartesian2Cylindrical (std::vector<double> &cylindrical,
			      std::vector<double> const &cartesian,
			      bool const &anglesInDegrees=false);

#ifdef HAVE_CASA
  bool Cartesian2Cylindrical (casa::Vector<double> &cylindrical,
			      casa::Vector<double> const &cartesian,
			      bool const &anglesInDegrees=false);
#endif
  
#ifdef HAVE_BLITZ
  bool Cartesian2Cylindrical (blitz::Array<double,1> &cylindrical,
			      blitz::Array<double,1> const &cartesian,
			      bool const &anglesInDegrees=false);
#endif
  
  // ----------------------------------------------------------------------------
  // Cartesian (x,y,z) -> Spherical (r,phi,theta)

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
  bool Cartesian2Spherical (double &r,
			    double &phi,
			    double &theta,
			    const double &x,
			    const double &y,
			    const double &z,
			    bool const &anglesInDegrees=false);

  /*!
    \brief Conversion from cartesian to spherical coordinates

    \retval spherical -- Vector in spherical coordinates, \f$ (r,\phi,\theta) \f$
    \param cartesian  -- Vector in cartesian coordinates, \f$ (x,y,z) \f$
    \param anglesInDegrees -- Are the angles given in units of degrees? If
                    <i>yes</i> angles will be converted to radians before the 
		    conversion.
    
    \return status -- Set to <i>false</i> if an error was encountered.
  */
  bool Cartesian2Spherical (std::vector<double> &spherical,
			    std::vector<double> const &cartesian,
			    bool const &anglesInDegrees=false);

#ifdef HAVE_CASA
  /*!
    \brief Conversion from cartesian to spherical coordinates

    \retval spherical -- Vector in spherical coordinates, \f$ (r,\phi,\theta) \f$
    \param cartesian  -- Vector in cartesian coordinates, \f$ (x,y,z) \f$
    \param anglesInDegrees -- Are the angles given in units of degrees? If
                    <i>yes</i> angles will be converted to radians before the 
		    conversion.
    
    \return status -- Set to <i>false</i> if an error was encountered.
  */
  bool Cartesian2Spherical (casa::Vector<double> &spherical,
			    casa::Vector<double> const &cartesian,
			    bool const &anglesInDegrees=false);
#endif
  
#ifdef HAVE_BLITZ
  /*!
    \brief Conversion from cartesian to spherical coordinates

    \retval spherical -- Vector in spherical coordinates, \f$ (r,\phi,\theta) \f$
    \param cartesian  -- Vector in cartesian coordinates, \f$ (x,y,z) \f$
    \param anglesInDegrees -- Are the angles given in units of degrees? If
                    <i>yes</i> angles will be converted to radians before the 
		    conversion.
    
    \return status -- Set to <i>false</i> if an error was encountered.
  */
  bool Cartesian2Spherical (blitz::Array<double,1> &spherical,
			    blitz::Array<double,1> const &cartesian,
			    bool const &anglesInDegrees=false);
#endif

  // ----------------------------------------------------------------------------
  // Cartesian (x,y,z) -> AzElRadius (Az,El,Radius)
  
  /*!
    \brief Conversion from cartesian to spherical coordinates

    \retval Az     -- Azimuth component of the vector, i.e. the angle running
                      from North (0 deg) to East (90 deg) clock-wise
    \retval El     -- Elevation component of the vector, i.e. the angle running
                      from the horizon (0 deg) to the zenith (90 deg)
    \retval Radius -- Radial distance from the coordinate origin
    \param x       -- \f$x\f$-component of the vector in cartesian coordinates
    \param y       -- \f$y\f$-component of the vector in cartesian coordinates
    \param z       -- \f$z\f$-component of the vector in cartesian coordinates
    \param anglesInDegrees -- Are the angles given in units of degrees? If
                    <i>yes</i> angles will be converted to radians before the 
		    conversion.
    
    \return status -- Set to <i>false</i> if an error was encountered.
  */
  bool Cartesian2AzElRadius (double &az,
			     double &el,
			     double &radius,
			     double const &x,
			     double const &y,
			     double const &z,
			     bool const &anglesInDegrees=false);
  
  bool Cartesian2AzElRadius (std::vector<double> &AzElRadius,
			     std::vector<double> const &cartesian,
			     bool const &anglesInDegrees=false);
  
#ifdef HAVE_CASA
  bool Cartesian2AzElRadius (casa::Vector<double> &AzElRadius,
			     casa::Vector<double> const &cartesian,
			     bool const &anglesInDegrees=false);
#endif
  
#ifdef HAVE_BLITZ
  bool Cartesian2AzElRadius (blitz::Array<double,1> &AzElRadius,
			     blitz::Array<double,1> const &cartesian,
			     bool const &anglesInDegrees=false);
#endif
  
  // ----------------------------------------------------------------------------
  // Cartesian (x,y,z) -> 

  
  // ============================================================================
  // 
  //  Conversion: Cylindrical (rho,phi,z) -> Other
  //
  // ============================================================================

  // ----------------------------------------------------------------------------
  // Cylindrical (rho,phi,z) -> Cartesian (x,y,z)

  /*!
    \brief Conversion from cylindrical to cartesian coordinates

    \retval x  -- \f$x\f$-component of the vector in cartesian coordinates
    \retval y  -- \f$y\f$-component of the vector in cartesian coordinates
    \retval z  -- \f$z\f$-component of the vector in cartesian coordinates
    \param rho -- \f$rho\f$-component of the vector in cylindrical coordinates
    \param phi -- \f$phi\f$-component of the vector in cylindrical coordinates
    \param z   -- \f$z\f$-component of the vector in cylindrical coordinates
    
    \return status -- Set to <i>false</i> if an error was encountered.
  */
  bool Cylindrical2Cartesian (double &x,
			      double &y,
			      double &z,
			      double const &rho,
			      double const &phi,
			      double const &z_cyl,
			      bool const &anglesInDegrees=false);
  
  /*!
    \brief Conversion from cylindrical to cartesian coordinates
  */
  bool Cylindrical2Cartesian (std::vector<double> &cartesian,
			      std::vector<double> const &cylindrical,
			      bool const &anglesInDegrees=false);

#ifdef HAVE_CASA
  bool Cylindrical2Cartesian (casa::Vector<double> &cartesian,
			      casa::Vector<double> const &cylindrical,
			      bool const &anglesInDegrees=false);
#endif
  
#ifdef HAVE_BLITZ
  bool Cylindrical2Cartesian (blitz::Array<double,1> &cartesian,
			      blitz::Array<double,1> const &cylindrical,
			      bool const &anglesInDegrees=false);
#endif

  // ----------------------------------------------------------------------------
  // Cylindrical (rho,phi,z) -> Spherical (r,phi,theta)
  
  /*!
    \brief Conversion from cylindrical to spherical coordinates

    \retval r     -- \f$r\f$ component of the vector in spherical coordinates
    \retval phi   -- \f$\phi\f$ component of the vector in spherical coordinates
    \retval theta -- \f$\theta\f$ component of the vector in spherical coordinates
    \param rho -- \f$rho\f$-component of the vector in cylindrical coordinates
    \param phi -- \f$phi\f$-component of the vector in cylindrical coordinates
    \param z   -- \f$z\f$-component of the vector in cylindrical coordinates
  */
  bool Cylindrical2Spherical (double &r,
			      double &phi,
			      double &theta,
			      double const &rho,
			      double const &phi_cyl,
			      double const &z,
			      bool const &anglesInDegrees=false);
  
  /*!
    \brief Conversion from cylindrical to spherical coordinates
  */
  bool Cylindrical2Spherical (std::vector<double> &spherical,
			      std::vector<double> const &cylindrical,
			      bool const &anglesInDegrees=false);
  
#ifdef HAVE_CASA
  /*!
    \brief Conversion from cylindrical to spherical coordinates
  */
  bool Cylindrical2Spherical (casa::Vector<double> &spherical,
			      casa::Vector<double> const &cylindrical,
			      bool const &anglesInDegrees=false);
#endif
  
#ifdef HAVE_BLITZ
  /*!
    \brief Conversion from cylindrical to spherical coordinates
  */
  bool Cylindrical2Spherical (blitz::Array<double,1> &spherical,
			      blitz::Array<double,1> const &cylindrical,
			      bool const &anglesInDegrees=false);
#endif
  
  // ============================================================================
  // 
  //  Conversion: North,East -> Other
  //
  // ============================================================================

  /*!
    \brief Conversion from North-East-Height to cartesian coordinates

    \retval x  -- \f$x\f$-component of the vector in cartesian coordinates
    \retval y  -- \f$y\f$-component of the vector in cartesian coordinates
    \retval z  -- \f$z\f$-component of the vector in cartesian coordinates
    \param N   -- North component of the input vector
    \param E   -- East component of the input vector
    \param H   -- Height component of the input vector
    
    \return status -- Set to <i>false</i> if an error was encountered.
  */
  inline bool NorthEastHeight2Cartesian (double &x,
					 double &y,
					 double &z,
					 double const &N,
					 double const &E,
					 double const &H)
    {
      bool status (true);

      try {
	x = E;
	y = N;
	z = H;
      } catch (std::string message) {
	std::cerr << "[NorthEastHeight2Cartesian] " << message << std::endl;
	status = false;
      }
      
      return status;
    }

  // ============================================================================
  // 
  //  Conversion: Spherical (r,phi,theta) -> other
  //
  // ============================================================================

  // ----------------------------------------------------------------------------
  // Spherical (r,phi,theta) -> Cartesian (x,y,z)

  /*!
    \brief Conversion from spherical to cartesian coordinates

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
  bool Spherical2Cartesian (double &x,
			    double &y,
			    double &z,
			    const double &r=1.0,
			    const double &phi=0.0,
			    const double &theta=0.0,
			    const bool &anglesInDegrees=false);

  /*!
    \brief Conversion from spherical to cartesian coordinates

    \retval cartesian -- Vector in cartesian coordinates, \f$ (x,y,z) \f$
    \param spherical  -- Vector in spherical coordinates, \f$ (r,\phi,\theta) \f$
    \param anglesInDegrees -- Are the angles given in units of degrees? If
           <i>yes</i> angles will be converted to radians before the conversion.

    \return status -- Set to <i>false</i> if an error was encountered.
  */
  bool Spherical2Cartesian (std::vector<double> &cartesian,
			      std::vector<double> const &spherical,
			      bool const &anglesInDegrees=false);
  
#ifdef HAVE_CASA
  bool Spherical2Cartesian (casa::Vector<double> &cartesian,
			    casa::Vector<double> const &spherical,
			    bool const &anglesInDegrees=false);
#endif

#ifdef HAVE_BLITZ
  bool Spherical2Cartesian (blitz::Array<double,1> &cartesian,
			    blitz::Array<double,1> const &spherical,
			    bool const &anglesInDegrees=false);

#endif

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
  vector<double> Spherical2Cartesian (vector<double> const &spherical,
				      bool const &anglesInDegrees);
#ifdef HAVE_CASA
  casa::Vector<double> Spherical2Cartesian (casa::Vector<double> const &spherical,
					    bool const &anglesInDegrees);
#endif
  

  // ----------------------------------------------------------------------------
  // Spherical (r,phi,theta) -> Cylindrical (rho,phi,z)

  /*!
    \brief Conversion from spherical to cylindrical coordinates

    \retval rho  --
    \retval phi  -- 
    \retval z    -- 
    \param r     -- \f$r\f$-component of the vector in spherical coordinates
    \param phi   -- \f$\phi\f$-component of the vector in spherical coordinates
    \param theta -- \f$\theta\f$-component of the vector in spherical coordinates
    \param anglesInDegrees -- Are the angles given in units of degrees? If
                    <i>yes</i> angles will be converted to radians before the 
		    conversion.
    
    \return status -- Set to <i>false</i> if an error was encountered.
  */
  bool Spherical2Cylindrical (double &rho,
			      double &phi_cyl,
			      double &z,
			      double const &r=1.0,
			      double const &phi=0.0,
			      double const &theta=0.0,
			      bool const &anglesInDegrees=false);

  /*!
    \brief Conversion from spherical to cylindrical coordinates

    \return status -- Set to <i>false</i> if an error was encountered.
  */
  bool Spherical2Cylindrical (std::vector<double> &cylindrical,
			      std::vector<double> const &spherical,
			      bool const &anglesInDegrees=false);

  /*!
    \brief Conversion from spherical to cylindrical coordinates

    \return status -- Set to <i>false</i> if an error was encountered.
  */
#ifdef HAVE_CASA
  bool Spherical2Cylindrical (casa::Vector<double> &cylindrical,
			      casa::Vector<double> const &spherical,
			      bool const &anglesInDegrees=false);
#endif

  // ============================================================================
  //
  //  Elementary conversion function, only using functionality of the standard
  //  library.
  //
  // ============================================================================
  
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

    \retval xyz             -- 
    \param  azze            -- 
    \param  anglesInDegrees -- Are the angles in the input vector are provided in
                              degrees? If yes, then an additional conversion step
			      is performed.
  */
  inline void azze2xyz (vector<double> &xyz,
			vector<double> const &azze,
			bool const &anglesInDegrees) {
    xyz = azze2xyz (azze,
		    anglesInDegrees);
  }
  
  /*!
    \brief Convert positions from Spherical to Cartesian coordinates

    Converts 3D vector representation from \f$ (r,\phi,\theta) \f$ to
    \f$ (x,y,z) \f$

    \retval cartesian      -- [position,3] Representation of the vector in
                              cartesian coordinates, \f$ (x,y,z) \f$    
    \param spherical       -- [position,3] Representation of the vector in
                              spherical coordinates
    \param anglesInDegrees -- Are the angles given in degrees?
  */
#ifdef HAVE_CASA
  inline void Spherical2Cartesian (casa::Matrix<double> &cartesian,
				   casa::Matrix<double> const &spherical,
				   bool const &anglesInDegrees) {
    casa::IPosition shape (spherical.shape());
    cartesian.resize(shape);
    for (int n(0); n<shape(0); n++) {
      cartesian.row(n) = Spherical2Cartesian (spherical.row(n),
					      anglesInDegrees);
    }
  }
#endif
  
  // ============================================================================
  //
  //  Conversion using CASA array classes
  //
  // ============================================================================

#ifdef HAVE_CASA
  
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
  casa::Vector<double> azel2Cartesian (const casa::Vector<double>& azel,
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
  casa::Vector<double> polar2Cartesian (casa::Vector<double> const &polar);

#endif
  
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
    blitz::Array<T,1> azel2Cartesian (const blitz::Array<T,1>& azel);  

  template <class T>
    blitz::Array<T,1> polar2Cartesian (blitz::Array<T,1> const &polar);

#endif
  
} // Namespace CR -- end

#endif /* VECTORCONVERSION_H */
  
