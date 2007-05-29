/***************************************************************************
 *   Copyright (C) 2004-2005                                               *
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

// Standard header files
#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include <math.h>
using namespace std;

// Custom header files
#include <Math/Constants.h>
#include <Math/Shapelets1D.h>

namespace CR {  // Namespace CR -- begin
  
  // ============================================================================
  //
  //  Construction / Deconstruction
  //
  // ============================================================================

  // ---------------------------------------------------------------- Shapelets1D
  
  Shapelets1D::Shapelets1D ()
    : Hermite1D ()
  {
    Shapelets1D::setBeta ();
  }
  
  // ---------------------------------------------------------------- Shapelets1D
  
  Shapelets1D::Shapelets1D (int const &order)
    : Hermite1D (order)
  {
    Shapelets1D::setBeta ();
  }
  
  // ---------------------------------------------------------------- Shapelets1D
  
  Shapelets1D::Shapelets1D (int const &order,
			    double const &beta)
    : Hermite1D (order)
  {
    Shapelets1D::setBeta (beta);
  }
  
  // --------------------------------------------------------------- ~Shapelets1D
  
  Shapelets1D::~Shapelets1D ()
  {
    coefficients_p.clear();
    integral_p.clear();
  }
  
  // =============================================================================
  //
  // Access to member data
  //
  // =============================================================================
  
  void Shapelets1D::initShapelets1D ()
  {
    // store numbers for book-keeping
    order_p    = Hermite1D::order ();
    nofCoeff_p = Hermite1D::order () + 1;
    // compute the normalization coefficients
    Shapelets1D::calcCoefficients ();
    // compute the integral of the n-th order Shapelet
    Shapelets1D::setIntegral ();
  }
  
  // ============================================================================
  //
  //  Function parameter values
  //
  // ============================================================================
  
  void Shapelets1D::setOrder (const int order) 
  {
    // base class
    Hermite1D::setOrder (order);
    // compute cached values
    Shapelets1D::initShapelets1D ();
  }
  
  
  void Shapelets1D::setBeta ()
  {
    double beta = 1.0;
    Shapelets1D::setBeta (beta);
  }
  
  void Shapelets1D::setBeta (const double beta)
  {
    beta_p = beta;
    Shapelets1D::initShapelets1D ();
  }
  
  double Shapelets1D::beta () {
    return beta_p;
  }
  
  // =============================================================================
  //
  //  Function evaluation.
  //
  // =============================================================================
  
  void Shapelets1D::calcCoefficients ()
  {
    double sqrt_pi = sqrt(pi);
    double pw;
    
    /* allocate memory for the normalization coefficients ... */
    coefficients_p.resize(nofCoeff_p,0.0);
    
    /* ... and compute their values */
    coefficients_p[0] = 1.0/sqrt(sqrt_pi);
    for (int i=1; i<nofCoeff_p; i++) {
      pw = 1.0*i;
      coefficients_p[i] = 1.0/sqrt(pow(2.0,pw)*sqrt_pi*Shapelets1D::factrl(i));
    }
  }
  
  double Shapelets1D::eval (int l,
			    double x)
  {
    // Value of the Hermite polynomial of order n
    double hermite (.0);
    // Value of the Shapelet of order n (return value)
    double phi (.0);
    // Argument for the exponential function
    double xx (.0);
    
    xx = -(x*x)/(2*beta_p*beta_p);
    
    if (l > order_p) {
      cerr << "[Shapelets1D::eval] Adjusting internal coefficient cache "
	   << order_p << " -> " << l << " ..." << endl;
      Shapelets1D::setOrder (l);
    }
    
    if (l<0) {  // case possible for derivative of 0th order function
      phi = 0.0;
    } else if (l==0) {
      phi = coefficients_p[0]*exp(xx);
    } else {
      // Shapelet basis function
      hermite = Hermite1D::eval(l,x/beta_p);
      phi = coefficients_p[l]*hermite*exp(xx);
    }
    
    return phi;
  }
  
  vector<double> Shapelets1D::eval (int l,
				    const vector<double>& x)
  {
    unsigned int nofValues = x.size();
    vector<double> y (nofValues);
    
    for (unsigned i=0; i<nofValues; i++) {
      y[i] = Shapelets1D::eval (l,x[i]);
    }
    
    return y;
  }
  
  // =============================================================================
  //
  //  Global function properties
  //
  // =============================================================================
  
  void Shapelets1D::setIntegral ()
  {
    double pw;
    
    integral_p.resize(nofCoeff_p,0.0);
    
    for (int n=0; n<nofCoeff_p; n++) {
      // n is even
      if ((n/2)*2 == n) {
	pw = 1.0*(1-n);
	integral_p[n] = sqrt(pow(2.0,pw)*sqrt(pi)*beta_p*Shapelets1D::bico(n,n/2));
      }
    }
  }
  
  double Shapelets1D::integral (int const &order)
  {
    // is the requested value in cache?
    if (order > order_p) {
      Shapelets1D::setOrder (order);
    }
    
    return integral_p[order];
  }
  
  double Shapelets1D::rmsRadius (int const &order) {
    
    double rms(0.0);
    
    rms = beta_p*sqrt(order+0.5);
    
    return rms;
  }
  
  double Shapelets1D::resolution (const int n) {
    
    double resolution (0.0);
    
    resolution = beta_p/(sqrt(n+0.5));
    
    return resolution;
  }
  
  
  // ============================================================================
  //
  //  Helper functions
  //
  // ============================================================================
  
  double Shapelets1D::gammln (double xx)
  {
    double x (0.0);
    double y (0.0);
    double tmp (0.0);
    double ser (0.0);
    static double cof[6]={76.18009172947146,-86.50532032941677,
			  24.01409824083091,-1.231739572450155,
			  0.1208650973866179e-2,-0.5395239384953e-5};
    int j;
    
    y=x=xx;
    tmp=x+5.5;
    tmp -= (x+0.5)*log(tmp);
    ser=1.000000000190015;
    
    for (j=0;j<=5;j++) {
      ser += cof[j]/++y;
    }
    
    return -tmp+log(2.5066282746310005*ser/x);
  }
  
  double Shapelets1D::factln (int n)
  {
    static double a[101];
    
    if (n < 0) {
      cerr << "[Shapelets1D::factrl] Negative factorial" << endl;
      return 0.0;
    }
    else if (n <= 1) {
      return 0.0;
    }
    else if (n <= 100) {
      return a[n] ? a[n] : (a[n]=Shapelets1D::gammln(n+1.0));
    }
    else {
      return Shapelets1D::gammln(n+1.0);
    }
  }
  
  double Shapelets1D::factrl (int n) {
    
    static int ntop (4);
    static double a[33]={1.0,1.0,2.0,6.0,24.0};
    int j (0);
    
    if (n < 0) cerr << "[Shapelets1D::factrl] Negative factorial" << endl;
    if (n > 32) return exp(Shapelets1D::gammln(n+1.0));
    while (ntop<n) {
      j=ntop++;
      a[ntop]=a[j]*ntop;
    }
    return a[n];
  }
  
  double Shapelets1D::bico (int n, int k)
  {
    return floor(0.5+exp(Shapelets1D::factln(n)-Shapelets1D::factln(k)-Shapelets1D::factln(n-k)));
  }
  
}  // Namespace CR -- end
