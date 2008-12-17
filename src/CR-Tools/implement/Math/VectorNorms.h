/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2005                                                    *
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

#ifndef VECTORNORMS_H
#define VECTORNORMS_H

#include <crtools.h>

#include <vector>

#ifdef HAVE_CASA
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
using casa::Double;
using casa::Float;
using casa::IPosition;
using casa::String;
using casa::uInt;
#else
#ifdef HAVE_BLITZ
#include <blitz/array.h>
#endif
#endif

namespace CR { // namespace CR -- begin
  
  /*!
    \file VectorNorms.h
    
    \ingroup CR_Math
    
    \brief Various vector norms
    
    \author Lars B&auml;hren
    
    \date 2005/11/29
    
    \test tVectorNorms.cc
    
    <h3>Synopsis</h3>
    
    <ul>
      <li>All functions are templated to allow maximum flexibility.
      <li>Most function support usage of:
      <ol>
        <li>C++ arrays
	<li>STD vectors
	<li>Blitz++ arrays
	<li>CASA arrays
      </ol>
    </ul>
    
    Implemented computations:
    <ul>
      <li>The <a href="http://mathworld.wolfram.com/L1-Norm.html">L1-Norm</a> is
      a vector norm defined for a vector \f$ \vec x = [x_1,x_2,...,x_N]^T \f$
      with complex entries by 
      \f[ |\vec x|_{1} = \sum_{i=1}^{N} |x_i| \f]
      <li>The <a href="http://mathworld.wolfram.com/L2-Norm.html">L2-Norm</a> is a
      vector norm defined for a vector \f$ \vec x = [x_1,x_2,...,x_N]^T \f$ with
      complex entries by
      \f[ |\vec x| = \sqrt{\sum_{i=1}^{N} |x_i|^2} \f]
      where \f$|x_{i}|\f$ on the right denotes the complex modulus. The
      \f$\ell^2\f$-norm is the vector norm that is commonly encountered in vector
      algebra and vector operations (such as the
      <a href="http://mathworld.wolfram.com/DotProduct.html">dot product</a>),
      where it is commonly denoted \f$|\vec x|\f$.
      <li>Normalization of a vector \f$ \vec x = [x_1,x_2,...,x_N]^T \f$:
      \f[ \hat x = \vec x / |\vec x| \f]
    </ul>
    
    <h3>Example(s)</h3>
    
    <ol>
      <li>Scalar product of two vectors:
      \code
      int nelem (3);
      std::vector<double> x (nelem);
      std::vector<double> y (nelem);

      double product = scalar_product (x,y);
      \endcode
      <li>Computation of L1-Norm of a vector:
      \code
      std::vector<double> x;
      double l1norm;
      
      l1norm = L1Norm (x);
      \endcode
    </ol>
  */
  
  // ============================================================================
  //
  //  
  //
  // ============================================================================

  /*!
    \brief Compute the scalar product between to vectors 

    \param x -- First vector to enter the scalar product
    \param y -- Second vector to enter the scalar product

    \return product -- Scalar product of the two vectors \f$x\f$ and \f$y\f$.
  */
  template <class T>
    T scalar_product (std::vector<T> const &x,
		      std::vector<T> const &y)
    {
      T product(0);
      
      scalar_product (product,x,y);
      
      return product;
    }

  /*!
    \brief Compute the scalar product between to vectors 

    \param x -- First vector to enter the scalar product
    \param y -- Second vector to enter the scalar product

    \retval product  -- Scalar product of the two vectors \f$x\f$ and \f$y\f$.
  */
  template <class T>
    void scalar_product (T &product,
			 std::vector<T> const &x,
			 std::vector<T> const &y)
    {
      product = 0;
      
      if (x.size() == y.size()) {
	for (uint n(0); n<x.size(); n++) {
	  product += x[n]*y[n];
	}
      } 
    }
  
  // ============================================================================
  //
  //  Vector norms
  //
  // ============================================================================

  /*!
    \brief In-place normalization of a vector

    Replaces vector \f$ \vec x \f$ by normalized vector
    \f$ \hat x = \vec x / |\vec x| \f$

    \param vec   -- Vector to normlize to length 1.
    \param nelem -- Number of elements in the vector.
  */
  template <class T>
    void normalize (T *vec,
		    unsigned int const &nelem)
    {
      T length = L2Norm (vec,nelem);

      if (length != 0) {
	for (unsigned int n(0); n<nelem; n++) {
	  vec[n] /= length;
	}
      }
    }

  /*!
    \brief In-place normalization of a vector

    \param vec   -- Vector to normlize to length 1.
  */
  template <class T>
    void normalize (std::vector<T> &vec)
    {
      unsigned int nelem(vec.size());
      T length = L2Norm (vec);

      if (length != 0) {
	for (unsigned int n(0); n<nelem; n++) {
	  vec[n] /= length;
	}
      }
    }

#ifdef HAVE_CASA
  /*!
    \brief In-place normalization of a vector

    \param vec   -- Vector to normlize to length 1.
  */
  template <class T>
    void normalize (casa::Vector<T> &vec)
    {
      unsigned int nelem(vec.nelements());
      T length = L2Norm (vec);
      
      if (length != 0) {
	for (unsigned int n(0); n<nelem; n++) {
	  vec(n) /= length;
	}
      }
    }
#endif

