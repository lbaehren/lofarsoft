/***************************************************************************
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

/* $Id$*/

#include <iostream>
#include <fstream>

#ifdef HAVE_CASA
#include <casa/Arrays/IPosition.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/Vector.h>
#endif

#include <Imaging/GeometricalDelay.h>

using blitz::Array;
using blitz::Range;
using CR::GeometricalDelay;
using CR::L2Norm;

//! Speed of light (Source: Wikipedia)
const double lightspeed = 299792458;

/*!
  \file tGeometricalDelay.cc

  \ingroup Imaging

  \brief A collection of test routines for GeometricalDelay
 
  \author Lars Baehren
 
  \date 2007/01/15
*/

// -----------------------------------------------------------------------------

#ifdef HAVE_CASA
void getPositions (casa::Matrix<double> &skyPositions,
		   casa::Matrix<double> &antPositions)
{
  int nofCoordinates (3);
  int nofAntennas (2);
  int nofDirections (3);

  // Antenna positions

  antPositions.resize(nofAntennas,nofCoordinates);

  antPositions(0,0) = -100.0;
  antPositions(0,1) = 0.0;
  antPositions(0,2) = 0.0;
  antPositions(1,0) = 100.0;
  antPositions(1,1) = 0.0;
  antPositions(1,2) = 0.0;

  // Pointing directions (sky positions)
  
  skyPositions.resize(nofDirections,nofCoordinates);

  skyPositions            = 0.0;
  skyPositions.diagonal() = 100.0;

//   std::cout << "-- nof. antennas = " << nofAntennas   << std::endl;
//   std::cout << "-- ant positions = " << antPositions  << std::endl;
//   std::cout << "-- sky positions = " << skyPositions  << std::endl;
}
#endif

// -----------------------------------------------------------------------------

