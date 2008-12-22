/*-------------------------------------------------------------------------*
 | $Id:: tNewClass.cc 1964 2008-09-06 17:52:38Z baehren                  $ |
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
#include <Imaging/GeomDelay.h>
#include "create_data.h"

// Namespace usage
using std::endl;
using CR::GeomDelay;

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
  std::cout << "\n[tGeomDelay::test_constructors]\n" << endl;

  int nofFailedTests (0);
  
  std::cout << "[1] Testing default constructor ..." << endl;
  try {
    GeomDelay newObject;
    //
    newObject.summary(); 
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  std::cout << "[2] Testing argumented constructor ..." << endl;
  try {
    Matrix<double> antPositions = get_antennaPositions();
    Matrix<double> skyPositions = get_skyPositions();
    GeomDelay newObject (antPositions,
			 skyPositions);
    //
    newObject.summary(); 
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  std::cout << "[3] Testing argumented constructor ..." << endl;
  try {
    Vector<casa::MVPosition> antPositions (1);
    Vector<casa::MVPosition> skyPositions (1);

    antPositions(0) = casa::MVPosition (100,100,0);
    skyPositions(0) = casa::MVPosition (casa::Quantity(100,"m"),
					casa::Quantity(0,"deg"),
					casa::Quantity(90,"deg"));
    GeomDelay newObject (antPositions,
			 skyPositions);
    //
    newObject.summary();
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
  std::cout << "\n[tGeomDelay::test_static_functions]\n" << endl;

  int nofFailedTests (0);

  std::cout << "[1] delay(std::vector<double>,std::vector<double>)" << endl;
  try {
    std::vector<double> antPosition (3);
    std::vector<double> skyPosition (3);

    antPosition[0] = antPosition[1] = 100.0;
    antPosition[2] = 0.0;

    skyPosition[0] = 100;
    skyPosition[1] = 100;
    skyPosition[2] = 100;

    std::cout << "-- antenna position  = ["
	      << antPosition[0] << ","
	      << antPosition[1] << ","
	      << antPosition[2] << "]"
	      << endl;
    std::cout << "-- sky position      = ["
	      << skyPosition[0] << ","
	      << skyPosition[1] << ","
	      << skyPosition[2] << "]"
	      << endl;
    std::cout << "-- geometrical delay = " << GeomDelay::delay(antPosition,
							       skyPosition,
							       false)
	      << endl;
    std::cout << "-- geometrical delay = " << GeomDelay::delay(antPosition,
							       skyPosition,
							       true)
	      << endl;
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  } 
  
  std::cout << "[2] delay(casa::Vector<double>,casa::Vector<double>)" << endl;
  try {
    casa::Vector<double> antPosition (3);
    casa::Vector<double> skyPosition (3);

    antPosition(0) = antPosition(1) = 100.0;
    antPosition(2) = 0.0;

    skyPosition(0) = 100;
    skyPosition(1) = 100;
    skyPosition(2) = 100;

    std::cout << "-- antenna position  = " << antPosition << endl;
    std::cout << "-- sky position      = " << skyPosition << endl;
    std::cout << "-- geometrical delay = " << GeomDelay::delay(antPosition,
							       skyPosition,
							       false)
	      << endl;
    std::cout << "-- geometrical delay = " << GeomDelay::delay(antPosition,
							       skyPosition,
							       true)
	      << endl;
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

  return nofFailedTests;
}
