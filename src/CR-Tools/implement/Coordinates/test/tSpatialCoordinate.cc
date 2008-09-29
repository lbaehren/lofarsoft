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
  
  std::cout << "\n[tSpatialCoordinate::test_SpatialCoordinate]\n" << std::endl;

  std::cout << "[1] Testing default constructor ..." << std::endl;
  try {
    SpatialCoordinate newSpatialCoordinate;
    //
    newSpatialCoordinate.summary(); 
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  std::cout << "[2] Construction with coordinate type ..." << std::endl;
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
int test_CoordinateFunctions ()
{
  int nofFailedTests (0);
  
  return nofFailedTests;
}

// -----------------------------------------------------------------------------

int main ()
{
  int nofFailedTests (0);

  // Test for the constructor(s)
  nofFailedTests += test_SpatialCoordinate ();
  /* Test wrapping of parameter access and conversion functions */
  nofFailedTests += test_CoordinateFunctions ();

  return nofFailedTests;
}
