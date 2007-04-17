
#ifndef HERMITE1D_H
#define HERMITE1D_H

/* $Id: Hermite1D.h,v 1.6 2006/06/26 15:57:56 bahren Exp $ */

// Standard header files
#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include <math.h>
#include <vector>
using namespace std;

/*!
  \class Hermite1D

  \ingroup Math
  \ingroup Shapelets

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

*/

class Hermite1D {

  int order_p;             // Order of the Hermite Polynomial
  int nofCoeff_p;          // Number of polynomial coefficients
  double *coefficients_p;  // Polynomial coefficients

 public:
 
 // === Construction / Destruction =============================================

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
  Hermite1D (const int order);

  //! Destructor
  virtual ~Hermite1D ();
  
  // === Access to the polynomial parameters ===================================

  /*!
    \brief Get the maximum order of the Hermite polynomial.
  
    See note above on relation of polynomial order and number number of 
    pre-computed coefficients.
  */
  int order ();
  
  /*!
    \brief Maximum order for which the polynomial coefficients are pre-computed.

    Set the maximum order, for which the polynomial coefficients are
    pre-computed. The internal stored value will be larger by one, than the
    maximum order of the polynomial; for this keep in mind that for a polynomial
    of order \f$ n \f$ we have coefficients \f$ a_0 , a_1 , ... , a_n \f$.

    \param order - Maximum order for which the polynomial coefficients are
                   pre-computed.
  */
  virtual void setOrder (const int order);

  // === Evaluation of the polynomial ==========================================

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
  double eval (const int l,
	       const double x);
  
  /*!
    \brief Evaluate the Hermite polynomial for a set of points.

    \param l - Order for the Hermite polynomial.
    \param x - Vector of values, \f$x\f$, for which the polynomial is evaluated.

    \return y - Values of the Hermite polynomial \f$ H_l (x) \f$.
  */
  vector<double> eval (const int l,
		       const vector<double>& x);

  // === Derivatives ===========================================================

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
  double ddx (const int l,
	      const double x);
  
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
  double d2dx2 (const int l,
		const double x);
  
 private:
  
  void calcCoefficients ();

};

#endif
