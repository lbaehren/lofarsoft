/***************************************************************************
 *   Copyright (C) 2005-2006                                               *
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

/* $Id: Hermite1D.h,v 1.4 2005/11/08 09:50:14 bahren Exp $ */

// Standard header files
#include <stdlib.h>
#include <stdio.h>
#include <cmath>

// additional packages
#include <blitz/array.h>

using blitz::Array;

namespace CR {  // Namespace CR -- BEGIN
  
  /*!
    \class Hermite1D
    
    \ingroup Math
    
    \brief A class to implement Hermite Polynomials.
    
    \author Lars B&auml;hren
    
    \date 2004/08
    
    \test tHermite1D.cc
    
    <h3>Prerequisites:</h3>
    <ul>
      <li>Wolfram Research, MathWorld, <i>Hermite Polynomial</i>
      [http://mathworld.wolfram.com/HermitePolynomial.html]
    </ul>
    
    <h3>Synopsis</h3>
    
    A Hermite polynomial of order \f$ l \f$ is given by
    \f[ H_l(x) = \sum_{2m+n=l} (-1)^m (2x)^n \frac{l!}{m!\,n!} \f]
    Important for computational purposes is the fact, that the such defined 
    polynomials obey a number of recursion relations:
    \f[ H_{l+1}(x) = 2 x H_{l}(x) - 2 l H_{l-1}(x) \f]
    \f[ H_l'(x) = 2 l H_{l-1}(x)  \f]
    
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
    
    <img src="../docs/figures/Hermite1D.png">
  */
  
  class Hermite1D {
    
    //! Order of the Hermite Polynomial
    int order_p;
    //! Number of polynomial coefficients
    int nofCoeff_p;
    //! Polynomial coefficients
    double *coefficients_p;
    
  public:
    
    // === Construction / Destruction ===========================================
    
    /*!
      \brief Empty constructor
      
      By default internals will be set up for polynomials up to order 10.
    */
    Hermite1D ();
    
    /*!
      \brief Argumented constructor.
      
      \param order - Maximum order for which the polynomial coefficients are
      pre-computed.
      
      \todo Enable handling of polynomial of 0-th order or throw warning in case
      such a request is detected.
    */
    Hermite1D (int const &order);
    
    /*!
      \brief Copy constructor
      
      \param other -- Another Hermite1D object from which to create this new
      one.
    */
    Hermite1D (Hermite1D const &other);
    
    //! Destructor
    virtual ~Hermite1D ();
    
    // ---------------------------------------------------------------- Operators
    
    /*!
      \brief Overloading of the copy operator
      
      \param other -- Another Hermite1D object from which to make a copy.
    */
    Hermite1D &operator= (Hermite1D const &other); 
    
    // === Access to the polynomial parameters ==================================
    
    /*!
      \brief Get the maximum order of the Hermite polynomial.
      
      See note above on relation of polynomial order and number number of 
      pre-computed coefficients.
    */
    int order () const {
      return order_p;
    }
    
    /*!
      \brief Maximum order for which the polynomial coefficients are pre-computed.
      
      Set the maximum order, for which the polynomial coefficients are
      pre-computed. The internal stored value will be larger by one, than the
      maximum order of the polynomial; for this keep in mind that for a polynomial
      of order \f$ n \f$ we have coefficients \f$ a_0 , a_1 , ... , a_n \f$.
      
      \param order - Maximum order for which the polynomial coefficients are
                     pre-computed.
    */
    virtual void setOrder (int const &order);
    
    // === Evaluation of the polynomial =========================================
    
    /*!
      \brief Evaluate the Hermite polynomial
      
      Hermite polynom of order l, using polynom procedure from Numerical Recipes.
      If the requested polynomial order is higher than the currently cached 
      polynomial coefficients, the internal cache first will be updated, since 
      multiple evaluations for this order might have to be done.
      
      \param l - order for the polynomial
      \param x - value for which the polynomial is evaluated
      
      \return y - Value of the Hermite polynomial \f$ H_l (x) \f$.
    */
    virtual double fx (int const &l,
		       double const &x);
    
