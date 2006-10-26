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

/* $Id: ShapeletsModel1D.cc,v 1.1 2005/07/15 10:10:37 bahren Exp $*/

#include <Functionals/ShapeletsModel1D.h>

#define PI 3.14159265358979323846

// =============================================================================
//
//  Construction / Destruction
//
// =============================================================================

ShapeletsModel1D::ShapeletsModel1D ()
  : Shapelets1D ()
{
  ShapeletsModel1D::initModel ();
}

ShapeletsModel1D::ShapeletsModel1D (const int order,
				    const double beta) 
  : Shapelets1D (order,beta)
{
  ShapeletsModel1D::initModel ();
}

ShapeletsModel1D::ShapeletsModel1D (const double center,
				    const vector<double>& coefficients) 
  : Shapelets1D ()
{
  ShapeletsModel1D::setCenter (center);
  ShapeletsModel1D::setCoefficients (coefficients);
}

ShapeletsModel1D::ShapeletsModel1D (const int order,
				    const double beta,
				    const double center,
				    const vector<double>& coefficients) 
  : Shapelets1D (order,beta)
{
  // Model parameters
  ShapeletsModel1D::setCenter (center);
  ShapeletsModel1D::setCoefficients (coefficients);
}

ShapeletsModel1D::~ShapeletsModel1D ()
{
  coefficients_p.clear();
}

// =============================================================================
//
//  Initialize model parameters
//
// =============================================================================

void ShapeletsModel1D::initModel () 
{
  int nofCoeff;
  double center (0.0);
  vector<double> coefficients;
  
  nofCoeff = Hermite1D::order() + 1;
  coefficients.resize(nofCoeff,1.0);

  ShapeletsModel1D::setCenter (center);
  ShapeletsModel1D::setCoefficients (coefficients);
}

// =============================================================================
//
//  Access to the model parameters
//
// =============================================================================

void ShapeletsModel1D::setOrder (const int order)
{
  Shapelets1D::setOrder (order);
  ShapeletsModel1D::updateGlobals ();
}

void ShapeletsModel1D::setBeta (const double beta) 
{
  Shapelets1D::setBeta (beta);
  ShapeletsModel1D::updateGlobals ();
}

void ShapeletsModel1D::setCenter (const double& center)
{
  center_p = center;
}

void ShapeletsModel1D::setCoefficients (const vector<double>& coefficients) 
{
  int order (Shapelets1D::order());
  int nofCoeff (coefficients.size());
  
  // Check if the number of coefficients matches the order of the model; if
  // this is not the case, adjust the order according to the number of 
  // coefficients.
  
  if (order+1 != nofCoeff) {
    Shapelets1D::setOrder(nofCoeff-1);
  }
  
  // Resize the vector holding the coefficient values
  coefficients_p.resize(nofCoeff,0.0);
  
  // Copy the input values to the storage area
  for (int n=0; n<nofCoeff; n++) {
    coefficients_p[n] = coefficients[n];
  }
  
  // update the integral value
  ShapeletsModel1D::updateGlobals ();
}

// =============================================================================
//
// Evaluation of the Shapelet model
//
// =============================================================================

double ShapeletsModel1D::fx (const double x)
{  
  int nofCoeff;
  double xx;
  double y;
  
  nofCoeff = coefficients_p.size();
  y = 0.0;
  xx = x-center_p;
  
  // evaluate the model
  for (int n=0; n<nofCoeff; n++) {
    y += coefficients_p[n]*Shapelets1D::fx(n,xx);
  }
  
  return y;	
}

vector<double> ShapeletsModel1D::fx (const vector<double>& x)
{  
  int nelem (x.size());      // Number of function arguments
  vector<double> y (nelem);  // Vector with the function values
  
  for (int i=0; i<nelem; i++) {
    y[i] = ShapeletsModel1D::fx(x[i]);
  }
  
  return y;
}


// =============================================================================
//
//  Global model properties
//
// =============================================================================

void ShapeletsModel1D::updateGlobals ()
{
  ShapeletsModel1D::calcIntegral ();
}

double ShapeletsModel1D::integral () {
  return integral_p;
}

void ShapeletsModel1D::calcIntegral () {
  
  int order (Shapelets1D::order());

  integral_p = 0.0;
  
  for (int n=0; n<=order; n++) {
    integral_p += coefficients_p[n]*Shapelets1D::integral(n);
  }
  
}
