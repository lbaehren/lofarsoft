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

#ifdef HAVE_CASA
#include <casa/Arrays/IPosition.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/Vector.h>
#else
#ifdef HAVE_BLITZ
#include <blitz/array.h>
using blitz::Array;
using blitz::Range;
#endif
#endif

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

#ifdef HAVE_CASA
/*!
  \brief Get arrays with the antenna and sky positions

  \retval skyPositions     -- Pointing positions on the sky
  \retval antPositions     -- Antenna positions
  \param antennaIndexFirst -- How to organize the matrix with the antenna
         positions? If <tt>antennaIndexFirst=true</tt> then the positions are
	 organized as <tt>[antenna,3]</tt>, otherwise <tt>[3,antenna]</tt>
*/
void get_positions (casa::Matrix<double> &skyPositions,
		    casa::Matrix<double> &antPositions,
		    bool const &antennaIndexFirst)
{
  int nofCoordinates (3);
  int nofAntennas (4);
  int nofDirections (3);

  // Antenna positions
  
  antPositions.resize(nofAntennas,nofCoordinates);
  antPositions = 0.0;
  
  // [100,0,0]
  antPositions(0,0) = 100;
  // [0,100,0]
  antPositions(1,1) = 100;
  // [-100,0,0]
  antPositions(2,0) = -100;
  // [0,-100,0]
  antPositions(3,1) = -100;
  
  if (!antennaIndexFirst) {
    Matrix<double> tmp = transpose(antPositions);
    //
    antPositions.resize (tmp.shape());
    antPositions = tmp;
  }
  
  // Pointing directions (sky positions)
  
  skyPositions.resize(nofDirections,nofCoordinates);

  skyPositions            = 0.0;
  skyPositions.diagonal() = 100.0;

}
#else
#ifdef HAVE_BLITZ
void get_positions (blitz::Array<double,2> &skyPositions,
		    blitz::Array<double,2> &antPositions,
		    bool const &antennaIndexFirst)
{
  int nofCoordinates (3);
  int nofAntennas (4);
  int nofDirections (3);

  // Antenna positions

  if (antennaIndexFirst) {
    antPositions.resize(nofAntennas,nofCoordinates);
    antPositions = 0.0;

    // [100,0,0]
    antPositions(0,0) = 100;
    // [0,100,0]
    antPositions(1,1) = 100;
    // [-100,0,0]
    antPositions(2,0) = -100;
    // [0,-100,0]
    antPositions(3,1) = -100;
  } else {
    antPositions.resize(nofCoordinates,nofAntennas);
    antPositions = 0.0;

    // [100,0,0]
    antPositions(0,0) = 100;
    // [0,100,0]
    antPositions(1,1) = 100;
    // [-100,0,0]
    antPositions(0,2) = -100;
    // [0,-100,0]
    antPositions(1,3) = -100;
  }

  // Pointing directions (sky positions)
  
  skyPositions.resize(nofDirections,nofCoordinates);

  skyPositions            = 0.0;
  skyPositions.diagonal() = 100.0;

}
#endif
#endif

// -----------------------------------------------------------------------------