    /*!
      \brief Evaluate the Hermite polynomial for a set of points.
      
      \param l - Order for the Hermite polynomial.
      \param x - Vector of values, \f$x\f$, for which the polynomial is evaluated.
      
      \return y - Values of the Hermite polynomial \f$ H_l (x) \f$.
    */
    virtual Array<double,1> fx (int const &l,
				Array<double,1> const &x);
    
    /*!
      \brief Evaluate the Hermite polynomial for a set of points.
      
      \param l - Order for the Hermite polynomial.
      \param x - Vector of values, \f$x\f$, for which the polynomial is evaluated.
      
      \return y - Values of the Hermite polynomial \f$ H_l (x) \f$.
    */
    virtual Array<double,1> fx (Array<int,1> const &l,
				Array<double,1> const &x);
    
    // === Derivatives ==========================================================
    
    /*!
      \brief First order derivative w.r.t. \f$ x \f$.
      
      The first order derivative of a Hermite polynomial of order \f$ l \f$,
      \f$ H_{l}(x) \f$, is given by
      \f[ H_{l}'(x) = 2 l H_{l-1}(x) \f]
      
      \param l - order for the polynomial
      \param x - value for which the polynomial is evaluated
      
      \return y - First order derivative of the Hermite polynomial
      \f$ H_l (x) \f$.
    */
    virtual double dfdx (int const &l,
			 double const &x);
    
    /*!
      \brief First order derivative w.r.t. \f$ x \f$.
      
      The first order derivative of a Hermite polynomial of order \f$ l \f$,
      \f$ H_{l}(x) \f$, is given by
      \f[ H_{l}'(x) = 2 l H_{l-1}(x) \f]
      
      \param l - order for the polynomial
      \param x - value for which the polynomial is evaluated
      
      \return y - First order derivative of the Hermite polynomial
      \f$ H_l (x) \f$.
    */
    virtual Array<double,1> dfdx (int const &l,
				  Array<double,1> const &x);
    
    /*!
      \brief First order derivative w.r.t. \f$ x \f$.
      
      The first order derivative of a Hermite polynomial of order \f$ l \f$,
      \f$ H_{l}(x) \f$, is given by
      \f[ H_{l}'(x) = 2 l H_{l-1}(x) \f]
      
      \param l - order for the polynomial
      \param x - value for which the polynomial is evaluated
      
      \return y - First order derivative of the Hermite polynomial
      \f$ H_l (x) \f$.
    */
    virtual Array<double,1> dfdx (Array<int,1> const &l,
				  Array<double,1> const &x);
    
    /*!
      \brief Second order derivative w.r.t. \f$ x \f$.
      
      The second order derivative of a Hermite polynomial of order \f$ l \f$,
      \f$ H_{l}(x) \f$, is given by
      \f[ H_{l}''(x) = 2 l H_{l}'(x) - 2 l H_{l}(x) = 2 l \left( H_{l-1}(x)
      - H_{l}(x) \right) \f]
      
      \param l - order for the polynomial
      \param x - value for which the polynomial is evaluated
      
      \return y - Second order derivative of the Hermite polynomial \f$ H_l (x) \f$.
    */
    virtual double d2fdx2 (int const &l,
			   double const &x);
    
    /*!
      \brief Second order derivative w.r.t. \f$ x \f$.
      
      \param l - order for the polynomial
      \param x - value for which the polynomial is evaluated
      
      \return y - Second order derivative of the Hermite polynomial \f$ H_l (x) \f$.
    */
    virtual Array<double,1> d2fdx2 (int const &l,
				    Array<double,1> const &x);
    
    /*!
      \brief Second order derivative w.r.t. \f$ x \f$.
      
      \param l - order for the polynomial
      \param x - value for which the polynomial is evaluated
      
      \return y - Second order derivative of the Hermite polynomial \f$ H_l (x) \f$.
    */
    virtual Array<double,1> d2fdx2 (Array<int,1> const &l,
				    Array<double,1> const &x);
    
  private:
    
    void calcCoefficients ();
    
    /*!
      \brief Unconditional copying
    */
    void copy (Hermite1D const& other);
    
    /*!
      \brief Unconditional deletion 
    */
    void destroy(void);
    
  };
  
}  // Namespace CR -- END

#endif
