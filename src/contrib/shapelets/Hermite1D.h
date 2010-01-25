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

#ifndef HERMITE1D_H
#define HERMITE1D_H

// Standard header files
#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include <math.h>
#include <vector>

static double pi = 3.141592653589793238462643383279502884197169;

/*!
  \class Hermite1D
  
  \ingroup contrib
  
  \brief A class to implement Hermite Polynomials.
  
  \author Lars B&auml;hren
  
  \date 2004/08
  
  \test tHermite1D.cc
  
  <h3>Prerequisites:</h3>

  <ul>
    <li>Wolfram Research, MathWorld, <a href="http://mathworld.wolfram.com/HermitePolynomial.html">Hermite Polynomial</a>
  </ul>
  
  <h3>Synopsis</h3>
  
  A Hermite polynomial of order \f$ l \f$ is given by
  \f[ H_l(x) = \sum_{2m+n=l} (-1)^m (2x)^n \frac{l!}{m!\,n!} \f]
  Important for computational purposes is the fact, that the such defined 
  polynomials obey a number of recursion relations:
  <ul>
    <li>Polynomial of next-higher order:
    \f[ H_{l+1}(x) = 2 x H_{l}(x) - 2 l H_{l-1}(x) \f]
    <li>Derivation:
    \f[ H_l'(x) = 2 l H_{l-1}(x)  \f]
  </ul>
  
  For the first few orders the Hermite polymials are:
  \f[
  \begin{array}{rclllll}
  H_{0} & = & +1  \\
  H_{1} & = &     & + 2 x \\
  H_{2} & = & -2  &       & + 4 x^{2} \\
  H_{3} & = &     & -12 x &            & 8 x^{3} \\
  H_{4} & = & +12 &       & - 48 x^{2} &         & 16 x^{4}
  \end{array}
  \f]
*/
class Hermite1D {
  
 protected:
  
  //! Order up to which the coefficients of the polynomial are cached
  int order_p;

 private:

  //! Polynomial coefficients
  std::vector<double> coefficients_p;
  
 public:
  
  // === Construction ===========================================================
  
  //! Argumented destructor
  Hermite1D (int const &order=10);
  
  //! Copy constructor
  Hermite1D (Hermite1D const &other);

  // === Destruction ============================================================

  //! Destructor
  virtual ~Hermite1D ();

  // === Operators ==============================================================

  //! Assignment/copy operator
  Hermite1D& operator= (Hermite1D const &other); 
  
  // === Parameter access =======================================================
  
  /*!
    \brief Get the maximum order of the Hermite polynomial.
    
    See note above on relation of polynomial order and number number of 
    pre-computed coefficients.
  */
  inline int order () const {
    return order_p;
  }
  
  //! Maximum order for which the polynomial coefficients are pre-computed.
  virtual void setOrder (int const &order);
  
  //! Get the number of polynomial coefficients
  inline unsigned int nofCoefficients () {
    return order_p+1;
  }
  
  //! Get the coefficients \f$ a_n \f$ of the polynomials
  inline std::vector<double> coefficients () const {
    return coefficients_p;
  };
  
  // === Methods ================================================================

  //! Provide a summary of the object's internal parameters and status
  void summary (std::ostream &os=std::cout);
  
  //! Evaluate the Hermite polynomial
  virtual double fx (int const &l,
		     double const &x);
  
  //! Evaluate the Hermite polynomial for a set of points.
  virtual std::vector<double> fx (const int l,
				  const std::vector<double>& x);
  
  //! First order derivative, \f$ \frac{d}{dx} H_l(x) \f$
  virtual double ddx (const int l,
		      const double x);
  
  //! Second order derivative, \f$ \frac{d^2}{dx^2} H_l(x) \f$
  virtual double d2dx2 (const int l,
			const double x);
  
 private:
  
  //! Unconditional copying
  void copy (Hermite1D const &other);
  //! Unconditional deletion
  void destroy ();
  
};

#endif
