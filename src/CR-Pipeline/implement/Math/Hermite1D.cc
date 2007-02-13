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

#include <iostream>
#include <Math/Hermite1D.h>

/*!
  \class Hermite1D
*/

namespace CR {  // Namespace CR -- BEGIN
  
  // =============================================================================
  //
  //  Construction
  //
  // =============================================================================
  
  Hermite1D::Hermite1D () 
  {
    int order = 10;
    Hermite1D::setOrder (order);
  }
  
  Hermite1D::Hermite1D (int const &order) {
    Hermite1D::setOrder (order);
  }
  
  Hermite1D::Hermite1D (Hermite1D const& other)
  {
    copy (other);
  }
  
  // =============================================================================
  //
  //  Deconstruction
  //
  // =============================================================================
  
  Hermite1D::~Hermite1D ()
  {
    destroy();
  }
  
  // ==============================================================================
  //
  //  Operators
  //
  // ==============================================================================
  
  Hermite1D &Hermite1D::operator= (Hermite1D const &other)
  {
    if (this != &other) {
      destroy ();
      copy (other);
    }
    return *this;
  }
  
  // ------------------------------------------------------------------------- copy
  
  void Hermite1D::copy (Hermite1D const &other)
  {
    order_p    = other.order_p;
    nofCoeff_p = other.nofCoeff_p;
    //
    int order2 = nofCoeff_p*nofCoeff_p;
    coefficients_p = new double[order2];
    for (int i=0; i<order2; i++) {
      coefficients_p[i] = other.coefficients_p[i];
    }
  }
  
  // ---------------------------------------------------------------------- destroy
  
  void Hermite1D::destroy ()
  {
    delete[] coefficients_p;
  }
  
  // =============================================================================
  //
  //  Access to the polynomial parameters
  //
  // =============================================================================
  
  void Hermite1D::setOrder (int const &order)
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
  
  void Hermite1D::calcCoefficients ()
  {  
    int order2 = nofCoeff_p*nofCoeff_p;
    
    // Allocate memory for the coefficients of the Hermite polynomials and
    // initialize the array elements
    
    coefficients_p = new double[order2];
    for (int i=0; i<order2; i++) {
      coefficients_p[i] = 0.0;
    }
    
    // Set the values of the fundamental coefficients on which the recursion
    // relation is build
    
    coefficients_p[0] = 1.0;             // h_{0,0}
    coefficients_p[1] = 0.0;             // h_{1,0}
    coefficients_p[nofCoeff_p+1] = 2.0;  // h_{1,1}
    
    // Compute all the higher order coefficients using the recursion relation;
    // start with n=2, since the recursion relation required values for n-1 and
    // n-2 to be known in advance
    
    for (int n=2; n<nofCoeff_p; n++) {
      for (int k=0; k<=n; k++) {
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
  
  // -------------------------------------------------------------------------- fx
  
  double Hermite1D::fx (int const &l,
			double const &x)
  {  
    double y (0.0);
    
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
      y = coefficients_p[l*nofCoeff_p+l];
      for (int i=l-1; i>=0; i--) {
	y = y*x + coefficients_p[l*nofCoeff_p+i];
      }
    }
    
    return y;  
  }
  
  // -------------------------------------------------------------------------- fx
  
  Array<double,1> Hermite1D::fx (int const &l,
				 Array<double,1> const &x)
  {
    int nelem (x.numElements());
    Array<double,1> y (nelem);
    
    for (int n=0; n<nelem; n++) {
      y(n) = fx (l,x(n));
    }
    
    return y;
  }
  
  // -------------------------------------------------------------------------- fx
  
  Array<double,1> Hermite1D::fx (Array<int,1> const &l,
				     Array<double,1> const &x)
  {
    int nelem (0);
    Array<double,1> y;
    
    // check if the vector sizes match
    if (l.size() != x.size()) {
      std::cout << "[Hermite1D::fx] Mismatching array sizes!" << std::endl;
      y.resize(1,0.0);
    } else {
      nelem = x.size();
      y.resize(nelem,0.0);
      
      for (int n(0); n<nelem; n++) {
	y[n] = fx (l[n],x[n]);
      }
    }
    
    return y;
  }
  
  // ------------------------------------------------------------------------ dfdx
  
  double Hermite1D::dfdx (int const &l,
			  double const &x)
  {
    double y (0.0);
    
    if (l > 0) {
      y = 2*l*fx(l-1,x);
    }
    
    return y;
  }
  
  // ------------------------------------------------------------------------ dfdx
  
  Array<double,1> Hermite1D::dfdx (int const &l,
				       Array<double,1> const &x)
  {
    int nelem (x.numElements());
    Array<double,1> y (nelem);
    
    for (int n=0; n<nelem; n++) {
      y(n) = dfdx (l,x(n));
    }
    
    return y;
  }
  
  // ------------------------------------------------------------------------ dfdx
  
  Array<double,1> Hermite1D::dfdx (Array<int,1> const &l,
				       Array<double,1> const &x)
  {
    int nelem (0);
    Array<double,1> y;
    
    // check if the vector sizes match
    if (l.size() != x.size()) {
      std::cout << "[Hermite1D::dfdx] Mismatching array sizes!" << std::endl;
      y.resize(1,0.0);
    } else {
      nelem = x.size();
      y.resize(nelem,0.0);
      
      for (int n(0); n<nelem; n++) {
	y(n) = dfdx (l(n),x(n));
      }
    }
    
    return y;
  }
  
  // ---------------------------------------------------------------------- d2fdx2
  
  double Hermite1D::d2fdx2 (int const &l,
			    double const &x)
  {
    double y (0.0);
    
    if (l > 0) {
      y = 2*l*(Hermite1D::fx(l-1,x)-Hermite1D::fx(l,x));
    }
    
    return y;
  }
  
  
  // ---------------------------------------------------------------------- d2fdx2
  
  Array<double,1> Hermite1D::d2fdx2 (int const &l,
					 Array<double,1> const &x)
  {
    int nelem;
    Array<double,1> y;
    
    nelem = x.size();
    y.resize(nelem,0.0);
    
    for (int n=0; n<nelem; n++) {
      y(n) = d2fdx2 (l,x(n));
    }
    
    return y;
  }
  
  // ---------------------------------------------------------------------- d2fdx2
  
  Array<double,1> Hermite1D::d2fdx2 (Array<int,1> const &l,
				     Array<double,1> const &x)
  {
    int nelem (0);
    Array<double,1> y;
    
    // check if the vector sizes match
    if (l.size() != x.size()) {
      std::cout << "[Hermite1D::d2fdx2] Mismatching array sizes!" << std::endl;
      y.resize(1,0.0);
    } else {
      nelem = x.size();
      y.resize(nelem,0.0);
      
      for (int n(0); n<nelem; n++) {
	y(n) = d2fdx2 (l(n),x(n));
      }
    }
    
    return y;
  }
  
}  // Namespace CR -- END
