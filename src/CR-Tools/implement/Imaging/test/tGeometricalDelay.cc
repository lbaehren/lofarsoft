/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
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

#include <iostream>
#include <fstream>
#include <string>

#include <casa/Arrays/IPosition.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/Vector.h>

#include "create_data.h"
#include <Imaging/GeometricalDelay.h>
#include <Math/VectorNorms.h>

using std::cerr;
using std::cout;
using std::endl;
using CR::GeometricalDelay;
using CR::L2Norm;
using CR::lightspeed;

/*!
  \file tGeometricalDelay.cc

  \ingroup CR_Imaging

  \brief A collection of test routines for the GeometricalDelay class
 
  \author Lars B&auml;hren
 
  \date 2007/01/15
*/

//______________________________________________________________________________
//                                                                 export_delays

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

//______________________________________________________________________________
//                                                                 compute_delay

/*!
  \brief Compute the geometrical delay

  \param pos_ant -- Antenna position, \f$\vec x\f$
  \param skyPosition -- Sky position, \f$\vec\rho\f$.
  \param farField    -- Compute geometrical delay for the far-field limit? By
         default this is not the case.
  \param normalize -- Normalize the sky position vector? This option is available
         only in the far-field case, where we can consider the beam pointing
	 towards a location on a sphere of unit radius. Setting
	 <tt>normalize=true</tt> will use
	 \f$ \hat \rho = \vec \rho / |\vec \rho| \f$ in place of the input vector
	 \f$ \vec \rho \f$.

  \return delay -- The value of the geometrical delay. For the far-field the 
          geometrical delay is computed through
          \f[ \tau = \frac{1}{c} \langle \vec \rho , \vec x \rangle \f]
	  whereas for the near-field we employe the full spatial information
	  on antenna position and beam pointing position:
	  \f[ \tau = \frac{1}{c} \Bigl( |\vec \rho - \vec x| - |\vec \rho| \Bigr) \f]
*/
double compute_delay (casa::Vector<double> const &pos_ant,
		      casa::Vector<double> const &skyPosition,
		      bool const &farField=false,
		      bool const &normalize=false)
{
  double delay (0);
  
  if (farField==true) {
    if (normalize==true) {
      casa::Vector<double> direction (3);
      CR::normalize (direction,skyPosition);
      delay = -(direction(0)*pos_ant(0)+direction(1)*pos_ant(1)+direction(2)*pos_ant(2))/lightspeed;
    } else {
      delay = -(skyPosition(0)*pos_ant(0)+skyPosition(1)*pos_ant(1)+skyPosition(2)*pos_ant(2))/lightspeed;
    }
  } else {
    casa::Vector<double> diff = skyPosition-pos_ant;
    delay = (CR::L2Norm(diff)-CR::L2Norm(skyPosition))/lightspeed;
  }
  
  return delay;
}

//______________________________________________________________________________
//                                                                  test_formula

/*!
  \brief Fundamental testing on the different formulae for delay computation

  \return nofFailedTests -- The number of failed tests.
*/
int test_formula ()
{
  cout << "\n[test_formula]\n" << endl;
  
  int nofFailedTests (0);
  
  int nofAxes (3);
  casa::Vector<double> pos_ant (nofAxes,0.0);
  casa::Vector<double> skyPosition (nofAxes,0.0);
  double delay_nearField (.0);
  double delay_farField (.0);
  double delay_farFieldN (.0);

  // sampling of the parameter space
  uint nofAntPositions (50);
  uint nofSkyPositions (100);
  double incrAnt (10);
  double incrSky (200);

  // feedback on used parameters
  cout << "-- nof. antenna positions     = " << nofAntPositions << endl;
  cout << "-- antenna position increment = " << incrAnt << endl;
  cout << "-- nof. sky positions         = " << nofSkyPositions << endl;
  cout << "-- sky position increment     = " << incrSky << endl;

  // export of the computation results
  std::ofstream outfile ("geometricalDelay.data");

  for (uint antpos (0); antpos<nofAntPositions; antpos++) {
    pos_ant(0) = antpos*incrAnt;
    for (uint skypos(0); skypos<nofSkyPositions; skypos++) {
      skyPosition = skypos*incrSky;
      // compute the delay
      delay_nearField = compute_delay(pos_ant,skyPosition,false,false);
      delay_farField  = compute_delay(pos_ant,skyPosition,true,false);
      delay_farFieldN = compute_delay(pos_ant,skyPosition,true,true);
      // export the computation results
      outfile << pos_ant << "\t"
	      << CR::L2Norm(pos_ant) << "\t"
	      << skyPosition << "\t"
	      << CR::L2Norm(skyPosition) << "\t"
	      << delay_nearField << "\t"
	      << delay_farField  << "\t"
	      << delay_farFieldN
	      << endl;
    }
    outfile << endl;
  }

  outfile.close();
  
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
			     CR::CoordinateType::Cartesian,
			     skyPositions,
			     CR::CoordinateType::Cartesian);
    cout << "-- Antenna positions in cylindrical coordinates ..." << std::endl;
    antPositions = 0.0;
    antPositions.column(0) = 100.0;
    antPositions(1,1) = CR::deg2rad(90);
    antPositions(2,1) = CR::deg2rad(180);
    antPositions(3,1) = CR::deg2rad(270);
    GeometricalDelay delay2 (antPositions,
			     CR::CoordinateType::Cylindrical,
			     skyPositions,
			     CR::CoordinateType::Cartesian);
    cout << "-- Antenna positions in spherical coordinates ..." << std::endl;
    antPositions = 0.0;
    antPositions.column(0) = 100.0;
    antPositions(1,1) = CR::deg2rad(90);
    antPositions(2,1) = CR::deg2rad(180);
    antPositions(3,1) = CR::deg2rad(270);
    antPositions.column(2) = CR::deg2rad(90);
    GeometricalDelay delay3 (antPositions,
			     CR::CoordinateType::Spherical,
			     skyPositions,
			     CR::CoordinateType::Cartesian);
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
    cout << "-- getting antenna positions ..." << std::endl;
    casa::Matrix<double> antPositions = get_antennaPositions(nofAntennas,
							     antennaIndexFirst);
    cout << "-- getting sky positions ..." << std::endl;
    casa::Matrix<double> skyPositions = get_skyPositions();
    // create new object
    GeometricalDelay delay (antPositions,
			    CR::CoordinateType::Cartesian,
			    skyPositions,
			    CR::CoordinateType::Cartesian,
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
					CR::CoordinateType::Cylindrical,
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
				    CR::CoordinateType::Cylindrical,
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
				    CR::CoordinateType::AzElRadius,
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
				    CR::CoordinateType::Cartesian);
    
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
				    CR::CoordinateType::Cartesian);
    
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
				    CR::CoordinateType::Cartesian);
    
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
				    CR::CoordinateType::Cartesian);
    
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
			    CR::CoordinateType::Cartesian,
			    skyPositions,
			    CR::CoordinateType::Spherical);
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
    cout << "-- switching to far-field geometry" << std::endl;
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
  
  nofFailedTests += test_formula ();

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
