/***************************************************************************
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

/* $Id$*/

#include <iostream>
#include <fstream>
#include <string>

#include <casa/Arrays/IPosition.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/Vector.h>

#include "create_data.h"
#include <Imaging/GeometricalDelay.h>

using std::cout;
using std::endl;
using CR::GeometricalDelay;
using CR::L2Norm;


//! Speed of light (Source: Wikipedia)
const double lightspeed = 299792458;

/*!
  \file tGeometricalDelay.cc

  \ingroup Imaging

  \brief A collection of test routines for GeometricalDelay
 
  \author Lars B&auml;hren
 
  \date 2007/01/15
*/

// -----------------------------------------------------------------------------

/*!
  \brief Export the positions and delays to an ASCII table

  \param antPositions -- [antenna,3] Three-dimensional positions of the antennas.
  \param skyPositions -- [position,3] Three-dimensional positions towards which
         the delays are computed.
  \param delays       -- [antenna,sky] Array storing the values of the
         geometrical delays.
  \param filename     -- Name of the file to which the data will be written
*/
void export_delays (casa::Matrix<double> const &antPositions,
		    casa::Matrix<double> const &skyPositions,
		    casa::Matrix<double> const &delays,
		    std::string const &filename="tGeometricalDelay.data")
{
  int numAntenna(0);
  int numSky (0);
  int coord(0);
  casa::IPosition shape(delays.shape());

  std::ofstream outfile (filename.c_str(),std::ios::out);

  for (numAntenna=0; numAntenna<shape(0); numAntenna++) {
    for (numSky=0; numSky<shape(1); numSky++) {
      // export antenna position
      outfile << antPositions(numAntenna,0) << "  "
	      << antPositions(numAntenna,1) << "  "
	      << antPositions(numAntenna,2) << "  ";
      // export sky position
      outfile << skyPositions(numSky,0) << "  "
	      << skyPositions(numSky,1) << "  "
	      << skyPositions(numSky,2) << "  ";
      // export geometrical delay
      outfile << delays(numAntenna,numSky) << std::endl;
    }
    outfile << std::endl;
  }
  
  outfile.close();
}

// -----------------------------------------------------------------------------