/*!
  \brief Fundamental testing on the different formulae for delay computation

  \return nofFailedTests -- The number of failed tests.
*/
#ifdef HAVE_CASA
int test_formula ()
{
  std::cout << "\n[test_formula] (CASA arrays)\n" << std::endl;
  
  int nofFailedTests (0);
  
  int nofCoordinates (3);
  casa::Matrix<double> antPositions;
  casa::Matrix<double> skyPositions;
  double delay (.0);
  
  getPositions (skyPositions, antPositions);

  /*!
    Standard version for the computation of the delay
   */
  std::cout << "[1] Far-field geometry" << std::endl;
  try {
    double scalarProduct (.0);

    for (int n(0); n<nofCoordinates; n++) {
      scalarProduct += (antPositions(1,n)-antPositions(0,n))*skyPositions(2,n);
    }
    delay = scalarProduct/lightspeed;
    
    std::cout << "delay(FF)     = " << delay         << std::endl;
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}
#else
#ifdef HAVE_BLITZ
int test_formula ()
{
  std::cout << "\n[test_formula] (Blitz++ arrays)\n" << std::endl;

  int nofFailedTests (0);

  int nofCoordinates (3);
  Array<double,1> skyPositions (nofCoordinates);
  Array<double,2> antPositions (2,nofCoordinates);
  double delay (.0);

  antPositions = -100.0, 0.0, 0.0, 100.0, 0.0, 0.0;
  skyPositions = 100.0, 100.0, 100.0;

  std::cout << "sky positions = " << skyPositions  << std::endl;
  std::cout << "ant positions = " << antPositions  << std::endl;

  /*!
    Standard version for the computation of the delay
   */
  std::cout << "[1] Far-field geometry" << std::endl;
  try {
    double scalarProduct (.0);

    for (int n(0); n<nofCoordinates; n++) {
      scalarProduct += (antPositions(1,n)-antPositions(0,n))*skyPositions(n);
    }
    delay = scalarProduct/lightspeed;
    
    std::cout << "delay(FF)     = " << delay         << std::endl;
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  /*!
    Computation of the delay based on the full 3-dim geometry
  */
  std::cout << "\n[2] Compute delay for full geometry\n" << std::endl;
  try {
    Array<double,1> diff0 (nofCoordinates);
    Array<double,1> diff1 (nofCoordinates);
    
    for (int n(0); n<nofCoordinates; n++) {
      diff0(n) = skyPositions(n)-antPositions(0,n);
      diff1(n) = skyPositions(n)-antPositions(1,n);
    }
    delay = (L2Norm(diff1)-L2Norm(diff0))/lightspeed;
    
    std::cout << "delay(NF)     = " << delay         << std::endl;
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

  /*
    Variation of the source position
  */
  std::cout << "\n[3] Variation of the source position\n" << std::endl;
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
      std::cout << "\t" << z << "\t" << delay << std::endl;
    }
  } catch (std::string message) {
    std::cerr << message << std::endl;
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
  std::cout << "\n[test_GeometricalDelay] (CASA arrays)\n" << std::endl;

  int nofFailedTests (0);
  casa::Matrix<double> antPositions;
  casa::Matrix<double> skyPositions;

  getPositions (skyPositions, antPositions);

  std::cout << "[1] Testing default constructor ..." << std::endl;
  try {
    GeometricalDelay delay;
    delay.summary();
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  std::cout << "[2] Testing argumented constructor ..." << std::endl;
  try {
    GeometricalDelay delay (antPositions,
			    skyPositions);
    delay.summary();
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  std::cout << "[3] Testing copy constructor ..." << std::endl;
  try {
    GeometricalDelay delay (antPositions,
			    skyPositions);
    delay.summary();

    GeometricalDelay delay_copy (delay);
    delay_copy.summary();
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}
#else
#ifdef HAVE_BLITZ
int test_GeometricalDelay ()
{
  std::cout << "\n[test_GeometricalDelay] (Blitz++ arrays)\n" << std::endl;

  int nofFailedTests (0);
  uint nofCoordinates (3);
  Array<double,2> antPositions (2,nofCoordinates);
  Array<double,2> skyPositions (3,nofCoordinates);
  
  antPositions = -100.0, 0.0, 0.0, 100.0, 0.0, 0.0;
  skyPositions = 100.0, 0.0, 0.0, 0.0, 100.0, 0.0, 0.0, 0.0, 100.0;
  
  std::cout << "[1] Testing default constructor ..." << std::endl;
  try {
    GeometricalDelay delay;
    delay.summary();
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  std::cout << "[2] Testing argumented constructor ..." << std::endl;
  try {
    GeometricalDelay delay (antPositions,
			    skyPositions);
    delay.summary();
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  std::cout << "[3] Testing copy constructor ..." << std::endl;
  try {
    GeometricalDelay delay (antPositions,
			    skyPositions);
    delay.summary();

    GeometricalDelay delay_copy (delay);
    delay_copy.summary();
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}
#endif
#endif

// -----------------------------------------------------------------------------

/*!
  \brief Test actual computation of the geometrical delay

  In order to plot the data in the exported table, run
  \verbatim
  set grid
  set xlabel 'Source distance, (0,0,z), from phase center [m]'
  set ylabel 'Geometrical delay [sec]'
  plot 'delays.data' u 2:3 t 'Antenna 1 [100,100,0]' w l, 'delays.data' u 2:4 t 'Antenna 1 [200,200,0]' w l
  \endverbatim

  \return nofFailedTests -- Number of failed tests

*/
#ifdef HAVE_CASA
int test_delayComputation ()
{
  std::cout << "\n[test_delayComputation]\n" << std::endl;

  int nofFailedTests (0);
  uint nofCoordinates (3);
  int nofAntennas (2);
  int nofPositions (100);
  double stepwidth (100);
  casa::Matrix<double> antPositions (nofAntennas,nofCoordinates);
  casa::Matrix<double> skyPositions (nofPositions,nofCoordinates);

  // assign positions of the antennas
  antPositions = 0.0;
  antPositions(0,0) = -100;
  antPositions(1,0) = 100;

  // assign the values for the sky positions
  for (int n (0); n<nofPositions; n++) {
    skyPositions.row(n) = stepwidth*n;
  }

  GeometricalDelay delay (antPositions,
			  skyPositions);
  delay.summary();
  
  casa::Matrix<double> delays = delay.delays();

  // export the computed values
  std::ofstream outfile;
  outfile.open("delays.data");
  for (int n (0); n<nofPositions; n++) {
    outfile << "\t" << n
	    << "\t" << skyPositions(n,0)
	    << "\t" << skyPositions(n,1)
	    << "\t" << skyPositions(n,2)
	    << "\t" << delays(0,n)       // delay for antenna 1 @ [-100,0,0]
	    << "\t" << delays(1,n)       // delay for antenna 2 @ [100,0,0]
	    << std::endl;
  }
  outfile.close();
  
  return nofFailedTests;
}
#else
#ifdef HAVE_BLITZ
int test_delayComputation ()
{
  std::cout << "\n[test_delayComputation]\n" << std::endl;

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
	    << std::endl;
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

  {
    nofFailedTests += test_formula ();
  }

  // Test for the constructor(s)
  {
    nofFailedTests += test_GeometricalDelay ();
  }

  // Test for the computation of the actual geometrical delay
  {
    nofFailedTests += test_delayComputation ();
  }


  return nofFailedTests;
}
