/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
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

#ifndef COORDINATECONVERSION_H
#define COORDINATECONVERSION_H

#include <cmath>
#include <iostream>
#include <string>

#ifdef HAVE_BLITZ
#include <blitz/array.h>
using blitz::Range;
#endif

#ifdef HAVE_CASA
#include <casa/Arrays/Vector.h>
#endif

namespace CR { // NAMESPACE CR -- BEGIN
  
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
  } CoordType;
  
  /*!
    \file CoordinateConversion.h
    
    \ingroup Imaging
    
    \brief Conversion between different coordinate representations
    
    \author Lars B&auml;hren

    \date 2007/01/16

    \test tCoordinateConversion.cc
    
    <h3>Prerequisite</h3>
    
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

    <center>
    <table border="0">
      <tr>
	<td>\image html Spherical_Coordinates_Wikipedia.png</td>
	<td>\image html Cylindrical_Coordinates_Wikipedia.png</td>
      </tr>
      <tr bgcolor="#dddddd">
        <td>Convention used for spherical coordinates</td>
        <td>Convention used for cylindrical coordinates</td>
      </tr>
    </table>
    </center>

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

    <h3>Synopsis</h3>

    <ol>
      <li>Conversion to <b>cartesian coordinates</b>:
      \f[
        \left[ \begin{array}{c} x \\ y \\ z \end{array} \right]
	= \left[ \begin{array}{c} \rho \cos\phi \\ \rho \sin \phi \\ z
	\end{array} \right]
	= \left[  \begin{array}{c} r \cos\phi \sin\theta \\ r \sin\phi \sin\theta
	\\ r \cos\theta \end{array} \right]
      \f]
      <li>Conversion to <b>spherical coordinates</b>:
      \f[
        \left[ \begin{array}{c} r \\ \phi \\ \theta \end{array} \right]
	= \left[ \begin{array}{c} \sqrt{x^2 + y^2 + z^2} \\ \mathrm{arctan} (y/x)
	\\ \mathrm{arctan} \left( \frac{\sqrt{x^2 + y^2}}{z} \right) \end{array}
	\right]
	= \left[ \begin{array}{c} \sqrt{\rho^2 + z^2} \\ \phi \\
	\mathrm{arctan} (\rho/z) \end{array} \right]
      \f]
      <li>Conversion to <b>cylindrical coordinates</b>:
      \f[
        \left[ \begin{array}{c} \rho \\ \phi \\ z \end{array} \right]
	= \left[ \begin{array}{c} \sqrt{x^2 + y^2} \\ \mathrm{arctan} (y/x) \\ z
	\end{array} \right]
	= \left[ \begin{array}{c} r \sin\theta \\ \mathrm{arctan} \left(
	\frac{1}{\tan\phi} \right) \\ r \cos\theta \end{array} \right]
      \f]
    </ol>
    
    <h3>Example(s)</h3>
    
  */  

  // ============================================================================
  //
  //  Coordinate conversions
  //
  // ============================================================================

  /*!
    \brief Conversion from cartesian to spherical coordinates

    \retval r     -- \f$r\f$-component of the vector in spherical coordinates
    \retval phi   -- \f$\phi\f$-component of the vector in spherical coordinates
    \retval theta -- \f$\theta\f$-component of the vector in spherical coordinates
    \param x      -- \f$x\f$-component of the vector in cartesian coordinates
    \param y      -- \f$y\f$-component of the vector in cartesian coordinates
    \param z      -- \f$z\f$-component of the vector in cartesian coordinates
    
    \return status -- Set to <i>false</i> if an error was encountered.
  */
  bool cartesian2spherical (double &r,
			    double &phi,
			    double &theta,
			    const double &x,
			    const double &y,
			    const double &z);

  /*!
    \brief Conversion from cartesian to cylindrical coordinates

    \retval r   -- \f$r\f$-component of the vector in cylindrical coordinates
    \retval phi -- \f$phi\f$-component of the vector in cylindrical coordinates
    \retval h   -- \f$h\f$-component of the vector in cylindrical coordinates
    \param  x   -- \f$x\f$-component of the vector in cartesian coordinates
    \param  y   -- \f$y\f$-component of the vector in cartesian coordinates
    \param  z   -- \f$z\f$-component of the vector in cartesian coordinates
    
    \return status -- Set to <i>false</i> if an error was encountered.
  */
  bool cartesian2cylindrical (double &r,
			      double &phi,
			      double &h,
			      const double &x,
			      const double &y,
			      const double &z);

  /*!
    \brief Conversion from cylindrical to cartesian coordinates

    \retval x  -- \f$x\f$-component of the vector in cartesian coordinates
    \retval y  -- \f$y\f$-component of the vector in cartesian coordinates
    \retval z  -- \f$z\f$-component of the vector in cartesian coordinates
    \param r   -- \f$r\f$-component of the vector in cylindrical coordinates
    \param phi -- \f$phi\f$-component of the vector in cylindrical coordinates
    \param h   -- \f$h\f$-component of the vector in cylindrical coordinates
    
    \return status -- Set to <i>false</i> if an error was encountered.
  */
  bool cylindrical2cartesian (double &x,
			      double &y,
			      double &z,
			      const double &r,
			      const double &phi,
			      const double &h);