/*!
  \brief Fundamental testing on the different formulae for delay computation

  \return nofFailedTests -- The number of failed tests.
*/
int test_formula ()
{
  cout << "\n[test_formula] (CASA arrays)\n" << endl;
  
  int nofFailedTests (0);
  
  int nofCoordinates (3);
  casa::Matrix<double> antPositions = get_antennaPositions();
  casa::Matrix<double> skyPositions = get_skyPositions();
  double delay (.0);
  
  /*!
    Standard version for the computation of the delay
   */
  cout << "[1] Far-field geometry" << endl;
  try {
    double scalarProduct (.0);

    for (int n(0); n<nofCoordinates; n++) {
      scalarProduct += (antPositions(1,n)-antPositions(0,n))*skyPositions(2,n);
    }
    delay = scalarProduct/lightspeed;
    
    cout << "delay(FF)     = " << delay         << endl;
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}

// -----------------------------------------------------------------------------

/*!
  \brief Test constructors for a new GeometricalDelay object

  \return nofFailedTests -- The number of failed tests.
*/
int test_GeometricalDelay ()
{
  cout << "\n[test_GeometricalDelay] (CASA arrays)\n" << endl;

  int nofFailedTests (0);

  cout << "[1] Testing default constructor ..." << endl;
  try {
    GeometricalDelay delay;
    delay.summary();
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  cout << "[2] Testing simplest argumented constructor ..." << endl;
  try {
    casa::Matrix<double> antPositions = get_antennaPositions();
    casa::Matrix<double> skyPositions = get_skyPositions();
    // construct new object
    GeometricalDelay delay (antPositions,
			    skyPositions);
    delay.summary();
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  cout << "[3] Testing argumented constructor specifying coordinates ..." << endl;
  try {
    // retrieve arrays with the positions
    casa::Matrix<double> antPositions = get_antennaPositions();
    casa::Matrix<double> skyPositions = get_skyPositions();
    cout << "-- Antenna positions in cartesian coordinates ..." << std::endl;
    GeometricalDelay delay1 (antPositions,
			     CR::Cartesian,
			     skyPositions,
			     CR::Cartesian);
    cout << "-- Antenna positions in cylindrical coordinates ..." << std::endl;
    antPositions = 0.0;
    antPositions.column(0) = 100.0;
    antPositions(1,1) = CR::deg2rad(90);
    antPositions(2,1) = CR::deg2rad(180);
    antPositions(3,1) = CR::deg2rad(270);
    GeometricalDelay delay2 (antPositions,
			     CR::Cylindrical,
			     skyPositions,
			     CR::Cartesian);
    cout << "-- Antenna positions in spherical coordinates ..." << std::endl;
    antPositions = 0.0;
    antPositions.column(0) = 100.0;
    antPositions(1,1) = CR::deg2rad(90);
    antPositions(2,1) = CR::deg2rad(180);
    antPositions(3,1) = CR::deg2rad(270);
    antPositions.column(2) = CR::deg2rad(90);
    GeometricalDelay delay3 (antPositions,
			     CR::Spherical,
			     skyPositions,
			     CR::Cartesian);
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  cout << "[4] Testing fully argumented constructor ..." << endl;
  try {
    uint nofAntennas (4);
    bool bufferDelays (false);
    bool antennaIndexFirst (false);
    // retrieve arrays with the positions
    std::cout << "-- getting antenna positions ..." << std::endl;
    casa::Matrix<double> antPositions = get_antennaPositions(nofAntennas,
							     antennaIndexFirst);
    std::cout << "-- getting sky positions ..." << std::endl;
    casa::Matrix<double> skyPositions = get_skyPositions();
    // create new object
    GeometricalDelay delay (antPositions,
			    CR::Cartesian,
			    skyPositions,
			    CR::Cartesian,
			    bufferDelays,
			    antennaIndexFirst);    
    // provide a summary
    delay.summary();
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  cout << "[5] Testing copy constructor ..." << endl;
  try {
    casa::Matrix<double> antPositions = get_antennaPositions();
    casa::Matrix<double> skyPositions = get_skyPositions();
    // construct new object
    GeometricalDelay delay (antPositions,
			    skyPositions);
    delay.summary();

    GeometricalDelay delay_copy (delay);
    delay_copy.summary();
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}

// -----------------------------------------------------------------------------

/*!
  \brief Test setting and retrieving of the antenna positions

  \return nofFailedTests -- The number of failed tests.
*/
int test_antennaPositions ()
{
  cout << "\n[test_antennaPositions]\n" << endl;
  
  int nofFailedTests (0);
  bool status (true);
  bool antennaIndexFirst (true);
  bool bufferDelays (false);
  uint nofAntennas (9);

  // create GeometricalDelay object to work with
  
  CR::GeometricalDelay delay;

  cout << "[1] Setting antenna positions with cartesian coordinates " << endl;
  try {
    Matrix<double> pos (nofAntennas,3,0.0);
    // set up positions
    pos(0,0) = -100;  pos(0,1) = 100;
    pos(1,0) = 0;     pos(1,1) = 100;
    pos(2,0) = 100;   pos(2,1) = 100;
    pos(3,0) = -100;  pos(3,1) = 0;
    pos(4,0) = 0;     pos(4,1) = 0;
    pos(5,0) = 100;   pos(5,1) = 0;
    pos(6,0) = -100;  pos(6,1) = -100;
    pos(7,0) = 0;     pos(7,1) = -100;
    pos(8,0) = 100;   pos(8,1) = -100;
    // summary before changes
    delay.summary();
    // set new antenna positions
    status = delay.setAntennaPositions (pos,
					bufferDelays);
    // summary after changes
    delay.summary();
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  cout << "[2] Setting antenna positions with cylindrical coordinates" << endl;
  try {
    Matrix<double> pos (nofAntennas,3,0.0);
    double incr (360/nofAntennas);
    // set up positions
    for (uint antenna(0); antenna<nofAntennas; antenna++) {
      pos(antenna,0) = 100;
      pos(antenna,1) = incr*antenna;
    }
    // summary before changes
    delay.summary();
    // set new antenna positions
    antennaIndexFirst = true;
    bufferDelays      = false;
    status = delay.setAntennaPositions (pos,
					antennaIndexFirst,
					CR::Cylindrical,
					true,
					bufferDelays);
    // summary after changes
    delay.summary();
    cout << delay.antennaPositions() << std::endl;
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  return nofFailedTests;
}

// -----------------------------------------------------------------------------

/*!
  \brief Test setting and retrieving of the sky positions

  \return nofFailedTests -- The number of failed tests.
*/
int test_skyPositions ()
{
  cout << "\n[test_skyPositions]\n" << endl;
  
  int nofFailedTests (0);
  GeometricalDelay delay;
  int nofSkyPositions (5);
  bool status (true);

  cout << "[1] Setting sky positions with cartesian coordinates, Matrix<double>"
	    << endl;
  try {
    uint nofPositions (9);
    Matrix<double> pos (nofPositions,3,100.0);
    // set up positions
    pos(0,0) = -100;  pos(0,1) = 100;
    pos(1,0) = 0;     pos(1,1) = 100;
    pos(2,0) = 100;   pos(2,1) = 100;
    pos(3,0) = -100;  pos(3,1) = 0;
    pos(4,0) = 0;     pos(4,1) = 0;
    pos(5,0) = 100;   pos(5,1) = 0;
    pos(6,0) = -100;  pos(6,1) = -100;
    pos(7,0) = 0;     pos(7,1) = -100;
    pos(8,0) = 100;   pos(8,1) = -100;
    // set new antenna positions
    status = delay.setSkyPositions (pos,
				    false);
    // summary after changes
    delay.summary();
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }  

  cout << "[2] Setting sky positions with cylindrical coordinates, Matrix<double>"
       << endl;
  try {
    uint nofPositions (9);
    double incr (360/nofPositions);
    Matrix<double> pos (nofPositions,3,100.0);
    // set up positions
    for (uint position(0); position<nofPositions; position++) {
      pos(position,1) = incr*position;
    }
    // set new sky positions
    status = delay.setSkyPositions (pos,
				    CR::Cylindrical,
				    true,
				    false);
    // summary after changes
    delay.summary();
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  cout << "[3] Setting sky positions with Az-El-Radius coordinates, Matrix<double>"
	    << endl;
  try {
    uint nofPositions (9);
    Matrix<double> pos (nofPositions,3,100.0);
    // set up positions
    pos(0,0) = -10;  pos(0,1) = 10;
    pos(1,0) = 0;    pos(1,1) = 10;
    pos(2,0) = 10;   pos(2,1) = 10;
    pos(3,0) = -10;  pos(3,1) = 0;
    pos(4,0) = 0;    pos(4,1) = 0;
    pos(5,0) = 10;   pos(5,1) = 0;
    pos(6,0) = -10;  pos(6,1) = -10;
    pos(7,0) = 0;    pos(7,1) = -10;
    pos(8,0) = 10;   pos(8,1) = -10;
    //
    cout << pos << endl;
    // set new antenna positions
    status = delay.setSkyPositions (pos,
				    CR::AzElRadius,
				    true,
				    false);
    // summary after changes
    delay.summary();
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  cout << "[3] Set new values via (Vector,Vector,Vector) ..." << endl;
  try {
    casa::Vector<double> xValues (2);
    casa::Vector<double> yValues (2);
    casa::Vector<double> zValues (3);

    indgen (xValues);
    indgen (yValues);
    indgen (zValues);

    status  = delay.setSkyPositions(xValues,
				    yValues,
				    zValues,
				    CR::Cartesian);
    
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }  

  cout << "[4] Set new values via (Matrix,Vector) ..." << endl;
  try {
    casa::Matrix<double> xyValues (2,2);
    casa::Vector<double> zValues (3);
    casa::Vector<int> axisOrder (3);

    indgen (xyValues);
    indgen (zValues);
    indgen (axisOrder);

    // default axis ordering: [0,1,2]

    status  = delay.setSkyPositions(xyValues,
				    zValues,
				    axisOrder,
				    CR::Cartesian);
    
    if (status) {
      cout << "-- Axis ordering = " << axisOrder << endl;
      cout << "-- Sky positions = " << delay.skyPositions() << endl;
    }

    // inverted axis ordering: [2,1,0]

    axisOrder(0) = 2;
    axisOrder(1) = 1;
    axisOrder(2) = 0;
    
    status  = delay.setSkyPositions(xyValues,
				    zValues,
				    axisOrder,
				    CR::Cartesian);
    
    if (status) {
      cout << "-- Axis ordering = " << axisOrder << endl;
      cout << "-- Sky positions = " << delay.skyPositions() << endl;
    }

    // axis ordering: [2,0,1]

    axisOrder(0) = 2;
    axisOrder(1) = 0;
    axisOrder(2) = 1;
    
    status  = delay.setSkyPositions(xyValues,
				    zValues,
				    axisOrder,
				    CR::Cartesian);
    
    if (status) {
      cout << "-- Axis ordering = " << axisOrder << endl;
      cout << "-- Sky positions = " << delay.skyPositions() << endl;
    }

  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  return nofFailedTests;
}

// -----------------------------------------------------------------------------

/*!
  \brief Test actual computation of the geometrical delay

  \return nofFailedTests -- Number of failed tests

*/
int test_delayComputation ()
{
  cout << "\n[test_delayComputation]\n" << endl;

  int nofFailedTests (0);
  bool antennaIndexFirst (true);
  casa::Matrix<double> antPositions = get_antennaPositions();
  casa::Matrix<double> skyPositions = get_skyPositions();
  
  // get the default values for antenna and sky positions

  // adjust the sky positions
  {
    uint nofPositions (200);
    double incr (100);

    skyPositions.resize (nofPositions,3);
    skyPositions.column(0) = 45;
    skyPositions.column(1) = 45;

    for (uint radius (0); radius<nofPositions; radius++) {
      skyPositions(radius,2) = radius*incr;
    }
    
    // create GeometricalDelay object
    GeometricalDelay delay (antPositions,
			    CR::Cartesian,
			    skyPositions,
			    CR::Spherical);
    delay.summary();

    /* Set geometry to near-field */
    delay.setNearField (true,
			false);
    /* Export positions and computed delays */
    export_delays (delay.antennaPositions(),
		   delay.skyPositions(),
		   delay.delays(),
		   "tGeometricalDelay-near.data");

    // retrieve the delay values for the far-field geometry
    std::cout << "-- switching to far-field geometry" << std::endl;
    delay.setNearField (false,
			false);
    /* Export positions and computed delays */
    export_delays (delay.antennaPositions(),
		   delay.skyPositions(),
		   delay.delays(),
		   "tGeometricalDelay-far.data");

  }
  
  return nofFailedTests;
}

// -----------------------------------------------------------------------------

/*!
  \brief Main function 

  \return nofFailedTests -- The number of failed tests
*/
int main ()
{
  int nofFailedTests (0);
  
//   nofFailedTests += test_formula ();

  // Test for the constructor(s)
  nofFailedTests += test_GeometricalDelay ();
  // Test manipulation of the antenna positions
  nofFailedTests += test_antennaPositions ();
  // Test manipulation of the sky positions
//   nofFailedTests += test_skyPositions ();
  // Test for the computation of the actual geometrical delay
  nofFailedTests += test_delayComputation ();
  
  return nofFailedTests;
}
