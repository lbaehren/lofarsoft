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

#include "Shapelets2D.h"

// =============================================================================
//
// Construction / Deconstruction
//
// =============================================================================

//_______________________________________________________________________________
//                                                                    Shapelets2D

Shapelets2D::Shapelets2D ()
  : Shapelets1D ()
{}

//_______________________________________________________________________________
//                                                                    Shapelets2D

/*!
  \param order - Maximum order for which the function coefficients are
         pre-computed.
  \param beta  - Shapelet scale parameter, \f$ \beta \f$.
*/
Shapelets2D::Shapelets2D (int const &order, 
			  double const &beta)
  : Shapelets1D (order,beta)
{}

Shapelets2D::~Shapelets2D () {}

// =============================================================================
//
//  Function evaluation
//
// =============================================================================

double Shapelets2D::fx (int const &l, 
			  double const &x,
			  int const &m,
			  double const &y) 
{  
  return Shapelets1D::fx(l,x)*Shapelets1D::fx(m,y);
}

double Shapelets2D::fx (const std::vector<int>& l,
			const std::vector<double>& x)
{
  return Shapelets1D::fx(l[0],x[0])*Shapelets1D::fx(l[1],x[1]);
}


// =============================================================================
//
//  Global function properties
//
// =============================================================================

double Shapelets2D::integral (const int l,
			      const int m)
{
  return Shapelets1D::integral(l)*Shapelets1D::integral(m);
}

double Shapelets2D::integral (const std::vector<int>& l)
{
  return Shapelets1D::integral(l[0])*Shapelets1D::integral(l[1]);
}
