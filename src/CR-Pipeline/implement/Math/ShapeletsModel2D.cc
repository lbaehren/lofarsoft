/***************************************************************************
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

/* $Id: ShapeletsModel2D.cc,v 1.1 2005/07/15 10:10:37 bahren Exp $ */

#include <Math/ShapeletsModel2D.h>

// =============================================================================
//
//  Construction
//
// =============================================================================

ShapeletsModel2D::ShapeletsModel2D ()
  : Shapelets1D ()
{
  ShapeletsModel2D::initModel ();
}

ShapeletsModel2D::ShapeletsModel2D (const int order,
				    const double beta) 
  : Shapelets1D (order,beta)
{
  ShapeletsModel2D::initModel ();
}

ShapeletsModel2D::ShapeletsModel2D (const vector<double>& center,
				    const double coefficients[])
  : Shapelets1D ()
{
  ShapeletsModel2D::setCenter (center);
  ShapeletsModel2D::setCoefficients (coefficients);
}

ShapeletsModel2D::ShapeletsModel2D (const int order,
				    const double beta,
				    const vector<double>& center,
				    const double coefficients[]) 
  : Shapelets1D (order,beta)
{
  ShapeletsModel2D::setCenter (center);
  ShapeletsModel2D::setCoefficients (coefficients);
}

ShapeletsModel2D::~ShapeletsModel2D ()
{
//   delete [] coefficients_p;
}

// =============================================================================
//
//  Initialization of the internal settings
//
// =============================================================================

void ShapeletsModel2D::initModel ()
{
  int order (Shapelets1D::order());
  int nofCoeff (order+1);

  center_p.resize (2,0.0);

  coefficients_p = new double[nofCoeff*nofCoeff];
}

// =============================================================================
//
//  Access to the model parameters
//
// =============================================================================

void ShapeletsModel2D::setOrder (const int order)
{
  Shapelets1D::setOrder (order);
  ShapeletsModel2D::updateGlobals ();
}

void ShapeletsModel2D::setBeta (const double beta) 
{
  Shapelets1D::setBeta (beta);
  ShapeletsModel2D::updateGlobals ();
}

vector<double> ShapeletsModel2D::center ()
{
  return center_p;
}

void ShapeletsModel2D::setCenter (const vector<double>& center)
{
  center_p.resize (center.size(),0.0);
  center_p = center;
}

// === Weighting coefficients of the Shapelet components =======================

double ShapeletsModel2D::coefficients (const int n1,
				       const int n2)
{
  int order (Shapelets1D::order());
  int nofCoeff (order+1);

  return coefficients_p[n1*nofCoeff+n2];
}

void ShapeletsModel2D::setCoefficients (const double coeff[]) 
{
  
}

void ShapeletsModel2D::setCoefficients (const double coeff,
					const vector<int>& n)
{
  ShapeletsModel2D::setCoefficients (coeff,n[0],n[1]);
}

void ShapeletsModel2D::setCoefficients (const double coeff,
					const int n1,
					const int n2)
{
  int order (Shapelets1D::order());
  int nofCoeff (order+1);
  
  coefficients_p[n1*nofCoeff+n2] = coeff;
}


// =============================================================================
//
// Evaluation of the Shapelet model
//
// =============================================================================

double ShapeletsModel2D::eval (const vector<double>& x)
{
  return ShapeletsModel2D::eval (x[0], x[1]);
}

double ShapeletsModel2D::eval (const double x1,
			       const double x2)
{
  int order (Shapelets1D::order());
  int nofCoeff (order+1);
  double y1(0.0);
  double yy(0.0);
  vector<double> xx(2);
  
  xx[0] = x1-center_p[0];
  xx[1] = x2-center_p[1];
  
  // evaluate the model
  for (int n1=0; n1<nofCoeff; n1++) {
    y1 = Shapelets1D::eval(n1,xx[0]);
    for (int n2=0; n2<nofCoeff; n2++) {
      yy += coefficients_p[n1*nofCoeff+n2]*y1*Shapelets1D::eval(n2,xx[1]);
    }
  }
  
  return yy;	
}


// =============================================================================
//
//  Global model properties
//
// =============================================================================

void ShapeletsModel2D::updateGlobals ()
{
  ShapeletsModel2D::calcIntegral ();
}

double ShapeletsModel2D::integral () {
  return integral_p;
}

void ShapeletsModel2D::calcIntegral () {
  
  int order (Shapelets1D::order());
  int nofCoeff (order+1);
  double int1 (0.0);

  integral_p = 0.0;
  
  // Calculate the integral using the integrals of the basis functions
  for (int n1=0; n1<nofCoeff; n1++) {
    int1 = Shapelets1D::integral(n1);
    for (int n2=0; n2<nofCoeff; n2++) {
      integral_p += coefficients_p[n1*nofCoeff+n2]*int1*Shapelets1D::integral(n2);
    }
  }
  
}
