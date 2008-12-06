/*-------------------------------------------------------------------------*
 | $Id:: tNewClass.cc 1302 2008-03-11 10:04:05Z baehren                  $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2008                                                    *
 *   Lars B"ahren (lbaehren@gmail.com)                                     *
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

#include <Coordinates/CoordinateType.h>
#include <Coordinates/SpatialCoordinate.h>

// Namespace usage
using std::cerr;
using std::cout;
using std::endl;
using CR::CoordinateType;
using CR::SpatialCoordinate;

/*!
  \file tSpatialCoordinate.cc

  \ingroup CR_Coordinates

  \brief A collection of test routines for the SpatialCoordinate class
 
  \author Lars B&auml;hren
 
  \date 2008/08/28
*/

// -----------------------------------------------------------------------------

/*!
  \brief Test constructors for a new SpatialCoordinate object

  \return nofFailedTests -- The number of failed tests encountered within this
          function.
*/
int test_SpatialCoordinate ()
{
  int nofFailedTests (0);
  
  cout << "\n[tSpatialCoordinate::test_SpatialCoordinate]\n" << std::endl;

  cout << "[1] Testing default constructor ..." << std::endl;
  try {
    SpatialCoordinate coord;
    //
    coord.summary(); 
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  cout << "[2] Construction with coordinate type ..." << std::endl;
  try {
    SpatialCoordinate directionRadius (CoordinateType::DirectionRadius);
    directionRadius.summary();
    //
    SpatialCoordinate cartesian (CoordinateType::Cartesian);
    cartesian.summary();
    //
    SpatialCoordinate spherical (CoordinateType::Spherical);
    spherical.summary();
    //
    SpatialCoordinate cylindrical (CoordinateType::Cylindrical);
    cylindrical.summary();
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  cout << "[3] Construction with reference code and projection" << std::endl;
  try {
    SpatialCoordinate coord1 (CoordinateType::DirectionRadius,
			      "J2000",
			      "SIN");
    SpatialCoordinate coord2 (CoordinateType::DirectionRadius,
			      "J2000",
			      "AIT");
    SpatialCoordinate coord3 (CoordinateType::DirectionRadius,
			      "J2000",
			      "TAN");
    coord1.summary();
    coord2.summary();
    coord3.summary();
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  cout << "[4] Testing construction from coordinate objects ..." << std::endl;
  try {
    casa::Vector<casa::String> units (1,"m");
    casa::Vector<casa::String> names (1,"Length");
    // create the coordinate objects required for the construction
    casa::DirectionCoordinate direction = CoordinateType::makeDirectionCoordinate();
    casa::LinearCoordinate linear       = CoordinateType::makeLinearCoordinate(1,
									       names,
									       units);
    //
    SpatialCoordinate coord (direction,linear);
    //
    coord.summary(); 
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

  cout << "[5] Testing construction from linear coordinate object ..." << endl;
  try {
    unsigned int nofAxes (3);
    casa::Vector<casa::String> units (nofAxes,"m");
    casa::Vector<casa::String> names (nofAxes,"Length");
    //
    casa::LinearCoordinate linear = CoordinateType::makeLinearCoordinate(nofAxes,
									 names,
									 units);
    //
    SpatialCoordinate coord (linear,CoordinateType::Cartesian);
    coord.summary();
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}

// -----------------------------------------------------------------------------

/*!
  \brief Test wrapping of parameter access and conversion functions

  Given the fact that we are potentially working with omre but a single object
  of type casa::Coordinate we need to wrap the calls to function handling either
  allowing access to internal parameters or carrying out coordinate conversions.

  \return nofFailedTests -- The number of failed tests encountered within this
          function.
*/
int test_wcs ()
{
  cout << "\n[tSpatialCoordinate::test_wcs]\n" << endl;
  
  int nofFailedTests (0);

  cout << "[1] Retrieve WCS parameters DirectionRadius coordinate ..." << endl;
  try {
    SpatialCoordinate coord;
    cout << "-- Reference pixel  = " << coord.referencePixel() << endl;
    cout << "-- Reference value  = " << coord.referenceValue() << endl;
    cout << "-- Increment        = " << coord.increment()      << endl;
    cout << "-- World axis units = " << coord.worldAxisUnits() << endl;
    cout << "-- World axis names = " << coord.worldAxisNames() << endl;
  } catch (std::string message) {
    cerr << message << endl;
    nofFailedTests++;
  }

  cout << "[2] Retrieve WCS parameters for Cartesian coordinate ..." << endl;
  try {
    SpatialCoordinate coord (CoordinateType::Cartesian);
    cout << "-- Reference pixel  = " << coord.referencePixel() << endl;
    cout << "-- Reference value  = " << coord.referenceValue() << endl;
    cout << "-- Increment        = " << coord.increment()      << endl;
    cout << "-- World axis units = " << coord.worldAxisUnits() << endl;
    cout << "-- World axis names = " << coord.worldAxisNames() << endl;
  } catch (std::string message) {
    cerr << message << endl;
    nofFailedTests++;
  }

  cout << "[3] Retrieve WCS parameters for Spherical coordinate ..." << endl;
  try {
    SpatialCoordinate coord (CoordinateType::Spherical);
    cout << "-- Reference pixel  = " << coord.referencePixel() << endl;
    cout << "-- Reference value  = " << coord.referenceValue() << endl;
    cout << "-- Increment        = " << coord.increment()      << endl;
    cout << "-- World axis units = " << coord.worldAxisUnits() << endl;
    cout << "-- World axis names = " << coord.worldAxisNames() << endl;
  } catch (std::string message) {
    cerr << message << endl;
    nofFailedTests++;
  }

  cout << "[4] Retrieve WCS parameters for Cylindrical coordinate ..." << endl;
  try {
    SpatialCoordinate coord (CoordinateType::Cylindrical);
    cout << "-- Reference pixel  = " << coord.referencePixel() << endl;
    cout << "-- Reference value  = " << coord.referenceValue() << endl;
    cout << "-- Increment        = " << coord.increment()      << endl;
    cout << "-- World axis units = " << coord.worldAxisUnits() << endl;
    cout << "-- World axis names = " << coord.worldAxisNames() << endl;
  } catch (std::string message) {
    cerr << message << endl;
    nofFailedTests++;
  }

  cout << "[5] Set increment of coordinate ..." << endl;
  try {
    cout << "--> SpatialCoordinate(DirectionRadius)" << endl;
    SpatialCoordinate coord (CoordinateType::DirectionRadius);
    cout << "-- old increment = " << coord.increment() << endl;
    coord.setIncrement(Vector<double>(3,0.5));
    cout << "-- new increment = " << coord.increment() << endl;
    //
    cout << "--> SpatialCoordinate(Cartesian)" << endl;
    SpatialCoordinate coord_cartesian (CoordinateType::Cartesian);
    cout << "-- old increment = " << coord_cartesian.increment() << endl;
    coord_cartesian.setIncrement(Vector<double>(3,0.5));
    cout << "-- new increment = " << coord_cartesian.increment() << endl;
    //
    cout << "--> SpatialCoordinate(Spherical)" << endl;
    SpatialCoordinate coord_spherical (CoordinateType::Spherical);
    cout << "-- old increment = " << coord_spherical.increment() << endl;
    coord_spherical.setIncrement(Vector<double>(3,0.5));
    cout << "-- new increment = " << coord_spherical.increment() << endl;
    //
    cout << "--> SpatialCoordinate(Cylindrical)" << endl;
    SpatialCoordinate coord_cylindrical (CoordinateType::Cylindrical);
    cout << "-- old increment = " << coord_cylindrical.increment() << endl;
    coord_cylindrical.setIncrement(Vector<double>(3,0.5));
    cout << "-- new increment = " << coord_cylindrical.increment() << endl;
  } catch (std::string message) {
    cerr << message << endl;
    nofFailedTests++;
  }

  cout << "[6] Set reference pixel of coordinate ..." << endl;
  try {
    cout << "--> SpatialCoordinate(DirectionRadius)" << endl;
    SpatialCoordinate coord (CoordinateType::DirectionRadius);
    cout << "-- old reference pixel = " << coord.referencePixel() << endl;
    coord.setReferencePixel(Vector<double>(3,0.5));
    cout << "-- new reference pixel = " << coord.referencePixel() << endl;
    //
    cout << "--> SpatialCoordinate(Cartesian)" << endl;
    SpatialCoordinate coord_cartesian (CoordinateType::Cartesian);
    cout << "-- old reference pixel = " << coord_cartesian.referencePixel() << endl;
    coord_cartesian.setReferencePixel(Vector<double>(3,0.5));
    cout << "-- new reference pixel = " << coord_cartesian.referencePixel() << endl;
    //
    cout << "--> SpatialCoordinate(Spherical)" << endl;
    SpatialCoordinate coord_spherical (CoordinateType::Spherical);
    cout << "-- old reference pixel = " << coord_spherical.referencePixel() << endl;
    coord_spherical.setReferencePixel(Vector<double>(3,0.5));
    cout << "-- new reference pixel = " << coord_spherical.referencePixel() << endl;
    //
    cout << "--> SpatialCoordinate(Cylindrical)" << endl;
    SpatialCoordinate coord_cylindrical (CoordinateType::Cylindrical);
    cout << "-- old reference pixel = " << coord_cylindrical.referencePixel() << endl;
    coord_cylindrical.setReferencePixel(Vector<double>(3,0.5));
    cout << "-- new reference pixel = " << coord_cylindrical.referencePixel() << endl;
  } catch (std::string message) {
    cerr << message << endl;
    nofFailedTests++;
  }

  cout << "[7] Set reference value of coordinate ..." << endl;
  try {
    cout << "--> SpatialCoordinate(DirectionRadius)" << endl;
    SpatialCoordinate coord (CoordinateType::DirectionRadius);
    cout << "-- old reference value = " << coord.referenceValue() << endl;
    coord.setReferenceValue(Vector<double>(3,0.5));
    cout << "-- new reference value = " << coord.referenceValue() << endl;
    //
    cout << "--> SpatialCoordinate(Cartesian)" << endl;
    SpatialCoordinate coord_cartesian (CoordinateType::Cartesian);
    cout << "-- old reference value = " << coord_cartesian.referenceValue() << endl;
    coord_cartesian.setReferenceValue(Vector<double>(3,0.5));
    cout << "-- new reference value = " << coord_cartesian.referenceValue() << endl;
    //
    cout << "--> SpatialCoordinate(Spherical)" << endl;
    SpatialCoordinate coord_spherical (CoordinateType::Spherical);
    cout << "-- old reference value = " << coord_spherical.referenceValue() << endl;
    coord_spherical.setReferenceValue(Vector<double>(3,0.5));
    cout << "-- new reference value = " << coord_spherical.referenceValue() << endl;
    //
    cout << "--> SpatialCoordinate(Cylindrical)" << endl;
    SpatialCoordinate coord_cylindrical (CoordinateType::Cylindrical);
    cout << "-- old reference value = " << coord_cylindrical.referenceValue() << endl;
    coord_cylindrical.setReferenceValue(Vector<double>(3,0.5));
    cout << "-- new reference value = " << coord_cylindrical.referenceValue() << endl;
  } catch (std::string message) {
    cerr << message << endl;
    nofFailedTests++;
  }

  return nofFailedTests;
}

// -----------------------------------------------------------------------------

/*!
  \brief Test the conversion between pixel coordinates and world coordinates

  Test the methods which provide the conversion between pixel coordinates and 
  world coordinates -- \e toWorld and \e toPixel

  \return nofFailedTests -- The number of failed tests encountered within this
          function.
 */
int test_conversion ()
{
  cout << "\n[tSpatialCoordinate::test_conversion]\n" << endl;

  int nofFailedTests (0);
  int nofAxes (3);
  int nofSteps (5);
  Vector<double> pixel (nofAxes);
  Vector<double> world (nofAxes);

  cout << "[1] Conversion pixel->world for SpatialCoordinate(DirectionRadius) ..."
       << endl;
  try {
    SpatialCoordinate coord (CoordinateType::DirectionRadius);
    coord.summary();
    //
    for (int axis(0); axis<nofAxes; axis++) {
      pixel=0;
      for (int step(0); step<nofSteps; step++) {
	pixel(axis) = double(step);
	coord.toWorld (world,pixel);
	cout << "\t" << pixel << " -> " << world << endl;
      }
    }
  } catch (std::string message) {
    cerr << message << endl;
    nofFailedTests++;
  }

  cout << "[2] Conversion pixel->world for SpatialCoordinate(Cartesian) ..."
       << endl;
  try {
    SpatialCoordinate coord (CoordinateType::Cartesian);
    coord.summary();
    //
    for (int axis(0); axis<nofAxes; axis++) {
      pixel=0;
      for (int step(0); step<nofSteps; step++) {
	pixel(axis) = double(step);
	coord.toWorld (world,pixel);
	cout << "\t" << pixel << " -> " << world << endl;
      }
    }
  } catch (std::string message) {
    cerr << message << endl;
    nofFailedTests++;
  }

  cout << "[3] Conversion pixel->world for SpatialCoordinate(Spherical) ..."
       << endl;
  try {
    SpatialCoordinate coord (CoordinateType::Spherical);
    coord.summary();
    //
    for (int axis(0); axis<nofAxes; axis++) {
      pixel=0;
      for (int step(0); step<nofSteps; step++) {
	pixel(axis) = double(step);
	coord.toWorld (world,pixel);
	cout << "\t" << pixel << " -> " << world << endl;
      }
    }
  } catch (std::string message) {
    cerr << message << endl;
    nofFailedTests++;
  }

  cout << "[4] Conversion pixel->world for SpatialCoordinate(Cylindrical) ..."
       << endl;
  try {
    SpatialCoordinate coord (CoordinateType::Cylindrical);
    coord.summary();
    //
    for (int axis(0); axis<nofAxes; axis++) {
      pixel=0;
      for (int step(0); step<nofSteps; step++) {
	pixel(axis) = double(step);
	coord.toWorld (world,pixel);
	cout << "\t" << pixel << " -> " << world << endl;
      }
    }
  } catch (std::string message) {
    cerr << message << endl;
    nofFailedTests++;
  }

  return nofFailedTests;
}

// -----------------------------------------------------------------------------

/*!
  \brief Test passing of coordinates to a coordinate system object

  \return nofFailedTests -- The number of failed tests encountered within this
          function.
*/
int test_coordinateSystem ()
{
  cout << "\n[test_coordinateSystem]\n" << endl;

  int nofFailedTests (0);
  
  casa::CoordinateSystem csys;
  casa::ObsInfo obsInfo;

  obsInfo.setTelescope("LOFAR");
  obsInfo.setObserver("Lars Baehren");

  csys.setObsInfo (obsInfo);

  try {
    SpatialCoordinate coord;
    
    coord.toCoordinateSystem (csys,true);

    cout << "-- Telescope        = " << csys.obsInfo().telescope() << endl;
    cout << "-- Observer         = " << csys.obsInfo().observer()  << endl;
    cout << "-- nof. coordinates = " << csys.nCoordinates()        << endl;
    cout << "-- nof. pixel axes  = " << csys.nPixelAxes()          << endl;
    cout << "-- nof. world axes  = " << csys.nWorldAxes()          << endl;
  } catch (std::string message) {
    cerr << message << endl;
    nofFailedTests++;
  }

  return nofFailedTests;
}

// -----------------------------------------------------------------------------

int main ()
{
  int nofFailedTests (0);

  // Test for the constructor(s)
  nofFailedTests += test_SpatialCoordinate ();
  /* Test wrapping of parameter access and conversion functions */
  nofFailedTests += test_wcs ();
  /* Test conversion between pixel and world coordinates */
  nofFailedTests += test_conversion();
  /* Test export of the encapsulated coordinates to a coordinate system */
  nofFailedTests += test_coordinateSystem ();

  return nofFailedTests;
}
