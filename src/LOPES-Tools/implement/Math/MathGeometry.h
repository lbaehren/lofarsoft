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

#ifndef MATHGEOMETRY_H
#define MATHGEOMETRY_H

// C++ Standard library
#include <ostream>
#include <iostream>
#include <string>
#include <fstream>

// AIPS++ wrapper classes
#include <casa/aips.h>
#include <casa/string.h>
#include <casa/Arrays.h>
#include <tasking/Glish.h>
#include <measures/Measures.h>

using casa::Vector;

/*!
  \file Math::MathGeometry.h

  \brief Functions for geometrical operations.

  \author Lars B&auml;hren

  <h3>Contents</h3>

  <ul>
    <li>Conversion of units
    <li>Conversion of coordinates
  </ul>
*/

namespace LOPES { // Namespace LOPES -- begin
  
  // === Conversion of units =================================================
  
  /*!
    \brief Convert angular position from degree to radian
    
    Common source of confusion can be the factor required to convert an angle 
    to its radian measure:
    \f[
    \frac{\alpha [^\circ]}{360 [^\circ]} = \frac{s}{ 2\pi}
    \qquad \longrightarrow \qquad
    \alpha [^\circ] = \frac{180 [^\circ]}{} \cdot s
    \qquad \hbox{and} \qquad
    s = \frac{\pi}{180 [^\circ]} \cdot \alpha [^\circ]
    \f]
  */
  double deg2rad (double const &deg);
  
  //! Convert angular positions from radian to degree
  double rad2deg (double const &rad);

  //! Convert angular positions from degree to radian
  Vector<double> deg2rad (const Vector<double>& deg);
  
  //! Convert angular position from radian to degree
  Vector<double> rad2deg (const Vector<double>& rad);

  /*!
    \brief Convert (zenit,azimut) angle to cartesian (x,y) position
    
    \param ze - Zenith angle (in degrees)
    \param az - Azimuth angle (in degrees)
    
    \f[
    x = \frac{\rm ze}{90^\circ} \cdot \sin ({\rm az}) \ , \quad
    y = \frac{\rm ze}{90^\circ} \cdot \cos ({\rm az})
    \f]
  */
  Vector<double> zeaz2cartesian (double const &ze,
				 double const &az);
  
  Vector<double> zeaz2cartesian (const Vector<double>&);
  
} // Namespace LOPES -- end

#endif
  
