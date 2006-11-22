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

#ifndef MATHALGEBRA_H
#define MATHALGEBRA_H

// AIPS++ wrapper classes
#include <casa/aips.h>
#include <casa/string.h>
#include <casa/Arrays.h>
#include <casa/Arrays/Array.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/Arrays/IPosition.h>
#include <casa/Arrays/Matrix.h>
#include <scimath/Mathematics/MatrixMathLA.h>
#include <casa/Arrays/Vector.h>
#include <scimath/Mathematics.h>
#include <casa/BasicSL/Complex.h>
#include <scimath/Mathematics.h>
#include <scimath/Mathematics/FFTServer.h>
#include <casa/BasicMath/Math.h>
#include <casa/Quanta.h>
#include <casa/OS/Time.h>

using casa::IPosition;
using casa::Matrix;
using casa::Vector;
using casa::String;

/*!
  \brief Algebraic operations on vectors and matrices
  
  \author Lars B&auml;hren
  
  \date 2004/10
*/

namespace LOPES { // Namespace LOPES -- begin
  
  /*!
    \brief L1-Norm of a vector.
    
    \param vec   -- Vector of real numbers.
    
    \return norm -- L1-Norm of the vector.
  */
  template <class T> T L1Norm (const Vector<T>&);
  
  /*!
    \brief Euclidean Norm of a vector
    
    <a href="http://mathworld.wolfram.com/EuclideanNorm.html">Euclidean norm</a>
    (\f$ \mathcal L^2 \f$ norm) of a vector:
    
    \f$ |\vec x| = \sqrt{\sum_{i} x_i^2} \f$
  */
  template <class T> T L2Norm (const Vector<T>& vec);
  
  /*!
    \brief Split a range of values into a number of subsets.
    
    Divide a range of values, \f$ [x_{\rm min},x_{\rm max}] \f$, into \f$ N \f$
    subsets.
    
    \param rangeValues  - Range of values, \f$ [x_{\rm min},x_{\rm max}] \f$.
    \param nofIntervals - The number of intervals, \f$ N \f$, into which the
    range of values is subdivided.
    \param method       - Method to divide the frequency range into sub-bands:
    <i>lin</i> (linear; this also is the default,
    <i>log</i> (logarithmic).
    
    \return indices - Matrix with the indices defining the limits of the 
    sub-bands; [2,nofIntervals]
  */
  Matrix<int> splitRange (const Vector<int>& rangeValues,
			  const int nofIntervals,
			  const String method);
  
} // Namespace LOPES -- end

#endif
