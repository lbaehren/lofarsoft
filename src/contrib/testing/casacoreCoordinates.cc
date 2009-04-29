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

/*!
  \file casacoreCoordinates.cc

  \brief A number of tests for clases in the casacore coordinates module

  \ingroup contrib

  \author Lars B&auml;hren

  <b>Note:</b> A number of the tests performed will not work with the original CASA
  libraries.

  The tests fall into two groups:
  <ol>
    <li>Basic tests working with the various coordinate classes of the casacore
    library, such as <i>DirectionCoordinate</i>, <i>LinearCoordinate</i>.
    <li>Combination of various coordinates within a STL container
  </ol>
*/

//_______________________________________________________________________________
//                                                                show_coordinate

/*!
  \brief Show properties of a casa::Coordinate object

  \param coord -- Coordinate object of which to show the properties.
*/
void show_coordinate (casa::Coordinate &coord)
{
  cout << "-- Coord. type      = " << coord.type()           << endl;
  cout << "-- Coord. name      = " << coord.showType()       << endl;
  cout << "-- nof. pixel axes  = " << coord.nPixelAxes()     << endl;
  cout << "-- nof. world axes  = " << coord.nWorldAxes()     << endl;
  cout << "-- World axis names = " << coord.worldAxisNames() << endl;
  cout << "-- World axis units = " << coord.worldAxisUnits() << endl;
  cout << "-- Reference value  = " << coord.referenceValue() << endl;
  cout << "-- Reference pixel  = " << coord.referencePixel() << endl;
  cout << "-- Increment        = " << coord.increment()      << endl;

  /* Test pixel to world conversion */

  int nofPixelAxes (coord.nPixelAxes());
  Vector<double> pixel (nofPixelAxes);
  Vector<double> world (nofPixelAxes);

  for (int n(0); n<nofPixelAxes; n++) {
    pixel    = 0.0;
    pixel(n) = 1.0;
    //conversion
    coord.toWorld(world,pixel);
    // feedback
    cout << "-- Pixel to World   = " << pixel << " -> " << world << endl;
  }
}

// ------------------------------------------------------------------------------

/*!
  \brief Test working with the Coordinates::DirectionCoordinate class

  \return nofFailedTests -- The number of failed tests encountered within this
          function.
*/
int test_DirectionCoordinate ()
{
  cout << "\n[test_DirectionCoordinate]\n" << endl;

  int nofFailedTests (0);
  Quantum<Double> refLon(135.0, "deg");
  Quantum<Double> refLat(60.0, "deg");
  Quantum<Double> incLon(-1.0, "deg");
  Quantum<Double> incLat(1.0, "deg");
  
  cout << "[1] J2000 / SIN ..." << endl;
  try {
    Matrix<Double> xform(2,2);
    xform = 0.0; xform.diagonal() = 1.0;
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
    show_coordinate (coord);
  } catch (string message) {
    cerr << message << endl;
    nofFailedTests++;
  }
  
  cout << "[1] AZEL / STG ..." << endl;
  try {
    Matrix<Double> xform(2,2);
    xform = 0.0; xform.diagonal() = 1.0;
    casa::DirectionCoordinate coord (MDirection::AZEL,
				     Projection(Projection::STG),
				     refLon,
				     refLat,
				     incLon,
				     incLat,
				     xform,
				     128,
				     128);
    // display properties of newly created coordinate
    show_coordinate (coord);
  } catch (string message) {
    cerr << message << endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}

// ------------------------------------------------------------------------------

/*!
  \brief Test working with the Coordinates::LinearCoordinate class

  \return nofFailedTests -- The number of failed tests encountered within this
          function.
*/
int test_LinearCoordinate ()
{
  cout << "\n[test_LinearCoordinate]\n" << endl;

  int nofFailedTests (0);

  cout << "[1] Creating 1-dim LinearCoordinate..." << endl;
  try {
    Vector<Double> crpix(1,0.0);
    Vector<Double> crval(1,100);
    Vector<Double> cdelt(1,-10);
    Matrix<Double> pc(1,1);
    Vector<casa::String> names(1,"Length");
    Vector<casa::String> units(1,"km");

    pc            = 0;
    pc.diagonal() = 1.0;

    casa::LinearCoordinate coord (names,
				  units,
				  crval,
				  cdelt,
				  pc,
				  crpix);

    show_coordinate (coord);

  } catch (string message) {
    cerr << message << endl;
    nofFailedTests++;
  }

  cout << "[2] Creating 3-dim LinearCoordinate (Cartesian) ..." << endl;
  try {
    int nofAxes (3);
    Vector<Double> crpix(nofAxes,0.0);
    Vector<Double> crval(nofAxes,0.0);
    Vector<Double> cdelt(nofAxes,0.5);
    Matrix<Double> pc(nofAxes,nofAxes,0.0);
    Vector<casa::String> names(nofAxes,"Distance");
    Vector<casa::String> units(nofAxes,"m");

    pc.diagonal() = 1.0;

    // create coordinate object
    casa::LinearCoordinate coord (names,
				  units,
				  crval,
				  cdelt,
				  pc,
				  crpix);
    // display properties of newly created coordinate
    show_coordinate (coord);
  } catch (string message) {
    cerr << message << endl;
    nofFailedTests++;
  }
  
  cout << "[3] Creating 3-dim LinearCoordinate (Spherical) ..." << endl;
  try {
    int nofAxes (3);
    Vector<Double> crpix(nofAxes,0.0);
    Vector<Double> crval(nofAxes,0.0);
    Vector<Double> cdelt(nofAxes,0.5);
    Matrix<Double> pc(nofAxes,nofAxes,0.0);
    Vector<casa::String> names(nofAxes);
    Vector<casa::String> units(nofAxes);

    pc.diagonal() = 1.0;
    names(0) = "Longitude";
    names(1) = "Latitude";
    names(2) = "Length";
    units(0) = "deg";
    units(1) = "deg";
    units(2) = "m";

    // create coordinate object
    casa::LinearCoordinate coord (names,
				  units,
				  crval,
				  cdelt,
				  pc,
				  crpix);
    // display properties of newly created coordinate
    show_coordinate (coord);
  } catch (string message) {
    cerr << message << endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}

//_______________________________________________________________________________
//                                                               test_export2fits

/*!
  \brief Test translation of WCS information during export to FITS

  \return nofFailedTests -- The number of failed tests encountered within this
          function.
*/
int test_export2fits ()
{
  int nofFailedTests (0);
  
  std::vector<std::string> refcode;
  std::vector<std::string> projection;

  refcode.push_back("AZEL");
  refcode.push_back("B1950");
  refcode.push_back("J2000");
  refcode.push_back("GALACTIC");

  projection.push_back("CAR");
  projection.push_back("MER");
  projection.push_back("SIN");
  projection.push_back("STG");
  projection.push_back("TAN");

  for (uint ref(0); ref<refcode.size(); ref++) {
    for (uint proj(0); proj<projection.size(); proj++) {
      // set filename base
      std::string filename = "image-" + refcode[ref] + "-" + projection[proj];

    }
  }

  return nofFailedTests;
}

// ------------------------------------------------------------------------------

int main ()
{
  int nofFailedTests (0);

  nofFailedTests += test_DirectionCoordinate ();
  nofFailedTests += test_LinearCoordinate ();

  nofFailedTests += test_export2fits ();

  return nofFailedTests;
}
