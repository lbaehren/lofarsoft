/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2008                                                    *
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

#include <crtools.h>
#include <Coordinates/SpatialCoordinate.h>
#include <Imaging/GeomDelay.h>
#include <Utilities/TestsCommon.h>
#include "create_data.h"

// Namespace usage
using std::cout;
using std::endl;
using CR::CoordinateType;
using CR::GeomDelay;
using CR::SpatialCoordinate;

/*!
  \file tGeomDelay.cc

  \ingroup CR_Imaging

  \brief A collection of test routines for the GeomDelay class
 
  \author Lars B&auml;hren
 
  \date 2008/12/17
*/

// -----------------------------------------------------------------------------

/*!
  \brief Test constructors for a new GeomDelay object

  \return nofFailedTests -- The number of failed tests encountered within this
          function.
*/
int test_constructors ()
{
  cout << "\n[tGeomDelay::test_constructors]\n" << endl;

  int nofFailedTests (0);
  
  cout << "[1] Testing default constructor ..." << endl;
  try {
    GeomDelay delay;
    //
    delay.summary(); 
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  cout << "[2] Construction using (Matrix<double>,Matrix<double>) ..." << endl;
  try {
    Matrix<double> antPositions = get_antennaPositions();
    Matrix<double> skyPositions = get_skyPositions();
    GeomDelay delay (antPositions,
		     skyPositions);
    //
    delay.summary(); 
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  cout << "[3] Construction using (Vector<MVPosition>,Vector<MVPosition>) ..."
       << endl;
  try {
    Vector<casa::MVPosition> antPositions (1);
    Vector<casa::MVPosition> skyPositions (1);
    
    antPositions(0) = casa::MVPosition (100,100,0);
    skyPositions(0) = casa::MVPosition (casa::Quantity(100,"m"),
					casa::Quantity(0,"deg"),
					casa::Quantity(90,"deg"));
    GeomDelay delay (antPositions,
		     skyPositions);
    //
    delay.summary();
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}

// -----------------------------------------------------------------------------

/*!
  \brief Test the static functions provided by the GeomDelay class
  
  \return nofFailedTests -- The number of failed tests encountered within this
          function.
*/
int test_static_functions ()
{
  cout << "\n[tGeomDelay::test_static_functions]\n" << endl;
  
  int nofFailedTests (0);
  
  cout << "[1] delay(std::vector<double>,std::vector<double>)" << endl;
  try {
    std::vector<double> antPosition (3);
    std::vector<double> skyPosition (3);

    antPosition[0] = antPosition[1] = 100.0;
    antPosition[2] = 0.0;
    
    skyPosition[0] = 100;
    skyPosition[1] = 100;
    skyPosition[2] = 100;
    
    cout << "-- antenna position  = ["
	 << antPosition[0] << ","
	 << antPosition[1] << ","
	 << antPosition[2] << "]"
	 << endl;
    cout << "-- sky position      = ["
	 << skyPosition[0] << ","
	 << skyPosition[1] << ","
	 << skyPosition[2] << "]"
	 << endl;
    cout << "-- geometrical delay = " << GeomDelay::delays(antPosition,
							   skyPosition,
							   false)
	 << endl;
    cout << "-- geometrical delay = " << GeomDelay::delays(antPosition,
							   skyPosition,
							   true)
	 << endl;
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  } 
  
  cout << "[2] delay(casa::Vector<double>,casa::Vector<double>)" << endl;
  try {
    casa::Vector<double> antPosition (3);
    casa::Vector<double> skyPosition (3);
    
    antPosition(0) = antPosition(1) = 100.0;
    antPosition(2) = 0.0;
    
    skyPosition(0) = 100;
    skyPosition(1) = 100;
    skyPosition(2) = 100;
    
    cout << "-- antenna position  = " << antPosition << endl;
    cout << "-- sky position      = " << skyPosition << endl;
    cout << "-- geometrical delay = " << GeomDelay::delays(antPosition,
							   skyPosition,
							   false)
	 << endl;
    cout << "-- geometrical delay = " << GeomDelay::delays(antPosition,
							   skyPosition,
							   true)
	 << endl;
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  } 

  cout << "[3] delay(casa::Matrix<double>, casa::Matrix<double>)" << endl;
  try {
    uint nofPositions (3);
    casa::Matrix<double> antPosition;
    casa::Matrix<double> skyPosition (nofPositions,3);

    CR::test_antPositions(antPosition,CoordinateType::Cartesian);
    
    skyPosition(0,0) = 100;
    skyPosition(0,1) = 0;
    skyPosition(0,2) = 0;
    skyPosition(1,0) = 0;
    skyPosition(1,1) = 100;
    skyPosition(1,2) = 0;
    skyPosition(2,0) = 0;
    skyPosition(2,1) = 0;
    skyPosition(2,2) = 100;

    cout << "-- shape(antenna positions)  = " << antPosition.shape() << endl;
    cout << "-- shape(sky positions)      = " << skyPosition.shape() << endl;
    cout << "-- geometrical delays (near) = " << GeomDelay::delays(antPosition,
								   skyPosition,
								   false)
	 << endl;
    cout << "-- geometrical delays (far)  = " << GeomDelay::delays(antPosition,
								   skyPosition,
								   true)
	 << endl;
    
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  } 
  
  cout << "[3] delay(casa::Matrix<double>,casa::Matrix<double>,casa::Matrix<double>)"
       << endl;
  try {
    uint nofPositions (3);
    casa::Matrix<double> antPosition;
    casa::Matrix<double> skyPosition (nofPositions,3);
    casa::Matrix<double> delays;
    
    CR::test_antPositions(antPosition,CoordinateType::Cartesian);
    
    skyPosition(0,0) = 100;
    skyPosition(0,1) = 0;
    skyPosition(0,2) = 0;
    skyPosition(1,0) = 0;
    skyPosition(1,1) = 100;
    skyPosition(1,2) = 0;
    skyPosition(2,0) = 0;
    skyPosition(2,1) = 0;
    skyPosition(2,2) = 100;

    cout << "-- shape(antenna positions)  = " << antPosition.shape() << endl;
    cout << "-- shape(sky positions)      = " << skyPosition.shape() << endl;

    GeomDelay::delays(delays, antPosition, skyPosition, false);
    cout << "-- geometrical delays (near) = " << delays << endl;
    
    GeomDelay::delays(delays, antPosition, skyPosition, true);
    cout << "-- geometrical delays (far)  = " << delays << endl;
    
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  } 
  
  return nofFailedTests;
}

//_______________________________________________________________________________
//                                                                   test_methods

/*!
  \brief Test for the various methods of the class
  
  \return nofFailedTests -- The number of failed tests encountered within this
          function.
*/
int test_methods ()
{
  cout << "\n[tGeomDelay::test_methods]\n" << endl;

  int nofFailedTests (0);
  
  Vector<casa::MVPosition> antPositions (1);
  Vector<casa::MVPosition> skyPositions (1);
  
  antPositions(0) = casa::MVPosition (100,100,0);
  skyPositions(0) = casa::MVPosition (casa::Quantity(100,"m"),
				      casa::Quantity(0,"deg"),
				      casa::Quantity(90,"deg"));
  GeomDelay geomDelay (antPositions,
		       skyPositions);
  
  cout << "[1] Get antenna and sky positions ..." << endl;
  try {
    Matrix<double> antPos = geomDelay.antPositions();
    Matrix<double> skyPos = geomDelay.skyPositions();
    
    cout << "-- Antenna positions = " << antPos << endl;
    cout << "-- Sky positions     = " << skyPos << endl;
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  cout << "[2] Set antenna positions via Matrix<double> ..." << endl;
  try {
    Matrix<double> pos (2,3);
    
    cout << "-- cartesian coordinates ..." << std::endl;
    
    CR::test_antPositions(pos,CoordinateType::Cartesian);
    geomDelay.setAntPositions (pos,
			       CoordinateType::Cartesian,
			       false);
    cout << geomDelay.antPositions() << std::endl;
    
    cout << "-- spherical coordinates ..." << std::endl;
    
    CR::test_antPositions(pos,CoordinateType::Spherical);
    geomDelay.setAntPositions (pos,
			       CoordinateType::Spherical,
			       true);
    cout << geomDelay.antPositions() << std::endl;
    
    cout << "-- cylindrical coordinates ..." << std::endl;
    
    CR::test_antPositions(pos,CoordinateType::Cylindrical);
    geomDelay.setAntPositions (pos,
			       CoordinateType::Cylindrical,
			       true);
    cout << geomDelay.antPositions() << std::endl;

  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  cout << "[3] Set antenna positions via Vector<MVPosition> ..." << std::endl;
  try {
    antPositions.resize(3);
    antPositions(0) = casa::MVPosition (100,0,0);
    antPositions(1) = casa::MVPosition (0,100,0);
    antPositions(2) = casa::MVPosition (100,100,0);

    geomDelay.setAntPositions (antPositions);

    cout << geomDelay.antPositions() << std::endl;
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  cout << "[4] Set sky positions via Matrix<double> ..." << std::endl;
  try {
    Matrix<double> pos (2,3);

    cout << "-- cartesian coordinates ..." << std::endl;

    pos.row(0) = -100;
    pos.row(1) = +100;
    geomDelay.setSkyPositions (pos,
			       CoordinateType::Cartesian,
			       false);
    cout << geomDelay.skyPositions() << std::endl;

    cout << "-- spherical coordinates ..." << std::endl;

    pos(0,0) = 1;
    pos(0,1) = 90;
    pos(0,2) = 0;
    pos(1,0) = 1;
    pos(1,1) = 0;
    pos(1,2) = 90;

    geomDelay.setSkyPositions (pos,
			       CoordinateType::Spherical,
			       true);
    cout << geomDelay.skyPositions() << std::endl;

    cout << "-- cylindrical coordinates ..." << std::endl;

    pos(0,0) = 1;
    pos(0,1) = 90;
    pos(0,2) = 1;
    pos(1,0) = 1;
    pos(1,1) = 90;
    pos(1,2) = -1;

    geomDelay.setSkyPositions (pos,
			       CoordinateType::Cylindrical,
			       true);
    cout << geomDelay.skyPositions() << std::endl;

    cout << "-- (Direction,Radius) coordinates ..." << std::endl;

    pos(0,0) = 45;
    pos(0,1) = 45;
    pos(0,2) = 1;
    pos(1,0) = -45;
    pos(1,1) = -45;
    pos(1,2) = 1;

    geomDelay.setSkyPositions (pos,
			       CoordinateType::DirectionRadius,
			       true);
    cout << geomDelay.skyPositions() << std::endl;

  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  cout << "[5] Set single sky position via Vector<double> ..." << std::endl;
  try {
    Vector<double> pos(3);

    pos(0) = 10;
    pos(1) = 20;
    pos(2) = 30;
    
    geomDelay.setSkyPositions (pos,
			       CoordinateType::Cartesian,
			       true);
    cout << geomDelay.skyPositions() << std::endl;
    
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  cout << "[6] Adjust settings for the buffering of the delay values ..." << endl;
  try {
    cout << "--> Internal settings before changing buffer settings:" << endl;
    geomDelay.summary();
    //
    geomDelay.bufferDelays (true);
    //
    cout << "--> Internal settings after switching on buffering:" << endl;
    geomDelay.summary();
    //
    geomDelay.bufferDelays (false);
    //
    cout << "--> Internal settings after switching off buffering:" << endl;
    geomDelay.summary();
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  cout << "[7] Test retrieval of the delay values ..." << endl;
  try {
    Matrix<double> pos (2,3);
    Matrix<double> delays;
    //
    pos(0,0) = 1;
    pos(0,1) = 90;
    pos(0,2) = 0;
    pos(1,0) = 1;
    pos(1,1) = 0;
    pos(1,2) = 90;
    geomDelay.setSkyPositions (pos,
			       CoordinateType::Spherical,
			       true);
    //
    cout << "-- nof. antenna pos = " << geomDelay.nofAntPositions() << endl;
    cout << "-- nof. sky pos     = " << geomDelay.nofSkyPositions() << endl;
    cout << "-- buffer delays    = " << geomDelay.bufferDelays() << endl;
    cout << "-- shape(delays)    = " << geomDelay.shape()        << endl;
    //
    delays = geomDelay.delays();
    cout << "-- delays by return = " << delays << endl;
    //
    delays = 1;
    geomDelay.delays(delays);
    cout << "-- delays by ref    = " << delays << endl;
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}
  
// -----------------------------------------------------------------------------

int main ()
{
  int nofFailedTests (0);

  // Test for the constructor(s)
  nofFailedTests += test_constructors();
  // Test for the static functions
  nofFailedTests += test_static_functions();
  // Test for the various methods of the class
  nofFailedTests += test_methods();

  return nofFailedTests;
}
