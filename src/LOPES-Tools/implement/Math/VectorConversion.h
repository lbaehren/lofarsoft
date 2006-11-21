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

/* $Id: VectorConversion.h,v 1.1 2006/11/08 17:42:42 bahren Exp $*/

#ifndef VECTORCONVERSION_H
#define VECTORCONVERSION_H

// Standard library header files
#include <cmath>
#include <string>

// External libraries
#ifdef HAVE_BLITZ_H
#include <blitz/array.h>
#endif

// AIPS++/CASA header files
#ifdef HAVE_CASA
#include <casa/aips.h>
#include <casa/Arrays.h>
#include <casa/Exceptions/Error.h>
using casa::Vector;
#endif

namespace LOPES { // Namespace LOPES -- begin
  
  /*!
    \file VectorConversion.cc

    \ingroup Math
    
    \brief A collection of function to handle conversion between vectors
    
    \author Lars B&auml;hren
    
    \date 2006/11/08
    
    \test tVectorConversion.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
      <li>[start filling in your text here]
    </ul>
    
    <h3>Synopsis</h3>
    
    <h3>Example(s)</h3>    
  */

  /*!
    \brief Conversion from Azimuth-Elevation to cartesian coordinates

    \retval x -- 
    \retval y -- 
    \retval z -- 
    \param r  -- 
    \param az -- 
    \param el -- 
  */
  template <class T>
    void azel2xyz (T &x, T &y, T &z,
		   T const &r, T const &az, T const &el);
  
  template <class T>
    void polar2xyz (T &x, T &y, T &z,
		    T const &r, T const &theta, T const &phi);

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
      x = -\cos(\theta) \cos(\phi) \ , \quad
      y = -\cos(\theta) \sin(\phi) \ , \quad
      z =  \sin(\theta)
    \f]
    If the source is located within the near-field of the array, we have 
    to take into account the distance:
    \f[
      x = - r\, \cos(\theta) \cos(\phi) \ , \quad
      y = - r\, \cos(\theta) \sin(\phi) \ , \quad
      z =   r\, \sin(\theta)
    \f]

    \param azel -- (az,el) coordinates

    \return xyz -- Vector in cartesian coordinates
   */
  template <class T>
    Vector<T> azel2cartesian (const Vector<T>& azel);  

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
  template <class T>
    Vector<T> polar2cartesian (Vector<T> const &polar);

#endif
  
  // ============================================================================
  //
  //  Conversion using Blitz++ array classes
  //
  // ============================================================================

#ifdef HAVE_BLITZ_H

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
  
} // Namespace LOPES -- end

#endif /* VECTORCONVERSION_H */
  
