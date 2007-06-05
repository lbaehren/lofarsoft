/***************************************************************************
 *   Copyright (C) 2005                                                  *
 *   Lars Bahren (<mail>)                                                     *
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

/* $Id: Algebra.h,v 1.2 2006/10/31 18:24:08 bahren Exp $*/

#ifndef _VECTORNORMS_H_
#define _VECTORNORMS_H_

#include <vector>

#ifdef HAVE_BLITZ
#include <blitz/array.h>
#endif

#ifdef HAVE_CASA
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
using casa::uInt;
#endif

namespace CR { // namespace CR -- begin

/*!
  \brief Various vector norms
  
  \ingroup Math

  \author Lars B&auml;hren

  \date 2005/11/29

  \test tVectorNorms.cc

  <h3>Prerequisite</h3>

  <h3>Synopsis</h3>

  <h3>Example(s)</h3>

*/

  // ============================================================================
  //
  //  Vector norms
  //
  // ============================================================================
  
  /*!
    \brief Compute the L1-Norm of a vector

    \param vec   -- Vector for which to comput the norm.
    \param nelem -- Number of elements in the vector.

    \return norm -- L1-norm of the vector
  */
  template <class T>
    T L1Norm (T const *vec,
	      unsigned int const &nelem);
  
  /*!
    \brief Compute the L2-Norm of a vector

    \param vec   -- Vector for which to comput the norm.
    \param nelem -- Number of elements in the vector.

    \return norm -- L2-norm of the vector
  */
  template <class T>
    T L2Norm (T const *vec,
	      unsigned int const &nelem);
  
  /*!
    \brief Compute the L1-Norm of a vector

    \param vec   -- Vector for which to comput the norm.

    \return norm -- L1-norm of the vector
  */
  template <class T>
    T L1Norm (std::vector<T> const &vec);
  
  /*!
    \brief Compute the L2-Norm of a vector

    \param vec   -- Vector for which to comput the norm.

    \return norm -- L2-norm of the vector
  */
  template <class T>
    T L2Norm (std::vector<T> const &vec);
  
#ifdef HAVE_BLITZ

  /*!
    \brief Compute the L1-Norm of a vector

    The <a href="http://mathworld.wolfram.com/L1-Norm.html">L1-Norm</a> is a
    vector norm defined for a vector \f$ \vec x = [x_1,x_2,...,x_N]^T \f$ with
    complex entries by 
    \f[ |\vec x|_{1} = \sum_{i=1}^{N} |x_i| \f]

    \param vec -- The input vector

    \return norm -- The L1-Norm for the vector
  */
  template <class T>
    T L1Norm (blitz::Array<T,1> const &vec);
  
  /*!
    \brief Compute the L2-Norm of a vector

    The <a href="http://mathworld.wolfram.com/L2-Norm.html">L2-Norm</a> is a
    vector norm defined for a vector \f$ \vec x = [x_1,x_2,...,x_N]^T \f$ with
    complex entries by
    \f[ |\vec x| = \sqrt{\sum_{i=1}^{N} |x_i|^2} \f]
    where \f$|x_{i}|\f$ on the right denotes the complex modulus. The
    \f$\ell^2\f$-norm is the vector norm that is commonly encountered in vector
    algebra and vector operations (such as the
    <a href="http://mathworld.wolfram.com/DotProduct.html">dot product</a>),
    where it is commonly denoted \f$|\vec x|\f$.

    \param vec -- The input vector

    \return norm -- The \f$\ell^2\f$-norm for the vector
  */
  template <class T>
    double L2Norm (blitz::Array<double,1> const &vec);
  
#endif

#ifdef HAVE_CASA

  /*!
    \brief Compute the L1-Norm of a vector

    The <a href="http://mathworld.wolfram.com/L1-Norm.html">L1-Norm</a> is a
    vector norm defined for a vector \f$ \vec x = [x_1,x_2,...,x_N]^T \f$ with
    complex entries by 
    \f[ |\vec x|_{1} = \sum_{i=1}^{N} |x_i| \f]

    \param vec -- The input vector

    \return norm -- The L1-Norm for the vector
  */
  template <class T>
    T L1Norm (const casa::Vector<T> &vec);
  
  /*!
    \brief Compute the L2-Norm of a vector

    The <a href="http://mathworld.wolfram.com/L2-Norm.html">L2-Norm</a> is a
    vector norm defined for a vector \f$ \vec x = [x_1,x_2,...,x_N]^T \f$ with
    complex entries by
    \f[ |\vec x| = \sqrt{\sum_{i=1}^{N} |x_i|^2} \f]
    where \f$|x_{i}|\f$ on the right denotes the complex modulus. The
    \f$\ell^2\f$-norm is the vector norm that is commonly encountered in vector
    algebra and vector operations (such as the
    <a href="http://mathworld.wolfram.com/DotProduct.html">dot product</a>),
    where it is commonly denoted \f$|\vec x|\f$.

    \param vec -- The input vector

    \return norm -- The \f$\ell^2\f$-norm for the vector
  */
  template <class T>
    double L2Norm (const casa::Vector<double> &vec);
  
#endif

  // ============================================================================
  //
  //  Sign (of vector elements)
  //
  // ============================================================================
  
  /*!
    \brief Determine the sign of a real-valued number 
    
    \param x -- A real number.
    
    \return sign - The sign of <i>x</i>.
  */
  template <class T>
    T sign (T const &x);
  
  /*!
    \brief Determine the sign of a real-valued number 
    
    \param x -- A real number.
    
    \return sign - The sign of <i>x</i>.
  */
  template <class T>
    std::vector<T> sign (std::vector<T> const &x);
  
#ifdef HAVE_CASA
  
  /*!
    \brief Determine the sign of a set of real-valued numbers
    
    \param x -- A set of real numbers
    
    \return sign - The signs of the \f$ x_i \f$.
  */
  template <class T>
    casa::Vector<T> sign (casa::Vector<T> const &x);

  /*!
    \brief Invert the order of elements in a vector.
  */
  template <class T>
    casa::Vector<T> invertOrder (casa::Vector<T> const &vec);

#endif

} // namespace CR -- end

#endif /* _VECTORNORMS_H_ */
