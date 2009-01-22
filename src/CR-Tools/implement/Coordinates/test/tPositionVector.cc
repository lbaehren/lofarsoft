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

int test_static_functions ()
{
  std::cout << "\n[tPositionVector::test_static_functions]\n" << std::endl;

  int nofFailedTests (0);

  //___________________________________________ Spherical -> Cartesian

  std::cout << "[1] Spherical -> Cartesian" << std::endl;
  try {
    casa::Vector<double> cartesian (3);
    casa::Vector<double> spherical (3);

    spherical(0) = 1;  // 
    spherical(1) = 0;
    spherical(2) = 0;
    PositionVector::convert(cartesian,CoordinateType::Cartesian,
			    spherical,CoordinateType::Spherical,
			    true);
    std::cout << "\t" << spherical << "\t->\t" << cartesian << std::endl;
    
    spherical(0) = 1;
    spherical(1) = 0;
    spherical(2) = 90;
    PositionVector::convert(cartesian,CoordinateType::Cartesian,
			    spherical,CoordinateType::Spherical,
			    true);
    std::cout << "\t" << spherical << "\t->\t" << cartesian << std::endl;
    
    spherical(0) = 1;
    spherical(1) = 90;
    spherical(2) = 90;
    PositionVector::convert(cartesian,CoordinateType::Cartesian,
			    spherical,CoordinateType::Spherical,
			    true);
    std::cout << "\t" << spherical << "\t->\t" << cartesian << std::endl;
    
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

  //_________________________________________ Cylindrical -> Cartesian

  std::cout << "[2] Cylindrical -> Cartesian" << std::endl;
  try {
    casa::Vector<double> cartesian (3);
    casa::Vector<double> cylindrical (3);

    cylindrical(0) = 1;  // r
    cylindrical(1) = 0;  // phi
    cylindrical(2) = 0;  // h
    PositionVector::convert(cartesian,CoordinateType::Cartesian,
			    cylindrical,CoordinateType::Cylindrical,
			    true);
    std::cout << "\t" << cylindrical << "\t->\t" << cartesian << std::endl;

    cylindrical(0) = 1;   // r
    cylindrical(1) = 90;  // phi
    cylindrical(2) = 0;   // h
    PositionVector::convert(cartesian,CoordinateType::Cartesian,
			    cylindrical,CoordinateType::Cylindrical,
			    true);
    std::cout << "\t" << cylindrical << "\t->\t" << cartesian << std::endl;

    cylindrical(0) = 0;   // r
    cylindrical(1) = 0;   // phi
    cylindrical(2) = 1;   // h
    PositionVector::convert(cartesian,CoordinateType::Cartesian,
			    cylindrical,CoordinateType::Cylindrical,
			    true);
    std::cout << "\t" << cylindrical << "\t->\t" << cartesian << std::endl;

  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

  //__________________________________________ AzElRadius -> Cartesian

  std::cout <<"[3] AzElRadius -> Cartesian" << std::endl;
  try {
    casa::Vector<double> cartesian (3);
    casa::Vector<double> azelradius (3);

    azelradius(0) = 0;  //  Az
    azelradius(1) = 0;  //  El
    azelradius(2) = 1;  //  r
    PositionVector::convert(cartesian,CoordinateType::Cartesian,
			    azelradius,CoordinateType::AzElRadius,
			    true);
    std::cout << "\t" << azelradius << "\t->\t" << cartesian << std::endl;
    
    azelradius(0) = 90;  //  Az
    azelradius(1) = 0;   //  El
    azelradius(2) = 1;   //  r
    PositionVector::convert(cartesian,CoordinateType::Cartesian,
			    azelradius,CoordinateType::AzElRadius,
			    true);
    std::cout << "\t" << azelradius << "\t->\t" << cartesian << std::endl;
    
    azelradius(0) = 0;   //  Az
    azelradius(1) = 90;  //  El
    azelradius(2) = 1;   //  r
    PositionVector::convert(cartesian,CoordinateType::Cartesian,
			    azelradius,CoordinateType::AzElRadius,
			    true);
    std::cout << "\t" << azelradius << "\t->\t" << cartesian << std::endl;
    
    azelradius(0) = 90;  //  Az
    azelradius(1) = 90;  //  El
    azelradius(2) = 1;   //  r
    PositionVector::convert(cartesian,CoordinateType::Cartesian,
			    azelradius,CoordinateType::AzElRadius,
			    true);
    std::cout << "\t" << azelradius << "\t->\t" << cartesian << std::endl;
    
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

  //_______________________________________ LongLatRadius -> Cartesian

  std::cout <<"[4] LongLatRadius -> Cartesian" << std::endl;
  try {

    casa::Vector<double> cartesian (3);
    casa::Vector<double> longLatRadius (3);

    longLatRadius(0) = 0;  //  long
    longLatRadius(1) = 0;  //  lat
    longLatRadius(2) = 1;  //  radius
    PositionVector::convert(cartesian,CoordinateType::Cartesian,
			    longLatRadius,CoordinateType::LongLatRadius,
			    true);
    std::cout << "\t" << longLatRadius << "\t->\t" << cartesian << std::endl;

    longLatRadius(0) = 90;  //  long
    longLatRadius(1) = 0;   //  lat
    longLatRadius(2) = 1;   //  radius
    PositionVector::convert(cartesian,CoordinateType::Cartesian,
			    longLatRadius,CoordinateType::LongLatRadius,
			    true);
    std::cout << "\t" << longLatRadius << "\t->\t" << cartesian << std::endl;

    longLatRadius(0) = 0;   //  long
    longLatRadius(1) = 90;  //  lat
    longLatRadius(2) = 1;   //  radius
    PositionVector::convert(cartesian,CoordinateType::Cartesian,
			    longLatRadius,CoordinateType::LongLatRadius,
			    true);
    std::cout << "\t" << longLatRadius << "\t->\t" << cartesian << std::endl;

  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

  //___________________________________________ Cartesian -> Spherical

  std::cout << "[5] Cartesian -> Spherical" << std::endl;
  try {
    casa::Vector<double> cartesian (3);
    casa::Vector<double> spherical (3);

    cartesian(0) = 0;
    cartesian(1) = 0;
    cartesian(2) = 0;
    PositionVector::convert(spherical,CoordinateType::Spherical,
			    cartesian,CoordinateType::Cartesian,
			    true);
    std::cout << "\t" << cartesian << "\t->\t" << spherical << std::endl;

    cartesian(0) = 1;
    cartesian(1) = 0;
    cartesian(2) = 0;
    PositionVector::convert(spherical,CoordinateType::Spherical,
			    cartesian,CoordinateType::Cartesian,
			    true);
    std::cout << "\t" << cartesian << "\t->\t" << spherical << std::endl;

    cartesian(0) = 0;
    cartesian(1) = 1;
    cartesian(2) = 0;
    PositionVector::convert(spherical,CoordinateType::Spherical,
			    cartesian,CoordinateType::Cartesian,
			    true);
    std::cout << "\t" << cartesian << "\t->\t" << spherical << std::endl;

    cartesian(0) = 0;
    cartesian(1) = 0;
    cartesian(2) = 1;
    PositionVector::convert(spherical,CoordinateType::Spherical,
			    cartesian,CoordinateType::Cartesian,
			    true);
    std::cout << "\t" << cartesian << "\t->\t" << spherical << std::endl;

  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

  //__________________________________________ AzElRadius -> Spherical

  std::cout << "[6] AzElRadius -> Spherical" << std::endl;
  try {
    casa::Vector<double> azelradius (3);
    casa::Vector<double> spherical (3);

    azelradius(0) = 0;
    azelradius(1) = 0;
    azelradius(2) = 0;
    PositionVector::convert(spherical,CoordinateType::Spherical,
			    azelradius,CoordinateType::AzElRadius,
			    true);
    std::cout << "\t" << azelradius << "\t->\t" << spherical << std::endl;

    azelradius(0) = 90;
    azelradius(1) = 0;
    azelradius(2) = 0;
    PositionVector::convert(spherical,CoordinateType::Spherical,
			    azelradius,CoordinateType::AzElRadius,
			    true);
    std::cout << "\t" << azelradius << "\t->\t" << spherical << std::endl;

    azelradius(0) = 0;
    azelradius(1) = 90;
    azelradius(2) = 0;
    PositionVector::convert(spherical,CoordinateType::Spherical,
			    azelradius,CoordinateType::AzElRadius,
			    true);
    std::cout << "\t" << azelradius << "\t->\t" << spherical << std::endl;

    azelradius(0) = 0;
    azelradius(1) = 0;
    azelradius(2) = 1;
    PositionVector::convert(spherical,CoordinateType::Spherical,
			    azelradius,CoordinateType::AzElRadius,
			    true);
    std::cout << "\t" << azelradius << "\t->\t" << spherical << std::endl;

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
  nofFailedTests += test_static_functions();
  // Test for the constructor(s)
  nofFailedTests += test_constructors ();

  return nofFailedTests;
}
