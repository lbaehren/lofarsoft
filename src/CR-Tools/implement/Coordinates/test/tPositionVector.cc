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

#include <Coordinates/PositionVector.h>

// Namespace usage
using CR::CoordinateType;
using CR::PositionVector;

/*!
  \file tPositionVector.cc

  \ingroup CR_Coordinates

  \brief A collection of test routines for the PositionVector class
 
  \author Lars B&auml;hren
 
  \date 2008/12/18
*/

// -----------------------------------------------------------------------------

/*!
  \brief Test constructors for a new PositionVector object

  \return nofFailedTests -- The number of failed tests encountered within this
          function.
*/
int test_constructors ()
{
  std::cout << "\n[tPositionVector::test_constructors]\n" << std::endl;

  int nofFailedTests (0);
  
  std::cout << "[1] Testing default constructor ..." << std::endl;
  try {
    PositionVector pos;
    pos.summary(); 
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  std::cout << "[2] Testing argumented constructor ..." << std::endl;
  try {
    CR::CoordinateType coordType (CR::CoordinateType::Spherical);

    PositionVector pos (coordType);
    pos.summary(); 
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}

// -----------------------------------------------------------------------------

/*!
  \brief Test conversion from (Azimuth,Elevation,Radius) coordinates to other

  \return nofFailedTests -- The number of failed tests encountered within this
          function.
*/
int test_AzElRadius2other ()
{
  std::cout << "\n[tPositionVector::test_AzElRadius2other]\n" << std::endl;

  int nofFailedTests (0);
  Vector<double> azel (3,.0);
  Vector<double> other (3,.0);

  return nofFailedTests;
}

// -----------------------------------------------------------------------------

/*!
  \brief Test conversion from cartesian coordinates to other

  \return nofFailedTests -- The number of failed tests encountered within this
          function.
*/
int test_Cartesian2other ()
{
  std::cout << "\n[tPositionVector::test_Cartesian2other]\n" << std::endl;

  int nofFailedTests (0);
  Vector<double> cartesian(3);
  Vector<double> other (3);

  //__________________________________________________________________

  std::cout << "[1] Cartesian -> AzElHeight" << std::endl;
  try {
    other = 0;
    //
    cartesian(0) = 0;
    cartesian(1) = 0;
    cartesian(2) = 0;
    PositionVector::convert(other,CoordinateType::AzElHeight,
			    cartesian,CoordinateType::Cartesian,
			    true);
    std::cout << "\t" << cartesian << "\t->\t" << other << std::endl;
    //
    cartesian(0) = 1;
    cartesian(1) = 0;
    cartesian(2) = 0;
    PositionVector::convert(other,CoordinateType::AzElHeight,
			    cartesian,CoordinateType::Cartesian,
			    true);
    std::cout << "\t" << cartesian << "\t->\t" << other << std::endl;
    //
    cartesian(0) = 0;
    cartesian(1) = 1;
    cartesian(2) = 0;
    PositionVector::convert(other,CoordinateType::AzElHeight,
			    cartesian,CoordinateType::Cartesian,
			    true);
    std::cout << "\t" << cartesian << "\t->\t" << other << std::endl;
    //
    cartesian(0) = 0;
    cartesian(1) = 0;
    cartesian(2) = 1;
    PositionVector::convert(other,CoordinateType::AzElHeight,
			    cartesian,CoordinateType::Cartesian,
			    true);
    std::cout << "\t" << cartesian << "\t->\t" << other << std::endl;
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

  //__________________________________________________________________

  std::cout << "[2] Cartesian -> AzElRadius" << std::endl;
  try {
    other = 0;
    //
    cartesian(0) = 0;
    cartesian(1) = 0;
    cartesian(2) = 0;
    PositionVector::convert(other,CoordinateType::AzElRadius,
			    cartesian,CoordinateType::Cartesian,
			    true);
    std::cout << "\t" << cartesian << "\t->\t" << other << std::endl;
    //
    cartesian(0) = 1;
    cartesian(1) = 0;
    cartesian(2) = 0;
    PositionVector::convert(other,CoordinateType::AzElRadius,
			    cartesian,CoordinateType::Cartesian,
			    true);
    std::cout << "\t" << cartesian << "\t->\t" << other << std::endl;
    //
    cartesian(0) = 0;
    cartesian(1) = 1;
    cartesian(2) = 0;
    PositionVector::convert(other,CoordinateType::AzElRadius,
			    cartesian,CoordinateType::Cartesian,
			    true);
    std::cout << "\t" << cartesian << "\t->\t" << other << std::endl;
    //
    cartesian(0) = 0;
    cartesian(1) = 0;
    cartesian(2) = 1;
    PositionVector::convert(other,CoordinateType::AzElRadius,
			    cartesian,CoordinateType::Cartesian,
			    true);
    std::cout << "\t" << cartesian << "\t->\t" << other << std::endl;
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

  //__________________________________________________________________

  std::cout << "[3] Cartesian -> Cartesian" << std::endl;
  try {
    other = 0;
    //
    cartesian(0) = 0;
    cartesian(1) = 0;
    cartesian(2) = 0;
    PositionVector::convert(other,CoordinateType::Cartesian,
			    cartesian,CoordinateType::Cartesian,
			    true);
    std::cout << "\t" << cartesian << "\t->\t" << other << std::endl;
    //
    cartesian(0) = 1;
    cartesian(1) = 0;
    cartesian(2) = 0;
    PositionVector::convert(other,CoordinateType::Cartesian,
			    cartesian,CoordinateType::Cartesian,
			    true);
    std::cout << "\t" << cartesian << "\t->\t" << other << std::endl;
    //
    cartesian(0) = 0;
    cartesian(1) = 1;
    cartesian(2) = 0;
    PositionVector::convert(other,CoordinateType::Cartesian,
			    cartesian,CoordinateType::Cartesian,
			    true);
    std::cout << "\t" << cartesian << "\t->\t" << other << std::endl;
    //
    cartesian(0) = 0;
    cartesian(1) = 0;
    cartesian(2) = 1;
    PositionVector::convert(other,CoordinateType::Cartesian,
			    cartesian,CoordinateType::Cartesian,
			    true);
    std::cout << "\t" << cartesian << "\t->\t" << other << std::endl;
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

  //__________________________________________________________________

  std::cout << "[4] Cartesian -> Cylindrical" << std::endl;
  try {
    other = 0;
    //
    cartesian(0) = 0;
    cartesian(1) = 0;
    cartesian(2) = 0;
    PositionVector::convert(other,CoordinateType::Cylindrical,
			    cartesian,CoordinateType::Cartesian,
			    true);
    std::cout << "\t" << cartesian << "\t->\t" << other << std::endl;
    //
    cartesian(0) = 1;
    cartesian(1) = 0;
    cartesian(2) = 0;
    PositionVector::convert(other,CoordinateType::Cylindrical,
			    cartesian,CoordinateType::Cartesian,
			    true);
    std::cout << "\t" << cartesian << "\t->\t" << other << std::endl;
    //
    cartesian(0) = 0;
    cartesian(1) = 1;
    cartesian(2) = 0;
    PositionVector::convert(other,CoordinateType::Cylindrical,
			    cartesian,CoordinateType::Cartesian,
			    true);
    std::cout << "\t" << cartesian << "\t->\t" << other << std::endl;
    //
    cartesian(0) = 0;
    cartesian(1) = 0;
    cartesian(2) = 1;
    PositionVector::convert(other,CoordinateType::Cylindrical,
			    cartesian,CoordinateType::Cartesian,
			    true);
    std::cout << "\t" << cartesian << "\t->\t" << other << std::endl;
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

  //__________________________________________________________________

  std::cout << "[5] Cartesian -> DirectionRadius" << std::endl;
  try {
    other = 0;
    //
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

  //__________________________________________________________________

  std::cout << "[6] Cartesian -> LongLatRadius" << std::endl;
  try {
    other = 0;
    //
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

  //__________________________________________________________________

  std::cout << "[7] Cartesian -> NorthEastHeight" << std::endl;
  try {
    other = 0;
    //
    cartesian(0) = 0;
    cartesian(1) = 0;
    cartesian(2) = 0;
    PositionVector::convert(other,CoordinateType::NorthEastHeight,
			    cartesian,CoordinateType::Cartesian,
			    true);
    std::cout << "\t" << cartesian << "\t->\t" << other << std::endl;
    //
    cartesian(0) = 1;
    cartesian(1) = 0;
    cartesian(2) = 0;
    PositionVector::convert(other,CoordinateType::NorthEastHeight,
			    cartesian,CoordinateType::Cartesian,
			    true);
    std::cout << "\t" << cartesian << "\t->\t" << other << std::endl;
    //
    cartesian(0) = 0;
    cartesian(1) = 1;
    cartesian(2) = 0;
    PositionVector::convert(other,CoordinateType::NorthEastHeight,
			    cartesian,CoordinateType::Cartesian,
			    true);
    std::cout << "\t" << cartesian << "\t->\t" << other << std::endl;
    //
    cartesian(0) = 0;
    cartesian(1) = 0;
    cartesian(2) = 1;
    PositionVector::convert(other,CoordinateType::NorthEastHeight,
			    cartesian,CoordinateType::Cartesian,
			    true);
    std::cout << "\t" << cartesian << "\t->\t" << other << std::endl;
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

  //__________________________________________________________________

  std::cout << "[8] Cartesian -> Spherical" << std::endl;
  try {
    other = 0;
    //
    cartesian(0) = 0;
    cartesian(1) = 0;
    cartesian(2) = 0;
    PositionVector::convert(other,CoordinateType::Spherical,
			    cartesian,CoordinateType::Cartesian,
			    true);
    std::cout << "\t" << cartesian << "\t->\t" << other << std::endl;
    //
    cartesian(0) = 1;
    cartesian(1) = 0;
    cartesian(2) = 0;
    PositionVector::convert(other,CoordinateType::Spherical,
			    cartesian,CoordinateType::Cartesian,
			    true);
    std::cout << "\t" << cartesian << "\t->\t" << other << std::endl;
    //
    cartesian(0) = 0;
    cartesian(1) = 1;
    cartesian(2) = 0;
    PositionVector::convert(other,CoordinateType::Spherical,
			    cartesian,CoordinateType::Cartesian,
			    true);
    std::cout << "\t" << cartesian << "\t->\t" << other << std::endl;
    //
    cartesian(0) = 0;
    cartesian(1) = 0;
    cartesian(2) = 1;
    PositionVector::convert(other,CoordinateType::Spherical,
			    cartesian,CoordinateType::Cartesian,
			    true);
    std::cout << "\t" << cartesian << "\t->\t" << other << std::endl;
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

  return nofFailedTests;
}

//_______________________________________________________________________________
//                                                         test_Cylindrical2other

/*!
  \brief Test conversion from Cylindrical coordinates to other

  \return nofFailedTests -- The number of failed tests encountered within this
          function.
*/
int test_Cylindrical2other ()
{
  std::cout << "\n[tPositionVector::test_Cylindrical2other]\n" << std::endl;

  int nofFailedTests (0);
  Vector<double> northEastHeight(3);
  Vector<double> other (3);

  return nofFailedTests;
}

//_______________________________________________________________________________
// test_DirectionRadius2other

/*!
  \brief Test conversion from (Direction,Radius) coordinates to other

  Since the direction typically is given as (Long,Lat) pair, this conversion
  should be identical to \f$ (Long,Lat,Radius) \f$ to \f$ (x,y,z) \f$.

  \return nofFailedTests -- The number of failed tests encountered within this
          function.
*/
int test_DirectionRadius2other ()
{
  std::cout << "\n[tPositionVector::test_DirectionRadius2other]\n" << std::endl;

  int nofFailedTests (0);
  Vector<double> in (3);
  Vector<double> other (3);

  std::cout << "[1] DirectionRadius -> Cartesian" << std::endl;
  try {
    in(0) = 0;
    in(1) = 0;
    in(2) = 1;
    PositionVector::convert(other,CoordinateType::Cartesian,
			    in,CoordinateType::DirectionRadius,
			    true);
    std::cout << "\t" << in << "\t->\t" << other << std::endl;
    //
    in(0) = 45;
    in(1) = 0;
    in(2) = 1;
    PositionVector::convert(other,CoordinateType::Cartesian,
			    in,CoordinateType::DirectionRadius,
			    true);
    std::cout << "\t" << in << "\t->\t" << other << std::endl;
    //
    in(0) = 0;
    in(1) = 45;
    in(2) = 1;
    PositionVector::convert(other,CoordinateType::Cartesian,
			    in,CoordinateType::DirectionRadius,
			    true);
    std::cout << "\t" << in << "\t->\t" << other << std::endl;
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

  return nofFailedTests;
}

//_______________________________________________________________________________
//                                                     test_NorthEastHeight2other

/*!
  \brief Test conversion from (North,East,Height) coordinates to other

  \return nofFailedTests -- The number of failed tests encountered within this
          function.
*/
int test_NorthEastHeight2other ()
{
  std::cout << "\n[tPositionVector::test_NorthEastHeight2other]\n" << std::endl;

  int nofFailedTests (0);
  Vector<double> northEastHeight(3);
  Vector<double> other (3);

  std::cout << "[1] NorthEastHeight -> AzElHeight" << std::endl;
  try {
    other = 0;
    //
    northEastHeight(0) = 0;
    northEastHeight(1) = 0;
    northEastHeight(2) = 0;
    PositionVector::convert(other,CoordinateType::AzElHeight,
			    northEastHeight,CoordinateType::NorthEastHeight,
			    true);
    std::cout << "\t" << northEastHeight << "\t->\t" << other << std::endl;
    //
    northEastHeight(0) = 1;
    northEastHeight(1) = 0;
    northEastHeight(2) = 0;
    PositionVector::convert(other,CoordinateType::AzElHeight,
			    northEastHeight,CoordinateType::NorthEastHeight,
			    true);
    std::cout << "\t" << northEastHeight << "\t->\t" << other << std::endl;
    //
    northEastHeight(0) = 0;
    northEastHeight(1) = 1;
    northEastHeight(2) = 0;
    PositionVector::convert(other,CoordinateType::AzElHeight,
			    northEastHeight,CoordinateType::NorthEastHeight,
			    true);
    std::cout << "\t" << northEastHeight << "\t->\t" << other << std::endl;
    //
    northEastHeight(0) = 0;
    northEastHeight(1) = 0;
    northEastHeight(2) = 1;
    PositionVector::convert(other,CoordinateType::AzElHeight,
			    northEastHeight,CoordinateType::NorthEastHeight,
			    true);
    std::cout << "\t" << northEastHeight << "\t->\t" << other << std::endl;
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

  std::cout << "[2] NorthEastHeight -> AzElRadius" << std::endl;
  try {
    other = 0;
    //
    northEastHeight(0) = 0;
    northEastHeight(1) = 0;
    northEastHeight(2) = 0;
    PositionVector::convert(other,CoordinateType::AzElRadius,
			    northEastHeight,CoordinateType::NorthEastHeight,
			    true);
    std::cout << "\t" << northEastHeight << "\t->\t" << other << std::endl;
    //
    northEastHeight(0) = 1;
    northEastHeight(1) = 0;
    northEastHeight(2) = 0;
    PositionVector::convert(other,CoordinateType::AzElRadius,
			    northEastHeight,CoordinateType::NorthEastHeight,
			    true);
    std::cout << "\t" << northEastHeight << "\t->\t" << other << std::endl;
    //
    northEastHeight(0) = 0;
    northEastHeight(1) = 1;
    northEastHeight(2) = 0;
    PositionVector::convert(other,CoordinateType::AzElRadius,
			    northEastHeight,CoordinateType::NorthEastHeight,
			    true);
    std::cout << "\t" << northEastHeight << "\t->\t" << other << std::endl;
    //
    northEastHeight(0) = 0;
    northEastHeight(1) = 0;
    northEastHeight(2) = 1;
    PositionVector::convert(other,CoordinateType::AzElRadius,
			    northEastHeight,CoordinateType::NorthEastHeight,
			    true);
    std::cout << "\t" << northEastHeight << "\t->\t" << other << std::endl;
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

  std::cout << "[3] NorthEastHeight -> Cartesian" << std::endl;
  try {
    other = 0;
    //
    northEastHeight(0) = 0;
    northEastHeight(1) = 0;
    northEastHeight(2) = 0;
    PositionVector::convert(other,CoordinateType::Cartesian,
			    northEastHeight,CoordinateType::NorthEastHeight,
			    true);
    std::cout << "\t" << northEastHeight << "\t->\t" << other << std::endl;
    //
    northEastHeight(0) = 1;
    northEastHeight(1) = 0;
    northEastHeight(2) = 0;
    PositionVector::convert(other,CoordinateType::Cartesian,
			    northEastHeight,CoordinateType::NorthEastHeight,
			    true);
    std::cout << "\t" << northEastHeight << "\t->\t" << other << std::endl;
    //
    northEastHeight(0) = 0;
    northEastHeight(1) = 1;
    northEastHeight(2) = 0;
    PositionVector::convert(other,CoordinateType::Cartesian,
			    northEastHeight,CoordinateType::NorthEastHeight,
			    true);
    std::cout << "\t" << northEastHeight << "\t->\t" << other << std::endl;
    //
    northEastHeight(0) = 0;
    northEastHeight(1) = 0;
    northEastHeight(2) = 1;
    PositionVector::convert(other,CoordinateType::Cartesian,
			    northEastHeight,CoordinateType::NorthEastHeight,
			    true);
    std::cout << "\t" << northEastHeight << "\t->\t" << other << std::endl;
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

  std::cout << "[4] NorthEastHeight -> Cylindrical" << std::endl;
  try {
    other = 0;
    //
    northEastHeight(0) = 0;
    northEastHeight(1) = 0;
    northEastHeight(2) = 0;
    PositionVector::convert(other,CoordinateType::Cylindrical,
			    northEastHeight,CoordinateType::NorthEastHeight,
			    true);
    std::cout << "\t" << northEastHeight << "\t->\t" << other << std::endl;
    //
    northEastHeight(0) = 1;
    northEastHeight(1) = 0;
    northEastHeight(2) = 0;
    PositionVector::convert(other,CoordinateType::Cylindrical,
			    northEastHeight,CoordinateType::NorthEastHeight,
			    true);
    std::cout << "\t" << northEastHeight << "\t->\t" << other << std::endl;
    //
    northEastHeight(0) = 0;
    northEastHeight(1) = 1;
    northEastHeight(2) = 0;
    PositionVector::convert(other,CoordinateType::Cylindrical,
			    northEastHeight,CoordinateType::NorthEastHeight,
			    true);
    std::cout << "\t" << northEastHeight << "\t->\t" << other << std::endl;
    //
    northEastHeight(0) = 0;
    northEastHeight(1) = 0;
    northEastHeight(2) = 1;
    PositionVector::convert(other,CoordinateType::Cylindrical,
			    northEastHeight,CoordinateType::NorthEastHeight,
			    true);
    std::cout << "\t" << northEastHeight << "\t->\t" << other << std::endl;
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

  std::cout << "[5] NorthEastHeight -> DirectionRadius" << std::endl;
  try {
    other = 0;
    //
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

  std::cout << "[6] NorthEastHeight -> LongLatRadius" << std::endl;
  try {
    other = 0;
    //
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

  std::cout << "[7] NorthEastHeight -> NorthEastHeight" << std::endl;
  try {
    other = 0;
    //
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

  std::cout << "[8] NorthEastHeight -> Spherical" << std::endl;
  try {
    other = 0;
    //
    northEastHeight(0) = 0;
    northEastHeight(1) = 0;
    northEastHeight(2) = 0;
    PositionVector::convert(other,CoordinateType::Spherical,
			    northEastHeight,CoordinateType::NorthEastHeight,
			    true);
    std::cout << "\t" << northEastHeight << "\t->\t" << other << std::endl;
    //
    northEastHeight(0) = 1;
    northEastHeight(1) = 0;
    northEastHeight(2) = 0;
    PositionVector::convert(other,CoordinateType::Spherical,
			    northEastHeight,CoordinateType::NorthEastHeight,
			    true);
    std::cout << "\t" << northEastHeight << "\t->\t" << other << std::endl;
    //
    northEastHeight(0) = 0;
    northEastHeight(1) = 1;
    northEastHeight(2) = 0;
    PositionVector::convert(other,CoordinateType::Spherical,
			    northEastHeight,CoordinateType::NorthEastHeight,
			    true);
    std::cout << "\t" << northEastHeight << "\t->\t" << other << std::endl;
    //
    northEastHeight(0) = 0;
    northEastHeight(1) = 0;
    northEastHeight(2) = 1;
    PositionVector::convert(other,CoordinateType::Spherical,
			    northEastHeight,CoordinateType::NorthEastHeight,
			    true);
    std::cout << "\t" << northEastHeight << "\t->\t" << other << std::endl;
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

  // Test the static functions provided by the class
  nofFailedTests += test_AzElRadius2other();
  nofFailedTests += test_Cartesian2other();
  nofFailedTests += test_Cylindrical2other();
  nofFailedTests += test_DirectionRadius2other();
  nofFailedTests += test_NorthEastHeight2other();
  // Test for the constructor(s)
  nofFailedTests += test_constructors ();

  return nofFailedTests;
}
