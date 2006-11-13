
#if !defined(MATHGEOMETRY_H)
#define MATHGEOMETRY_H

/* $Id: MathGeometry.h,v 1.5 2006/11/08 17:42:42 bahren Exp $ */

// C++ Standard library
#include <ostream>
#include <iostream>
#include <string>
#include <fstream>
using namespace std;

// AIPS++ wrapper classes
#ifdef HAVE_AIPS_H
#include <casa/aips.h>
#include <casa/string.h>
#include <casa/Arrays.h>
#include <tasking/Glish.h>
#include <measures/Measures.h>
#include <casa/namespace.h>
#endif

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

#ifdef HAVE_AIPS_H
  
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
  
#endif
  
} // Namespace LOPES -- end
  
#endif
  
