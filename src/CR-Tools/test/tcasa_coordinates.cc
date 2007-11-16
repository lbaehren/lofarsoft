/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2007                                                    *
 *   Lars Baehren (bahren@astron.nl)                                       *
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
#include <string>

using std::cerr;
using std::cout;
using std::endl;
using std::string;

#include <casa/aips.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/Vector.h>
#include <casa/BasicSL/String.h>
#include <casa/Quanta/Quantum.h>
#include <coordinates/Coordinates/DirectionCoordinate.h>
#include <coordinates/Coordinates/LinearCoordinate.h>
#include <coordinates/Coordinates/Projection.h>
#include <measures/Measures/MDirection.h>

using casa::Double;
using casa::Matrix;
using casa::MDirection;
using casa::Projection;
using casa::Quantum;
using casa::Vector;

int test_Coordinates ()
{
  cout << "\n[test_Coordinates]\n" << endl;

  int nofFailedTests (0);
  
  cout << "[1] Creating DirectionCoordinate ..." << endl;
  try {
    Matrix<Double> xform(2,2);
    xform = 0.0; xform.diagonal() = 1.0;
    Quantum<Double> refLon(135.0, "deg");
    Quantum<Double> refLat(60.0, "deg");
    Quantum<Double> incLon(-1.0, "deg");
    Quantum<Double> incLat(1.0, "deg");
    casa::DirectionCoordinate coord (MDirection::J2000,
				     Projection(Projection::SIN),
				     refLon,
				     refLat,
				     incLon,
				     incLat,
				     xform,
				     128,
				     128);
    // display properties of newly created coordinate
    cout << " -- reference value = " << coord.referenceValue() << endl;
    cout << " -- reference pixel = " << coord.referencePixel() << endl;
    cout << " -- increment       = " << coord.increment() << endl;
  } catch (string message) {
    cerr << message << endl;
    nofFailedTests++;
  }
  
  cout << "[2] Creating LinearCoordinate ..." << endl;
  try {
    Vector<Double> crpix(1);
    Vector<Double> crval(1);
    Vector<Double> cdelt(1);
    Matrix<Double> pc(1,1);
    Vector<casa::String> names(1);
    Vector<casa::String> units(1);

    crpix         = 0.0;
    crval         = 100.0;
    cdelt         = -10.0;
    pc            = 0;
    pc.diagonal() = 1.0;
    names         = "length";
    units         = "km";

    casa::LinearCoordinate coord (names,
				  units,
				  crval,
				  cdelt,
				  pc,
				  crpix);

    // display properties of newly created coordinate
    cout << " -- reference value = " << coord.referenceValue() << endl;
    cout << " -- reference pixel = " << coord.referencePixel() << endl;
    cout << " -- increment       = " << coord.increment() << endl;

  } catch (string message) {
    cerr << message << endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}

int main ()
{
  int nofFailedTests (0);

  nofFailedTests += test_Coordinates ();

  return nofFailedTests;
}
