/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2005                                                    *
 *   Lars Baehren (bahren@astron.nl)                                       *
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

/* $Id$*/

#ifndef _LINEARREGRESSION_H_
#define _LINEARREGRESSION_H_

#include <casa/aips.h>
#include <casa/Arrays.h>
#include <casa/Arrays/Vector.h>
#include <casa/Arrays/ArrayMath.h>

#include <casa/namespace.h>

/*!
  \class LinearRegression.h

  \ingroup Math

  \brief Brief description for class LinearRegression

  \author Lars B&auml;hren

  \date 2005/08/03

  \test tLinearRegression.cc

  <h3>Prerequisite</h3>

  <h3>Synopsis</h3>

  Given two vectors \f$ \mathbf{x} = \{ x_1, ..., x_N \} \f$ and
  \f$ \mathbf{y} = \{ y_1, ..., y_N \} \f$ of input values,
  performs a linear regression
  \f[ f(x) = a + b x \f]
  where
  <ul type="square">
    <li>the slope of the linear function
    \f[ a = \langle y \rangle - b \cdot \langle x \rangle \f]
    <li>the intersection point with the y-axis
    \f[
      b = \frac{\sum_{i} (x_i - \langle x \rangle) \cdot
      (y_i - \langle y_i \rangle)}{\sum_i (x_i - \langle x \rangle)^2}
    \f]
    <li>the \f$ \chi^2 \f$ of the fit
    \f[ \chi^2 = \sum_i (f(x_i) - y_i)^2 \f]
  
*/

template <class T> class LinearRegression {

  //! Slope of the function.
  T a_p;
  
  //! Function value at x=0.
  T b_p;

  //! Function values at the input points.
  Vector<T> y_p;

  //! Chi^2 of the fit.
  T chisq_p;

 public:

  // --- Construction ----------------------------------------------------------

  /*!
    \brief Default constructor.
  */
  LinearRegression ();

  /*!
    \brief Argumented constructor.

    \param x -- 
    \param y -- Function values \f$ y_i (x_i) \f$.
  */
  LinearRegression (const Vector<T>& x,
		    const Vector<T>& y);

  // --- Destruction -----------------------------------------------------------

  /*!
    \brief Destructor
  */
  ~LinearRegression ();

  // --- Operators -------------------------------------------------------------

  LinearRegression<T> &operator=(const LinearRegression<T> &other);

  // --- Parameters ------------------------------------------------------------

  /*!
    \brief Slope of the function.

    \return a -- Slope of the function.
   */
  T a () {
    return a_p;
  }

  /*!
    \brief Function value at x=0.

    \return b -- Function value at x=0.
  */
  T b () {
    return b_p;
  }

  /*!
    \brief \f$ \chi^2 \f$ of the fit.

    \return \f$ \chi^2 \f$ of the fit.
  */
  T chisq () {
    return chisq_p;
  }

  // --- Computation -----------------------------------------------------------
  
  /*!
    \brief Linear regression fit through a set of points \f$ {x,y} \f$

    \param x -- 
    \param y -- Function values \f$ y_i (x_i) \f$.
   */
  void fit (const Vector<T>& x,
	    const Vector<T>& y);

  // --- Function evaluation ---------------------------------------------------

  /*!
    \brief Function values at the input points.

    \return y -- Function values at the input points.
   */
  Vector<T> y () {
    return y_p;
  }

  /*!
    \brief Evaluate the linear function at a point.

    \param x -- 

    \return y -- Function value \f$ y (x;a,b) \f$.
   */
  T y (const T& x) {
    return a_p + b_p*x;
  };

  /*!
    \brief Evaluate the linear function at a number of points.

    \param x -- Set of points \f$ x_i \f$, where the function is to be
                evaluated.

    \return y -- Function values \f$ y_i (x_i;a,b) \f$.
   */
  Vector<T> y (const Vector<T>& x);

 private:

  void init ();

};

#endif