  /*!
    \brief Out-of-place normalization of a vector

    \retval nvec -- Normalized version of the input vector
    \param vec   -- Vector to normlize to length 1.
  */
  template <class T>
    void normalize (std::vector<T> &nvec,
		    std::vector<T> const &vec)
    {
      unsigned int nelem(vec.size());
      T length = L2Norm (vec);
      
      for (unsigned int n(0); n<nelem; n++) {
	nvec[n] = vec[n]/length;
      }
    }

#ifdef HAVE_CASA
  /*!
    \brief Out-of-place normalization of a vector

    \retval nvec -- Normalized version of the input vector
    \param vec   -- Vector to normlize to length 1.
  */
  template <class T>
    void normalize (casa::Vector<T> &nvec,
		    casa::Vector<T> const &vec)
    {
      T length = L2Norm (vec);

      if (length != 0) {
	nvec = vec/length;
      }
    }
#endif

  // ----------------------------------------------------------------------------
  // L1-Norm

  /*!
    \brief Compute the L1-Norm of a vector

    The <a href="http://mathworld.wolfram.com/L1-Norm.html">L1-Norm</a> is a
    vector norm defined for a vector \f$ \vec x = [x_1,x_2,...,x_N]^T \f$ with
    complex entries by 
    \f[ |\vec x|_{1} = \sum_{i=1}^{N} |x_i| \f]

    \param vec   -- Vector for which to comput the norm.
    \param nelem -- Number of elements in the vector.

    \return norm -- L1-norm of the vector
  */
  template <class T>
    T L1Norm (T const *vec,
	      unsigned int const &nelem)
    {
      T sum (0.0);
      
      for (unsigned int n(0); n<nelem; n++) {
	sum += fabs(vec[n]);
      }
      
      return sum;
    }
  
  /*!
    \brief Compute the L1-Norm of a vector

    The <a href="http://mathworld.wolfram.com/L1-Norm.html">L1-Norm</a> is a
    vector norm defined for a vector \f$ \vec x = [x_1,x_2,...,x_N]^T \f$ with
    complex entries by 
    \f[ |\vec x|_{1} = \sum_{i=1}^{N} |x_i| \f]

    \param vec   -- Vector for which to comput the norm.

    \return norm -- L1-norm of the vector
  */
  template <class T>
    T L1Norm (std::vector<T> const &vec)
    {
      unsigned int nelem(vec.size());
      T sum (0.0);
      
      for (unsigned int n(0); n<nelem; n++) {
	sum += fabs(vec[n]);
      }
      
      return sum;
    }
  
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
    T L1Norm (casa::Vector<T> const &vec)
    {
      return sum(fabs(vec));
    }
#else
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
    T L1Norm (blitz::Array<T,1> const &vec)
    {
      return sum(fabs(vec));
    }
#endif
#endif
  
  // ----------------------------------------------------------------------------
  // L2-Norm

  /*!
    \brief Compute the L2-Norm of a vector

    \param vec   -- Vector for which to comput the norm.
    \param nelem -- Number of elements in the vector.

    \return norm -- L2-norm of the vector
  */
  template <class T>
    T L2Norm (T const *vec,
	      unsigned int const &nelem)
    {
      T sum2 (0.0);
      
      for (unsigned int n(0); n<nelem; n++) {
	sum2 += vec[n]*vec[n];
      }
      
      return sqrt (sum2);
    }

  /*!
    \brief Compute the L2-Norm of a vector

    \param vec   -- Vector for which to comput the norm.

    \return norm -- L2-norm of the vector
  */
  template <class T>
    T L2Norm (std::vector<T> const &vec)
    {
      uint nelem = vec.size();
      T sum2     = 0.0;
      
      for (unsigned int n(0); n<nelem; n++) {
	sum2 += vec[n]*vec[n];
      }
      
      return sqrt (sum2);
    }

#ifdef HAVE_CASA

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
    T L2Norm (casa::Vector<T> const &vec)
    {
      T norm (0.0);
      norm = sqrt(sum(pow(vec,2)));
      return norm;
    }
#else
#ifdef HAVE_BLITZ
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
    T L2Norm (blitz::Array<T,1> const &vec)
    {
      return sqrt(sum(pow2(vec)));
    }
#endif
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
    T sign (T const &x)
    {
      if (x<0) return static_cast<T>(-1);
      else if (x>0) return static_cast<T>(+1);
      else return static_cast<T>(0);
    }
  
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
    casa::Vector<T> sign (casa::Vector<T> const &x)
    {
      int nelem (x.nelements());
      casa::Vector<T> s(nelem);
      
      for (int n=0; n<nelem; ++n) {
	s(n) = sign (x(n));
      }
      
      return s;
    }
  
  /*!
    \brief Invert the order of elements in a vector.
  */
  template <class T>
    casa::Vector<T> invertOrder (casa::Vector<T> const &vec);
#endif

} // namespace CR -- end

#endif /* _VECTORNORMS_H_ */
