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

/* $Id: CoordinateGrid.cc,v 1.2 2006/01/16 18:17:33 bahren Exp $ */

#include <Coordinates/CoordinateGrid.h>

using std::cerr;
using std::cout;
using std::endl;

namespace CR {  // Namespace CR -- begin

// ----------------------------------------------- CoordinateGrid::CoordinateGrid

CoordinateGrid::CoordinateGrid ()
{
  int nofAxes (2);
  Vector<int> shape;
  Vector<double> center;
  Vector<double> increment;
  Vector<String> units (nofAxes);
  
  shape = CoordinateGrid::defaultShape ();
  center = CoordinateGrid::defaultCenter ();
  increment = CoordinateGrid::defaultIncrement ();
  units = CoordinateGrid::defaultUnits (nofAxes);
  
  CoordinateGrid::initCoordinateGrid (shape,center,increment,units);
}

// ----------------------------------------------- CoordinateGrid::CoordinateGrid

CoordinateGrid::CoordinateGrid (const Vector<int>& shape,
				const Vector<double>& center,
				const Vector<double>& increment)
{
  int nofAxes (shape.nelements());
  Vector<String> units (CoordinateGrid::defaultUnits (nofAxes));
  //
  CoordinateGrid::initCoordinateGrid (shape,
				      center,
				      increment,
				      units);
}

// ----------------------------------------------- CoordinateGrid::CoordinateGrid

CoordinateGrid::CoordinateGrid (const Vector<int>& shape,
				const Vector<double>& center,
				const Vector<double>& increment,
				const Vector<String>& units)
{
  CoordinateGrid::initCoordinateGrid (shape,
				      center,
				      increment,
				      units);
}

// =============================================================================
//
//  Destruction
//
// =============================================================================

CoordinateGrid::~CoordinateGrid () {}

// =============================================================================
//
//  Initialization
//
// =============================================================================

// ------------------------------------------- CoordinateGrid::initCoordinateGrid

void CoordinateGrid::initCoordinateGrid (const Vector<int>& shape,
					 const Vector<double>& center,
					 const Vector<double>& increment,
					 const Vector<String>& units)
{
  // Check if the shapes are consistent
  int nShape (shape.nelements());
  int nCenter (center.nelements());
  int nIncrement (increment.nelements());
  int nUnits (units.nelements());
  
  if (nCenter == nIncrement && nCenter == nShape && nCenter == nUnits) {
    shape_p = shape;
    center_p = center;
    increment_p = increment;
    units_p = units;
    //
    useLimits_p = false;
    limits_p = CoordinateGrid::defaultLimits (nShape);
    //
    CoordinateGrid::setGrid ();
  } else {
    cerr << "[CoordinateGrid::initCoordinateGrid] Inconsistent array shapes"
	 << endl;
    cerr << " - center    : " << nCenter << endl;
    cerr << " - increment : " << nIncrement << endl;
    cerr << " - shape     : " << nShape << endl;
    cerr << " - units     : " << nUnits << endl;
  }
}

// =============================================================================
//
//  Default values
//
// =============================================================================

Vector<double> CoordinateGrid::defaultCenter ()
{
  Vector<double> center(3);
  //
  center(0) = 0.0;
  center(1) = 90.0;
  center(2) = -1.0;
  //
  return center;
}

Vector<double> CoordinateGrid::defaultIncrement ()
{
  Vector<double> increment(3);
  //
  increment(0) = 1.0;
  increment(1) = 1.0;
  increment(2) = 1.0;
  // 
  return increment;
}

Vector<int> CoordinateGrid::defaultShape ()
{
  Vector<int> shape(3);
  //
  shape = 10;
  //
  return shape;
}

Vector<String> CoordinateGrid::defaultUnits (const int nofAxes) {

  Vector<String> units(nofAxes);
  //
  if (nofAxes == 1) {
    units(0) = "deg";
  }
  if (nofAxes == 2) {
    units(1) = "deg";
  }
  if (nofAxes == 3) {
    units(2) = "m";
  }
  //
  return units;
}

Matrix<double> CoordinateGrid::defaultLimits (const int naxes) {

  Matrix<double> limits (naxes,2);
  //
  limits = 0.0;
  //
  return limits;

}

// =============================================================================
//
//  Access to the grid parameters
//
// =============================================================================

IPosition CoordinateGrid::shape ()
{
  return shape_p;
}

void CoordinateGrid::setShape (const IPosition& shape)
{
  uint nelem (shape.nelements());
  //
  if (nelem == shape_p.nelements()) {
    shape_p = shape;
  } else {
    cerr << "[CoordinateGrid::setShape] Inconsistent array shapes" << endl;
    cerr << " - Stored shape : " << shape_p << endl;
    cerr << " - New shape .. : " << shape << endl;
  }
}

void CoordinateGrid::setShape (const Vector<int>& shape)
{
  int nelem (shape.nelements());
  IPosition newshape (nelem);
  
  for (int i=0; i<nelem; i++) {
    newshape(i) = shape(i);
  }
  
  CoordinateGrid::setShape (newshape);
}

Vector<double> CoordinateGrid::center ()
{
  return center_p;
}

Vector<double> CoordinateGrid::increment ()
{
  return increment_p;
}

Vector<String> CoordinateGrid::units ()
{
  return units_p;
}

// =============================================================================
//
//  Masking of the grid node values
//
// =============================================================================

Matrix<double> CoordinateGrid::limits ()
{
  return limits_p;
}

Vector<double> CoordinateGrid::limits (const int& axis)
{
  Vector<double> limits(2);
  //
  limits(0) = limits_p(axis,0);
  limits(1) = limits_p(axis,1);
  //
  return limits;
}

void CoordinateGrid::setLimits (const Matrix<double>& limits)
{
  // Check if the shape if correct
  IPosition shapeNEW (limits.shape());
  IPosition shapeOLD (limits_p.shape());
  
  if (shapeNEW == shapeOLD) {
    limits_p = limits;
    //
    CoordinateGrid::setMask ();
  } else {
    cerr << "[CoordinateGrid::setLimits] Inconsistent array shapes" << endl;
    cerr << " - Shape of stored limits : " << shapeOLD << endl;
    cerr << " - Shape of new limits .. : " << shapeNEW << endl;
  }
}

void CoordinateGrid::setLimits (const Vector<double>& limits,
				const int& axis)
{
  int nofAxes (shape_p.nelements());

  if (axis < nofAxes) {
    limits_p(axis,0) = min(limits);
    limits_p(axis,1) = max(limits);
    //
    CoordinateGrid::setMask ();
  } else {
    cerr << "[CoordinateGrid::setLimits] Invalid axis number" << endl;
    cerr << " - Number of grid axes    : " << nofAxes << endl;
    cerr << " - Maximum value of index : " << nofAxes-1 << endl;
    cerr << " - Targetted grid axis    : " << axis << endl;
  }
}

bool CoordinateGrid::useLimits ()
{
  return useLimits_p;
}

void CoordinateGrid::useLimits (const bool useLimits)
{
  useLimits_p = useLimits;
}

void CoordinateGrid::setMask ()
{
  int nofNodes;
  int nofAxes;

  grid_p.shape(nofNodes,nofAxes);
  
  mask_p.resize (nofNodes);

  for (int axis=0; axis<nofAxes; axis++) {
    for (int node=0; node<nofNodes; node++) {
      if (grid_p(node,axis) < limits_p(axis,0)) {
	mask_p(node) = false;
      }
      else if (grid_p(node,axis) > limits_p(axis,1)) {
	mask_p(node) = false;
      }
    }
  }

  // Activate using the just set limits
  CoordinateGrid::useLimits (true);
}

// =============================================================================
//
//  Coordinate Grid
//
// =============================================================================

// --------------------------------------------------------- CoordinateGrid::grid

void CoordinateGrid::grid (std::ostream & os)
{
  int nelem;
  int naxes;
  
  grid_p.shape(nelem,naxes);
  
  for (int i=0; i<nelem; i++) {
    for (int n=0; n<naxes; n++) {
      os << grid_p(i,n) << "\t";
    }
    os << endl;
  }
  
}

// --------------------------------------------------------- CoordinateGrid::grid

void CoordinateGrid::grid (Array<double>& grid)
{
  int nofAxes (shape_p.nelements());
  IPosition shape (nofAxes+1,0);
  int nelem (1);

  for (int axis=0; axis<nofAxes; axis++) {
    shape(axis) = shape_p(axis);
    nelem *= shape_p(axis);
  }
  shape(nofAxes) = nofAxes;

  grid.resize (shape);
  
  // -----------------------------------------------------------------
  // Fill the values of the grid node coordinates from the internal
  // array into the returned array.

  shape = 0;
  shape(0) = -1;    
  for (int m=0; m<nelem; m++) {
    // increment the last element in the position vector
    shape(0) += 1;
    // go through the axes and check if we are within the axis shape
    for (int n=0; n<nofAxes-1; n++) {
      // check if we're within the range of this axis
      if (shape(n) == shape_p(n)) {
	shape(n) = 0;
	shape(n+1) += 1;
      }
    }
    // compute the coordinates of the grid node
    for (int n=0; n<nofAxes; n++) {
      shape(nofAxes) = n;
      // Debugging code:
//       cout << m << "\t" << shape << "\t" << grid_p (m,n) << endl;
      grid(shape) = grid_p (m,n);
    }
  }
  
}

// ------------------------------------------------------ CoordinateGrid::setGrid

void CoordinateGrid::setGrid ()
{
  bool verboseON (false);
  int nofAxes (shape_p.nelements());
  int nelem (1);
  Vector<double> coord(nofAxes);
  
  // Resize the matrix holding the grid coordinates
  for (int i=0; i<nofAxes; i++) {
    nelem *= shape_p(i);
  }
  grid_p.resize(nelem,nofAxes);
  
  // -----------------------------------------------------------------
  // Provide some feedback (if enabled)

  if (verboseON) {
    cout << "[CoordinateGrid::setGrid]" << endl;
    cout << " - Shape of the coordinates matrix : " << grid_p.shape() << endl;
    cout << " - Shape of the coordinate vector  : " << coord.shape() << endl;
  }
    
  // -----------------------------------------------------------------
  // Compute the coordinates of the grid nodes
  
  if (nofAxes == 2) {
    nelem = 0;
    //
    for (int i=0; i<shape_p(0); i++) {
      coord(0) = CoordinateGrid::calcCoordinate (0,i);
      for (int j=0; j<shape_p(0); j++) {
	coord(1) = CoordinateGrid::calcCoordinate (1,j);
	//
	grid_p (nelem,0) = coord (0); 
	grid_p (nelem,1) = coord (1); 
	//
	nelem++;
      }
    }
  }
  else if (nofAxes == 3) {
    nelem = 0;
    //
    for (int i=0; i<shape_p(0); i++) {
      coord(0) = CoordinateGrid::calcCoordinate (0,i);
      for (int j=0; j<shape_p(0); j++) {
	coord(1) = CoordinateGrid::calcCoordinate (1,j);
	for (int k=0; k<shape_p(0); k++) {
	  coord(2) = CoordinateGrid::calcCoordinate (2,k);
	  //
	  grid_p (nelem,0) = coord (0); 
	  grid_p (nelem,1) = coord (1); 
	  grid_p (nelem,2) = coord (2); 
	  //
	  nelem++;
	}
      }
    }
  }
  else if (nofAxes > 3) {
    // --- experimental code for an N-dimensional regular grid -------
    Vector<int> pos (nofAxes,0);
    //
    pos(0) = -1;    
    for (int m=0; m<nelem; m++) {
      // increment the last element in the position vector
      pos(0) += 1;
      // go through the axes and check if we are within the axis shape
      for (int n=0; n<nofAxes-1; n++) {
	// check if we're within the range of this axis
	if (pos(n) == shape_p(n)) {
	  pos(n) = 0;
	  pos(n+1) += 1;
	}
      }
      // compute the coordinates of the grid node
      for (int n=0; n<nofAxes; n++) {
	grid_p (m,n) = CoordinateGrid::calcCoordinate (n,pos(n));
      }
    }
    // ---------------------------------------------------------------
  }
  
}

// ----------------------------------------------- CoordinateGrid::calcCoordinate

double CoordinateGrid::calcCoordinate (const int numCoord, 
				       const int numPoint)
{
  double coord (center_p(numCoord));
  coord += increment_p(numCoord)*(numPoint-(shape_p(numCoord)-1)/2.0);
  return coord;
}

}  // Namespace CR -- end
