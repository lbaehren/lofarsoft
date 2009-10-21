/*-------------------------------------------------------------------------*
| $Id$ |
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

#include <Coordinates/GridCoordinate.h>

namespace CR {  // Namespace CR -- begin
  
  // ==============================================================================
  //
  //  Construction
  //
  // ==============================================================================
  
  GridCoordinate::GridCoordinate ()
    : LinearCoordinate (1),
      blc_p (Vector<double> (1,0.0)),
      trc_p (Vector<double> (1,1.0)),
      shape_p (IPosition (1,1))
  {;}
  
GridCoordinate::GridCoordinate (double const &blc,
				double const &trc,
				uInt const &shape)
  : LinearCoordinate (1),
    blc_p (Vector<double> (1,blc)),
    trc_p (Vector<double> (1,trc)),
    shape_p (IPosition (1,shape))
{
  setGridIncrement();
}

GridCoordinate::GridCoordinate (String const &names,
				String const &units,
				double const &blc,
				double const &trc,
				uInt const &shape)
  : LinearCoordinate (1),
    blc_p (Vector<double> (1,blc)),
    trc_p (Vector<double> (1,trc)),
    shape_p (IPosition (1,shape))
{
  LinearCoordinate::setWorldAxisNames(Vector<String> (1,names));
  LinearCoordinate::overwriteWorldAxisUnits(Vector<String> (1,units));
  //
  setGridIncrement();
}

GridCoordinate::GridCoordinate (Vector<double> const &blc,
				Vector<double> const &trc,
				IPosition const &shape)
  : LinearCoordinate (shape.nelements()),
    blc_p (blc),
    trc_p (trc),
    shape_p (shape)
{
  setGridIncrement();
}

GridCoordinate::GridCoordinate (Vector<String> const &names,
				Vector<String> const &units,
				Vector<double> const &blc,
				Vector<double> const &trc,
				IPosition const &shape)
  : LinearCoordinate (shape.nelements()),
    blc_p (blc),
    trc_p (trc),
    shape_p (shape)
{
  LinearCoordinate::setWorldAxisNames(names);
  LinearCoordinate::overwriteWorldAxisUnits(units);
  //
  setGridIncrement();
}

GridCoordinate::GridCoordinate (GridCoordinate const &other)
{
  copy (other);
}

// ==============================================================================
//
//  Destruction
//
// ==============================================================================

GridCoordinate::~GridCoordinate ()
{
  destroy();
}

// ==============================================================================
//
//  Operators
//
// ==============================================================================

GridCoordinate& GridCoordinate::operator= (const GridCoordinate& other)
{
  if (this != &other) {
    destroy ();
    copy (other);
  }
  return *this;
}

void GridCoordinate::copy (const GridCoordinate& other)
{
  blc_p   = other.blc_p;
  trc_p   = other.trc_p;
  shape_p = other.shape_p;
}

void GridCoordinate::destroy ()
{;}

// ==============================================================================
//
//  Parameters
//
// ==============================================================================

// ----------------------------------------------------------------------- setTRC

void GridCoordinate::setTRC (const Vector<double>& trc)
{
  trc_p = trc;
  setGridIncrement();
}

// ----------------------------------------------------------------------- setBLC

void GridCoordinate::setBLC (const Vector<double>& blc)
{
  blc_p = blc;
  setGridIncrement();
}

// --------------------------------------------------------------------- setShape

void GridCoordinate::setShape (const IPosition& shape)
{
  shape_p = shape;
  setGridIncrement();
}

// ==============================================================================
//
//  Methods from the base class
//
// ==============================================================================

bool GridCoordinate::setIncrement (const Vector<double> &incr)
{
  uInt naxes (incr.nelements());
  
  // check array sizes
  if (blc_p.nelements() == naxes && trc_p.nelements() == naxes && shape_p.nelements()== naxes) {
    // recompute the number of grid nodes
    for (uInt n(0); n<naxes; n++) {
      shape_p(n) = int((trc_p(n)-blc_p(n))/incr(n) + 1.0);
    }
  }

  return LinearCoordinate::setIncrement (incr);
}


// ==============================================================================
//
//  Methods
//
// ==============================================================================

void GridCoordinate::setGridIncrement ()
{

  Vector<double> incr (LinearCoordinate::increment());
  uInt naxes (incr.nelements());
  
  // check array sizes
  if (blc_p.nelements() == naxes && trc_p.nelements() == naxes && shape_p.nelements()== naxes) {  
    for (uInt n(0); n<naxes; n++) {
      incr(n) = (trc_p(n)-blc_p(n))/(shape_p(n)-1.0);
    }
  }

  LinearCoordinate::setIncrement (incr);
  LinearCoordinate::setReferenceValue (blc_p);
  LinearCoordinate::setReferencePixel (Vector<double> (naxes,0.0));
}
  
  // ------------------------------------------------------------------ coordinates
  
  Array<double> coordinates ()
  {
    Array<double> coords;
    
    return coords;
  }
  
}  // Namespace CR -- end
