/***************************************************************************
 *   Copyright (C) 2005                                                    *
 *   Lars Bahren (bahren@astron.nl)                                        *
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

/* $Id: ArrayMath.h,v 1.4 2006/10/31 18:24:08 bahren Exp $*/

#ifndef ARRAYMATH_H
#define ARRAYMATH_H

#include <casa/aips.h>
#include <casa/Arrays.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/BasicMath/Math.h>
#include <casa/BasicSL/Complex.h>

using casa::Array;
using casa::ArrayConformanceError;
using casa::Complex;
using casa::Double;
using casa::DComplex;
using casa::Float;

namespace LOPES {

/*!
  \file ArrayMath.h

  \brief Mathematical operations for combination of complex and real arrays.

  \author Lars B&auml;hren

  \date 2005/07/27

  \test tArrayMath.cc

  <h3>Prerequisite</h3>

  <ul type="square">
    <li>[AIPS++] Array -- A templated N-D Array class with zero origin.
    <li>[AIPS++] ArrayMath -- Mathematical operations for Arrays.
  </ul>

  <h3>Synopsis</h3>

  For the four basic operations between two complex numbers \f$ c_1 \f$ and
  \f$ c_2 \f$, where \f$ c_j = a_j + i b_j \f$, we have:
  <ol>
    <li>Addition
    \f[ c_1+c_2 = (a_1+a_2) + i (b_1+b+2) \f]
    <li>Substraction
    \f[ c_1-c_2 = (a_1-a_2) + i (b_1-b+2) \f]
    <li>Multiplication
    \f[ c_1*c_2 = \left( a_1 a_2 - b_1 b_2 \right) + i \left( a_1 b_2 a_2 b_1 \right) \f]
    <li>Division
    \f[ \frac{c_1}{c_2} = \frac{a_1 a_2 + b_1 b_2 + i \left( -a_1 b_2 + b_1 a_2
    \right)}{a_2^2 + b_2^2} \f]
  </ol>

  The basic operations implemented with this class are:
  <ul>
    <li>Element by element arithmetic of two arrays
    <li>Element by element arithmetic between an array and a scalar
  </ul>
*/


/*!
  \brief Element by element arithmetic of two arrays: addition.

  <i>left</i> and <i>right</i> must be conformant.

  \param left  -- Array of real values.
  \param right -- Array of complex values.
  
*/
template <class T, class U> Array<U> operator+ (const Array<T> &left,
						const Array<U> &right);

/*!
  \brief Element by element arithmetic of two arrays: substraction.

  <i>left</i> and <i>right</i> must be conformant.

  \param left  -- Array of real values.
  \param right -- Array of complex values.
  
*/
template <class T, class U>  Array<U> operator- (const Array<T> &left,
						 const Array<U> &right);
/*!
  \brief Element by element arithmetic of two arrays: multiplication.

  <i>left</i> and <i>right</i> must be conformant.

  \param left  -- Array of real values.
  \param right -- Array of complex values.
*/
template <class T, class U>  Array<U> operator* (const Array<T> &left,
						 const Array<U> &right);

/*!
  \brief Element by element arithmetic between an array and a scalar: addition.

  Templated for combinations (Float,Complex) and (Double,DComplex).

  \param left  -- Array of real values.
  \param right -- Complex valued scalar.  
 */
template <class T, class U> Array<U> operator+ (const Array<T> &left,
						const U &right);

/*!
  \brief Element by element arithmetic between an array and a scalar: substraction.

  Templated for combinations (Float,Complex) and (Double,DComplex).

  \param left  -- Array of real values.
  \param right -- Complex valued scalar.  
 */
template <class T, class U> Array<U> operator- (const Array<T> &left,
						const U &right);

/*!
  \brief  Element by element arithmetic between an array and a scalar: multiplication

  Templated for combinations (Float,Complex) and (Double,DComplex).

  \param left  -- Array of real values.
  \param right -- Complex valued scalar.  
*/
template <class T, class U> Array<U> operator* (const Array<T> &left,
						const U &right);

} // Namespace LOPES -- end

#endif
