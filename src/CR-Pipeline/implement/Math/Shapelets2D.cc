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

/* $Id$ */

/*!
  \class Shapelets2D
*/

#include <Math/Shapelets2D.h>

namespace CR {  // Namespace CR -- BEGIN

// =============================================================================
//
// Construction / Deconstruction
//
// =============================================================================

Shapelets2D::Shapelets2D ()
  : Shapelets1D ()
{}

Shapelets2D::Shapelets2D (int const &order)
  : Shapelets1D (order)
{}

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

// =============================================================================
//
//  Global function properties
//
// =============================================================================

double Shapelets2D::integral (int const &l,
			      int const &m)
{
  return Shapelets1D::integral(l)*Shapelets1D::integral(m);
}

double Shapelets2D::integral (vector<int> const &l)
{
  return Shapelets1D::integral(l[0])*Shapelets1D::integral(l[1]);
}

}  // Namespace CR -- END
