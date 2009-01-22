/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
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

// Namespace usage
using CR::CoordinateType;

/*!
  \file tCoordinateType.cc

  \ingroup CR_Coordinates

  \brief A collection of test routines for the CoordinateType class
 
  \author Lars B&auml;hren
 
  \date 2008/08/29
*/

// -----------------------------------------------------------------------------

/*!
  \brief Test constructors for a new CoordinateType object

  \return nofFailedTests -- The number of failed tests within this function.
*/
int test_CoordinateType ()
{
  std::cout << "\n[tCoordinateType::test_CoordinateType]\n" << std::endl;

  int nofFailedTests (0);
  
  std::cout << "[1] Testing default constructor ..." << std::endl;
  try {
    CoordinateType coord;
    //
    coord.summary(); 
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

  std::cout << "[2] Argumented constructor ..." << std::endl;
  try {
    CoordinateType coord1 (CoordinateType::DirectionRadius);
    coord1.summary();
    //
    CoordinateType coord2 (CoordinateType::Cartesian);
    coord2.summary();
    //
    CoordinateType coord3 (CoordinateType::Spherical);
    coord3.summary();
    //
    CoordinateType coord4 (CoordinateType::Cylindrical);
    coord4.summary();
    //
    CoordinateType coord5 (CoordinateType::Time);
    coord5.summary();
    //
    CoordinateType coord6 (CoordinateType::Frequency);
    coord6.summary();
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  std::cout << "[3] Copy constructor ..." << std::endl;
  try {
    CoordinateType coord1 (CoordinateType::DirectionRadius);
    coord1.summary();
    //
    CoordinateType coord2 (coord1);
    coord2.summary();
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}

// -----------------------------------------------------------------------------

/*!
  \brief Test generation of casa::Coordinate objects

  \return nofFailedTests -- The number of failed tests within this function.
*/
int test_casaCoordinate ()
{
  std::cout << "\n[tCoordinateType::test_casaCoordinate]\n" << std::endl;

  int nofFailedTests (0);

  std::cout << "[1] Create linear coordinate with single axis ..." << std::endl;
  try {
    casa::LinearCoordinate coord (CoordinateType::makeLinearCoordinate());
    // summary of the coordinate
    CoordinateType::summary (std::cout,coord);
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

  std::cout << "[2] Create linear coordinate with 3 axes ..." << std::endl;
  try {
    unsigned int nofAxes (3);
    casa::LinearCoordinate coord (CoordinateType::makeLinearCoordinate(nofAxes));
    // summary of the coordinate
    CoordinateType::summary (std::cout,coord);
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

  std::cout << "[3] Create linear coordinate with 3 axes ..." << std::endl;
  try {
    unsigned int nofAxes (3);
    Vector<casa::String> names (nofAxes,"Length");
    Vector<casa::String> units (nofAxes,"m");
    // create Coordinate object
    casa::LinearCoordinate coord (CoordinateType::makeLinearCoordinate(nofAxes,
								       names,
								       units));
    // summary of the coordinate
    CoordinateType::summary (std::cout,coord);
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

  return nofFailedTests;
}

// -----------------------------------------------------------------------------

int main ()
{
  int nofFailedTests (0);

  // Test for the constructor(s)
  nofFailedTests += test_CoordinateType ();
  // Test generation of casa::Coordinate objects
  nofFailedTests += test_casaCoordinate ();
  
  return nofFailedTests;
}
