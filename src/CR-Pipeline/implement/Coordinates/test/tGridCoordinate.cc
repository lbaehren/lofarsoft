/***************************************************************************
 *   Copyright (C) 2006                                                    *
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

/* $Id: tGridCoordinate.cc,v 1.2 2006/08/24 13:55:12 bahren Exp $*/

#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/Vector.h>
#include <casa/BasicSL/String.h>
#include <casa/Exceptions/Error.h>
#include <coordinates/Coordinates/LinearCoordinate.h>

#include <Coordinates/GridCoordinate.h>

/*!
  \file tGridCoordinate.cc

  \brief A collection of test routines for GridCoordinate
 
  \author Lars B&auml;hren
 
  \date 2006/04/04

  <h3>Motivation</h3>

  We want to be able to work with a simple linear axis, which is defined within 
  a certain value range.
*/

using std::cerr;
using std::cout;
using std::endl;

using casa::Double;
using casa::Int;
using casa::String;
using casa::uInt;

using CR::GridCoordinate;

// -----------------------------------------------------------------------------

/*!
  \brief Retrieve the internal parameters of the GridCoordinate object

  This will make a call to the various methods that can be used to retrieve the 
  objects internal parameters and lists the output, e.g.
  \verbatim
  [1] Testing default constructor ...
   Bottom-left-corner   : [1]
   Top-right-corner     : [1]
   Shape of the grid    : [1]
   World axis names     : []
   World axis units     : []
   Reference value      : [0]
   Reference pixel      : [0]
   Coordinate increment : [1]
  \endverbatim
*/
void show (GridCoordinate& grid)
{
  cout << " -- Bottom-left-corner   : " << grid.blc() << endl;
  cout << " -- Top-right-corner     : " << grid.trc() << endl;
  cout << " -- Shape of the grid    : " << grid.shape() << endl;
  cout << " -- World axis names     : " << grid.worldAxisNames() << endl;
  cout << " -- World axis units     : " << grid.worldAxisUnits() << endl;
  cout << " -- Reference value      : " << grid.referenceValue() << endl;
  cout << " -- Reference pixel      : " << grid.referencePixel() << endl;
  cout << " -- Coordinate increment : " << grid.increment() << endl;
}

// -----------------------------------------------------------------------------

/*!
  \brief Test constructors for a new GridCoordinate object

  \return nofFailedTests -- The number of failed tests.
*/
Int test_GridCoordinate ()
{
  Int nofFailedTests (0);
  uInt naxes (3);
  String name  ("Distance");
  String unit  ("m");
  Double min   (100.0);
  Double max   (10000.0);
  uInt nofNodes (500);
  
  cout << "\n[test_GridCoordinate]\n" << endl;

  cout << "[1] Testing default constructor ..." << endl;
  try {
    GridCoordinate grid;
    //
    show (grid);
  } catch (AipsError x) {
    cerr << "[test_GridCoordinate] " << x.getMesg() << endl;
  }
  
  cout << "[2] Testing argumented constructor ..." << endl;
  try {
    GridCoordinate grid (min,max,nofNodes);
    //
    show (grid);
  } catch (AipsError x) {
    cerr << "[test_GridCoordinate] " << x.getMesg() << endl;
  }
  
  cout << "[3] Testing argumented constructor ..." << endl;
  try {
    IPosition shape (naxes,nofNodes);
    GridCoordinate grid (Vector<Double> (naxes,min),
			 Vector<Double> (naxes,max),
			 shape);
    //
    show (grid);
  } catch (AipsError x) {
    cerr << "[test_GridCoordinate] " << x.getMesg() << endl;
  }
  
  cout << "[4] Testing argumented constructor ..." << endl;
  try {
    IPosition shape (naxes,500);
    GridCoordinate grid (Vector<String> (naxes,name),
			 Vector<String> (naxes,unit),
			 Vector<Double> (naxes,min),
			 Vector<Double> (naxes,max),
			 shape);
    //
    show (grid);
  } catch (AipsError x) {
    cerr << "[test_GridCoordinate] " << x.getMesg() << endl;
  }
  
  cout << "[5] Testing copy constructor ..." << endl;
  try {
    GridCoordinate grid1;
    GridCoordinate grid2 (grid1);
    //
    cout << " - original object:" << endl;
    show (grid1);
    cout << " - copied object:" << endl;
    show (grid2);
  } catch (AipsError x) {
    cerr << "[test_GridCoordinate] " << x.getMesg() << endl;
  }
  
  return nofFailedTests;
}

// -----------------------------------------------------------------------------

int main ()
{
  Int nofFailedTests (0);

  // Test for the constructor(s)
  {
    nofFailedTests += test_GridCoordinate ();
  }

  return nofFailedTests;
}