/*!
  \brief Fundamental testing on the different formulae for delay computation

  \return nofFailedTests -- The number of failed tests.
*/
#ifdef HAVE_CASA
int test_formula ()
{
  cout << "\n[test_formula] (CASA arrays)\n" << endl;
  
  int nofFailedTests (0);
  
  int nofCoordinates (3);
  casa::Matrix<double> antPositions;
  casa::Matrix<double> skyPositions;
  double delay (.0);
  
  get_positions (skyPositions, antPositions,true);

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
#else
#ifdef HAVE_BLITZ
int test_formula ()
{
  cout << "\n[test_formula] (Blitz++ arrays)\n" << endl;

  int nofFailedTests (0);

  int nofCoordinates (3);
  Array<double,1> skyPositions (nofCoordinates);
  Array<double,2> antPositions (2,nofCoordinates);
  double delay (.0);

  antPositions = -100.0, 0.0, 0.0, 100.0, 0.0, 0.0;
  skyPositions = 100.0, 100.0, 100.0;

  cout << "sky positions = " << skyPositions  << endl;
  cout << "ant positions = " << antPositions  << endl;

  /*!
    Standard version for the computation of the delay
   */
  cout << "[1] Far-field geometry" << endl;
  try {
    double scalarProduct (.0);

    for (int n(0); n<nofCoordinates; n++) {
      scalarProduct += (antPositions(1,n)-antPositions(0,n))*skyPositions(n);
    }
    delay = scalarProduct/lightspeed;
    
    cout << "delay(FF)     = " << delay         << endl;
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  /*!
    Computation of the delay based on the full 3-dim geometry
  */
  cout << "\n[2] Compute delay for full geometry\n" << endl;
  try {
    Array<double,1> diff0 (nofCoordinates);
    Array<double,1> diff1 (nofCoordinates);
    
    for (int n(0); n<nofCoordinates; n++) {
      diff0(n) = skyPositions(n)-antPositions(0,n);
      diff1(n) = skyPositions(n)-antPositions(1,n);
    }
    delay = (L2Norm(diff1)-L2Norm(diff0))/lightspeed;
    
    cout << "delay(NF)     = " << delay         << endl;
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  /*
    Variation of the source position
  */
  cout << "\n[3] Variation of the source position\n" << endl;
  try {
    double zmin (100.0);
    double zmax (1000.0);
    double zstep (100.0);
    Array<double,1> diff0 (nofCoordinates);
    Array<double,1> diff1 (nofCoordinates);

    // go through the distance steps
    for (double z(zmin); z<=zmax; z+=zstep) {
      // set the z-coordinate of the sky position
      skyPositions(2) = z;
      // compute the geometrical delay
      for (int n(0); n<nofCoordinates; n++) {
	diff0(n) = skyPositions(n)-antPositions(0,n);
	diff1(n) = skyPositions(n)-antPositions(1,n);
      }
      delay = (L2Norm(diff1)-L2Norm(diff0))/lightspeed;
      // dispay the result
      cout << "\t" << z << "\t" << delay << endl;
    }
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}
#endif
#endif

// -----------------------------------------------------------------------------

/*!
  \brief Test constructors for a new GeometricalDelay object

  \return nofFailedTests -- The number of failed tests.
*/
#ifdef HAVE_CASA
int test_GeometricalDelay ()
{
  cout << "\n[test_GeometricalDelay] (CASA arrays)\n" << endl;

  int nofFailedTests (0);
  casa::Matrix<double> antPositions;
  casa::Matrix<double> skyPositions;

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
    // retrieve arrays with the positions
    get_positions (skyPositions,
		   antPositions,
		   true);
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
    get_positions (skyPositions,
		   antPositions,
		   true);
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
    bool bufferDelays (false);
    bool antennaIndexFirst (false);
    // retrieve arrays with the positions
    get_positions (skyPositions,
		   antPositions,
		   antennaIndexFirst);
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
    get_positions (skyPositions,
		   antPositions,
		   true);
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
#else
#ifdef HAVE_BLITZ
int test_GeometricalDelay ()
{
  cout << "\n[test_GeometricalDelay] (Blitz++ arrays)\n" << endl;

  int nofFailedTests (0);
  uint nofCoordinates (3);
  Array<double,2> antPositions (2,nofCoordinates);
  Array<double,2> skyPositions (3,nofCoordinates);
  
  antPositions = -100.0, 0.0, 0.0, 100.0, 0.0, 0.0;
  skyPositions = 100.0, 0.0, 0.0, 0.0, 100.0, 0.0, 0.0, 0.0, 100.0;
  
  cout << "[1] Testing default constructor ..." << endl;
  try {
    GeometricalDelay delay;
    delay.summary();
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  cout << "[2] Testing argumented constructor ..." << endl;
  try {
    GeometricalDelay delay (antPositions,
			    skyPositions);
    delay.summary();
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  cout << "[3] Testing copy constructor ..." << endl;
  try {
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
#endif
#endif

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

#ifdef HAVE_CASA  
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
#endif

  return nofFailedTests;
}

// -----------------------------------------------------------------------------

/*!
  \brief Test actual computation of the geometrical delay

  \return nofFailedTests -- Number of failed tests

*/
#ifdef HAVE_CASA
int test_delayComputation ()
{
  cout << "\n[test_delayComputation]\n" << endl;

  int nofFailedTests (0);
  bool antennaIndexFirst (true);
  casa::Matrix<double> antPositions;
  casa::Matrix<double> skyPositions;

  // get the default values for antenna and sky positions

  get_positions (skyPositions,
		 antPositions,
		 antennaIndexFirst);

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

    // retrieve the delay values for the near-field geometry
    delay.setNearField (true,
			false);
    casa::Matrix<double> delaysNearField = delay.delays();

    // retrieve the delay values for the far-field geometry
    std::cout << "-- switching to far-field geometry" << std::endl;
    delay.setNearField (false,
			false);
    delay.summary();
    casa::Matrix<double> delaysFarField = delay.delays();
    
    // export the delays for later plotting
    std::ofstream outfile ("delays.data",std::ios::out);
    for (uint coord (0); coord<nofPositions; coord++) {
      outfile << skyPositions(coord,2)
	      << "\t" << delaysNearField(0,coord)
	      << "\t" << delaysFarField(0,coord)
	      << endl;
    }
    outfile.close();
  }
  
  return nofFailedTests;
}
#else
#ifdef HAVE_BLITZ
int test_delayComputation ()
{
  cout << "\n[test_delayComputation]\n" << endl;

  int nofFailedTests (0);
  uint nofCoordinates (3);
  int nofAntennas (3);
  int nofPositions (1000);
  double offset (1000);
  double stepwidth (100);
  Array<double,2> antPositions (nofAntennas,nofCoordinates);
  Array<double,2> skyPositions (nofPositions,nofCoordinates);
  
  antPositions = 100,100,0, 200,200,0,-100,-100,0;

  // assign the values for the sky positions
  for (int n (0); n<nofPositions; n++) {
    skyPositions(n,0) = 0.0;
    skyPositions(n,1) = 0.0;
    skyPositions(n,2) = offset+stepwidth*n;
  }

  GeometricalDelay delay (antPositions,
			  skyPositions);
  delay.summary();
  
  Array<double,2> delays = delay.delays();

  // export the computed values
  std::ofstream outfile;
  outfile.open("delays.data");
  for (int n (0); n<nofPositions; n++) {
    outfile << "\t" << n
	    << "\t" << skyPositions(n,2)
	    << "\t" << delays(0,n)       // delay for antenna 1 @ [100,100,0]
	    << "\t" << delays(1,n)       // delay for antenna 2 @ [200,200,0]
	    << "\t" << delays(2,n)       // delay for antenna 3 @ [-100,-100,0]
	    << endl;
  }
  outfile.close();
  
  return nofFailedTests;
}
#endif
#endif

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
  nofFailedTests += test_skyPositions ();
  // Test for the computation of the actual geometrical delay
  nofFailedTests += test_delayComputation ();
  
  return nofFailedTests;
}
