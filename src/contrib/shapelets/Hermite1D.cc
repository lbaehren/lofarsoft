/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2007                                                    *
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

#include "Hermite1D.h"

// ==============================================================================
//
//  Construction
//
// ==============================================================================

//_______________________________________________________________________________
//                                                                      Hermite1D

/*!
  \param order - Maximum order for which the polynomial coefficients are
         pre-computed.
  
  \todo Enable handling of polynomial of 0-th order or throw warning in case
  such a request is detected.
*/
Hermite1D::Hermite1D (int const &order)
{
  setOrder (order);
}

//_______________________________________________________________________________
//                                                                      Hermite1D

Hermite1D::Hermite1D (Hermite1D const &other)
{
  copy (other);
}

// ==============================================================================
//
//  Destruction
//
// ==============================================================================

//_______________________________________________________________________________
//                                                                     ~Hermite1D

Hermite1D::~Hermite1D ()
{
  destroy();
}

//_______________________________________________________________________________
//                                                                        destroy

void Hermite1D::destroy ()
{;}

// ==============================================================================
//
//  Operators
//
// ==============================================================================

//_______________________________________________________________________________
//                                                                      operator=

Hermite1D& Hermite1D::operator= (Hermite1D const &other)
{
  if (this != &other) {
    destroy ();
    copy (other);
  }
  return *this;
}

//_______________________________________________________________________________
//                                                                           copy

void Hermite1D::copy (Hermite1D const &other)
{
  order_p = other.order_p;

  coefficients_p.resize(order_p);
  coefficients_p = other.coefficients_p;
}

// =============================================================================
//
//  Parameter access
//
// =============================================================================

/*!
  Set the maximum order, for which the polynomial coefficients are
  pre-computed. The internal stored value will be larger by one, than the
  maximum order of the polynomial; for this keep in mind that for a polynomial
  of order \f$ n \f$ we have coefficients \f$ a_0 , a_1 , ... , a_n \f$.
  
  \param order - Maximum order for which the polynomial coefficients are
         pre-computed.
*/
void Hermite1D::setOrder (int const &order)
{
  /* [1] store polynomial order */
  order_p = order;

  /* [2] compute the polynomial coefficients */
  int nofCoeff = nofCoefficients();
  int nelem    = nofCoeff*nofCoeff;
  int n        = 0;
  int k        = 0;

  // Resize array storing polynomial coefficients
  if (coefficients_p. size() != nelem) {
    coefficients_p.resize(nelem);
    for (n=0; n<nelem; n++) {
      coefficients_p[n] = 0.0;
    }
  }
  
  // Set the values of the fundamental coefficients on which the recursion
  // relation is build
  
  coefficients_p[0]          = 1.0;  // h_{0,0}
  coefficients_p[1]          = 0.0;  // h_{1,0}
  coefficients_p[nofCoeff+1] = 2.0;  // h_{1,1}
  
  // Compute all the higher order coefficients using the recursion relation;
  // start with n=2, since the recursion relation required values for n-1 and
  // n-2 to be known in advance
  
  for (n=2; n<nofCoeff; n++) {
    for (k=0; k<=n; k++) {
      //starts with k = 0 until k = n since k > n terms = 0
      if (k == 0) {           
	//if k = 0 then do not calculate (k-1)th term
	coefficients_p[n*nofCoeff+k] = -2*(n-1)*coefficients_p[(n-2)*nofCoeff+k];
      } else if (k==n) {
	//if k = n then do not calculate (n-2) term since (n-2) > k
	coefficients_p[n*nofCoeff+k] = 2*coefficients_p[(n-1)*nofCoeff+k-1];
      } else {
	coefficients_p[n*nofCoeff+k] = 2*coefficients_p[(n-1)*nofCoeff+k-1] - 2*(n-1)*coefficients_p[(n-2)*nofCoeff+k];
      }
    }
  }
}

// ==============================================================================
// 
//  Methods
// 
// ==============================================================================

//_______________________________________________________________________________
//                                                                        summary

void Hermite1D::summary (std::ostream &os)
{
  os << "[Hermite1D] Summary of internal paramaters." << std::endl;
  os << "-- Polynomial order  = " << order_p               << std::endl;
  os << "-- nof. coefficients = " << coefficients_p.size() << std::endl;
}

//_______________________________________________________________________________
//                                                                             fx

/*!
  Hermite polynom of order l, using polynom procedure from Numerical Recipes.
  If the requested polynomial order is higher than the currently cached 
  polynomial coefficients, the internal cache first will be updated, since 
  multiple evaluations for this order might have to be done.
  
  \param l - order for the polynomial
  \param x - value for which the polynomial is evaluated
  
  \return y - Value of the Hermite polynomial \f$ H_l (x) \f$.
*/
double Hermite1D::fx (int const &l,
		      double const &x)
{  
  double y (0);
  unsigned int nofCoeff = nofCoefficients();
  
  /* Check if we have already computed the polynomial coefficients up to the
     order requested; if this is not the case, compute them and adjust the
     internal parameters. */
  if (l > order_p) {
    std::cerr << "[Hermite1D::fx] Adjusting internal coefficient cache "
	      << order_p << " -> " << l << " ..." << std::endl;
    Hermite1D::setOrder (l);
  }
  
  if (l<0) {
    y = 0.0;
  } else if (l==0) {
    y = 1.0;
  } else {
    y = coefficients_p[l*nofCoeff+l];
    for (int i=l-1; i>=0; i--) {
      y = y*x + coefficients_p[l*nofCoeff+i];
    }
  }
  
  return y;  
}

//_______________________________________________________________________________
//                                                                             fx

/*!
  \param l - Order for the Hermite polynomial.
  \param x - Vector of values, \f$x\f$, for which the polynomial is evaluated.
  
  \return y - Values of the Hermite polynomial \f$ H_l (x) \f$.
*/
std::vector<double> Hermite1D::fx (const int l,
				     const std::vector<double>& x)
{
  int nelem (x.size());
  std::vector<double> y (nelem);
  
  for (int n=0; n<nelem; n++) {
    y[n] = Hermite1D::fx (l,x[n]);
  }
  
  return y;
}

//_______________________________________________________________________________
//                                                                            ddx

/*!
  The first order derivative of a Hermite polynomial of order \f$ l \f$,
  \f$ H_{l}(x) \f$, is given by
  \f[ H_{l}'(x) = 2 l H_{l-1}(x) \f]
  
  \param l - order for the polynomial
  \param x - value for which the polynomial is evaluated
  
  \return y - First order derivative of the Hermite polynomial
          \f$ H_l (x) \f$.
*/
double Hermite1D::ddx (const int l,
		       const double x)
{
  double y (0.0);
  
  if (l > 0) {
    y = 2*l*Hermite1D::fx(l-1,x);
  }
  
  return y;
}

//_______________________________________________________________________________
//                                                                          d2dx2

/*!
  The second order derivative of a Hermite polynomial of order \f$ l \f$,
  \f$ H_{l}(x) \f$, is given by
  \f[ H_{l}''(x) = 2 l H_{l}'(x) - 2 l H_{l}(x) = 2 l \left( H_{l-1}(x)
  - H_{l}(x) \right) \f]
  
  \param l - order for the polynomial
  \param x - value for which the polynomial is evaluated
  
  \return y - Second order derivative of the Hermite polynomial \f$ H_l (x) \f$.
*/
double Hermite1D::d2dx2 (const int l,
			 const double x)
{
  double y (0.0);
  
  if (l > 0) {
    y = 2*l*(Hermite1D::fx(l-1,x)-Hermite1D::fx(l,x));
  }
  
  return y;
}