#ifdef HAVE_BLITZ

  /*!
    \brief Conversion from cartesian to spherical coordinates

    \retval spherical -- [3] Spherical coordinates, \f$ \vec x =
                         (r,\phi,\theta) \f$
    \param  cartesian -- [3] Cartesian coordinates, \f$ \vec x = (x,y,z) \f$
    
    \return status -- Set to <i>false</i> if an error was encountered.
  */
  bool cartesian2spherical (blitz::Array<double,1> &spherical,
			    const blitz::Array<double,1> &cartesian);

  /*!
    \brief Conversion from cartesian to spherical coordinates

    \retval spherical -- [nofPoints,3] Spherical coordinates, \f$ \vec x =
                         (r,\phi,\theta) \f$
    \param  cartesian -- [nofPoints,3] Cartesian coordinates, \f$ \vec x =
                         (x,y,z) \f$
    
    \return status -- Set to <i>false</i> if an error was encountered.
  */
  bool cartesian2spherical (blitz::Array<double,2> &spherical,
			    const blitz::Array<double,2> &cartesian);

  /*!
    \brief Conversion from cartesian to cylindrical coordinates

    \retval cylindrical -- Cylindrical coordinates, \f$ \vec x = (\rho,\phi,z) \f$
    \param  cartesian   -- Cartesian coordinates, \f$ \vec x = (x,y,z) \f$
    
    \return status -- Set to <i>false</i> if an error was encountered.
  */
  bool cartesian2cylindrical (blitz::Array<double,1> &cylindrical,
			      const blitz::Array<double,1> &cartesian);

  /*!
    \brief Conversion from cartesian to cylindrical coordinates

    \retval cylindrical -- Cylindrical coordinates, \f$ \vec x = (\rho,\phi,z) \f$
    \param  cartesian   -- Cartesian coordinates, \f$ \vec x = (x,y,z) \f$
    
    \return status -- Set to <i>false</i> if an error was encountered.
  */
  bool cartesian2cylindrical (blitz::Array<double,2> &cylindrical,
			      const blitz::Array<double,2> &cartesian);

  /*!
    \brief Conversion from spherical to cartesian coordinates

    \retval cartesian -- [3] Cartesian coordinates, \f$ \vec x = (x,y,z) \f$
    \param  spherical -- [3] Spherical coordinates, \f$ \vec x =
                         (r,\phi,\theta) \f$
    \param anglesInDegrees -- Are the angles given in units of degrees? If
                    <i>yes</i> angles will be converted to radians before the 
		    conversion.
    
    \return status -- Set to <i>false</i> if an error was encountered.
  */
  bool spherical2cartesian (blitz::Array<double,1> &cartesian,
			    const blitz::Array<double,1> &spherical,
			    const bool &anglesInDegrees=false);
  
  /*!
    \brief Conversion from spherical to cartesian coordinates

    \retval cartesian -- [nofPoints,3] Cartesian coordinates,
                         \f$\vec x = (x,y,z)\f$;
    \param  spherical -- [nofPoints,3] Spherical coordinates,
                         \f$\vec x = (r,\phi,\theta)\f$, with angles given in
			 radians.
    
    \return status -- Set to <i>false</i> if an error was encountered.
  */
  bool spherical2cartesian (blitz::Array<double,2> &cartesian,
			    const blitz::Array<double,2> &spherical);
  
  /*!
    \brief Conversion from cylindrical to cartesian coordinates
    
    \retval cartesian   -- Cartesian coordinates, \f$ \vec x = (x,y,z) \f$
    \param  cylindrical -- Cylindrical coordinates, \f$ \vec x = (\rho,\phi,z) \f$
    
    \return status -- Set to <i>false</i> if an error was encountered.
  */
  bool cylindrical2cartesian (blitz::Array<double,1> &cartesian,
			      const blitz::Array<double,1> &cylindrical);
  
  /*!
    \brief Conversion from cylindrical to cartesian coordinates
    
    \retval cartesian   -- Cartesian coordinates, \f$ \vec x = (x,y,z) \f$
    \param  cylindrical -- Cylindrical coordinates, \f$ \vec x = (\rho,\phi,z) \f$
    
    \return status -- Set to <i>false</i> if an error was encountered.
  */
  bool cylindrical2cartesian (blitz::Array<double,2> &cartesian,
			      const blitz::Array<double,2> &cylindrical);

#endif

}  // NAMESPACE CR -- END

#endif /* COORDINATECONVERSION_H */
  
