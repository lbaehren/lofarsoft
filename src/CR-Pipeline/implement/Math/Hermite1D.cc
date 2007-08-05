/*-------------------------------------------------------------------------*
 | $Id:: IO.h 393 2007-06-13 10:49:08Z baehren                           $ |
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

#include <Math/Hermite1D.h>

namespace CR {  // Namespace CR -- begin
  
  // =============================================================================
  //
  //  Construction / Deconstruction
  //
  // =============================================================================
  
  Hermite1D::Hermite1D () 
  {
    int order (10);
    Hermite1D::setOrder (order);
  }
  
  Hermite1D::Hermite1D (const int order) {
    Hermite1D::setOrder (order);
  }
  
  Hermite1D::~Hermite1D () {
    delete[] coefficients_p;
  }
  
  // =============================================================================
  //
  //  Access to the polynomial parameters
  //
  // =============================================================================
  
  void Hermite1D::setOrder (int order)
  {
    // store polynomial order
    order_p = order;
    // store the number of polynomial coefficients
    nofCoeff_p = order+1;
    // compute the polynomial coefficients
    Hermite1D::calcCoefficients ();
  }
  
  // =============================================================================
  // 
  //  Computation of the polynomial coefficients
  // 
  // =============================================================================
  
  void Hermite1D::calcCoefficients () {
    
    int order2 = nofCoeff_p*nofCoeff_p;
    int n (0);
    int k (0);
    
    // Allocate memory for the coefficients of the Hermite polynomials and
    // initialize the array elements
    
    coefficients_p = new double[order2];
    for (n=0; n<order2; n++) {
      coefficients_p[n] = 0.0;
    }
    
    // Set the values of the fundamental coefficients on which the recursion
    // relation is build
    
    coefficients_p[0] = 1.0;             // h_{0,0}
    coefficients_p[1] = 0.0;             // h_{1,0}
    coefficients_p[nofCoeff_p+1] = 2.0;  // h_{1,1}
    
    // Compute all the higher order coefficients using the recursion relation;
    // start with n=2, since the recursion relation required values for n-1 and
    // n-2 to be known in advance
    
    for (n=2; n<nofCoeff_p; n++) {
      for (k=0; k<=n; k++) {
	//starts with k = 0 until k = n since k > n terms = 0
	if (k == 0) {           
	  //if k = 0 then do not calculate (k-1)th term
	  coefficients_p[n*nofCoeff_p+k] = -2*(n-1)*coefficients_p[(n-2)*nofCoeff_p+k];
	} else if (k==n) {
	  //if k = n then do not calculate (n-2) term since (n-2) > k
	  coefficients_p[n*nofCoeff_p+k] = 2*coefficients_p[(n-1)*nofCoeff_p+k-1];
	} else {
	  coefficients_p[n*nofCoeff_p+k] = 2*coefficients_p[(n-1)*nofCoeff_p+k-1] - 2*(n-1)*coefficients_p[(n-2)*nofCoeff_p+k];
	}
      }
    }
    
  }
  
  // =============================================================================
  // 
  //  Evaluation of the polynomial.
  // 
  // =============================================================================
  
  double Hermite1D::eval (const int l,
			  const double x) {
    
    double y;
    
    /* Check if we have already computed the polynomial coefficients up to the
       order requested; if this is not the case, compute them and adjust the
       internal parameters. */
    if (l > order_p) {
      cerr << "[Hermite1D::eval] Adjusting internal coefficient cache "
	   << order_p << " -> " << l << " ..." << endl;
      Hermite1D::setOrder (l);
    }
    
    if (l<0) {
      y = 0.0;
    } else if (l==0) {
      y = 1.0;
    } else {
      y = coefficients_p[l*nofCoeff_p+l];
      for (int i=l-1; i>=0; i--) {
	y = y*x + coefficients_p[l*nofCoeff_p+i];
      }
    }
    
    return y;  
  }
  
  vector<double> Hermite1D::eval (const int l,
				  const vector<double>& x)
  {
    int nelem;
    vector<double> y;
    
    nelem = x.size();
    y.resize(nelem,0.0);
    
    for (int n=0; n<nelem; n++) {
      y[n] = Hermite1D::eval (l,x[n]);
    }
    
    return y;
  }
  
  // ============================================================================
  // 
  //  Derivative w.r.t. function argument
  // 
  // ============================================================================

  // ------------------------------------------------------------------------ ddx
  
  double Hermite1D::ddx (const int l,
			 const double x)
  {
    double y (0.0);
    
    if (l > 0) {
      y = 2*l*Hermite1D::eval(l-1,x);
    }
    
    return y;
  }

  // ---------------------------------------------------------------------- d2dx2
  
  double Hermite1D::d2dx2 (const int l,
			   const double x)
  {
    double y (0.0);
    
    if (l > 0) {
      y = 2*l*(Hermite1D::eval(l-1,x)-Hermite1D::eval(l,x));
    }
    
    return y;
  }
  
}  // Namespace CR -- end
