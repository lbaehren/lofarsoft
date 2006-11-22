/***************************************************************************
 *   Copyright (C) 2005-2006                                               *
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

#ifndef ALGEBRA_H
#define ALGEBRA_H

#include <casa/aips.h>
#include <casa/string.h>
#include <casa/Arrays.h>
#include <casa/Arrays/Array.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/Arrays/IPosition.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/Vector.h>
#include <scimath/Mathematics.h>
#include <casa/BasicSL/Complex.h>
#include <casa/BasicMath/Math.h>

using casa::Complex;
using casa::DComplex;
using casa::IPosition;
using casa::String;
using casa::Vector;

/*!
  \class Algebra.h

  \brief Algebraic operations on vectors and matrices

  \author Lars B&auml;hren

  \date 2005/11/29

  \test tAlgebra.cc

  <h3>Prerequisite</h3>

  <h3>Synopsis</h3>

  <h3>Example(s)</h3>

*/

namespace LOPES { // namespace LOPES -- begin
  
  /*!
    \brief Determine the sign of a real-valued number 
    
    \param x -- A real number.
    
    \return sign - The sign of <i>x</i>.
  */
  template <class T>
    T sign (const T& x);
  
  /*!
    \brief Determine the sign of a set of real-valued numbers

    \param x -- A set of real numbers

    \return sign - The signs of the \f$ x_i \f$.
  */
  template <class T>
    Vector<T> sign (const Vector<T>& x);

  /*!
    \brief Invert the order of elements in a vector.
  */
  template <class T>
    Vector<T> invertOrder (const Vector<T>& vec);

} // namespace LOPES -- end

#endif /* _ALGEBRA_H_ */
