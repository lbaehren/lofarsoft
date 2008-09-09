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

#include <Coordinates/CoordinateDomain.h>

// Namespace usage
using CR::CoordinateDomain;

/*!
  \file tCoordinateDomain.cc

  \ingroup CR_Coordinates

  \brief A collection of test routines for the CoordinateDomain class
 
  \author Lars B&auml;hren
 
  \date 2008/09/09
*/

// -----------------------------------------------------------------------------

/*!
  \brief Test constructors for a new CoordinateDomain object

  \return nofFailedTests -- The number of failed tests within this function.
*/
int test_constructors ()
{
  int nofFailedTests (0);
  
  std::cout << "\n[tCoordinateDomain::test_constructors]\n" << std::endl;

  std::cout << "[1] Simple argumented constructor ..." << std::endl;
  try {
    CoordinateDomain domainTime (CoordinateDomain::Time);
    CoordinateDomain domainFreq (CoordinateDomain::Frequency);
    CoordinateDomain domainSpace (CoordinateDomain::Space);
    //
    domainTime.summary();
    domainFreq.summary();
    domainSpace.summary();
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}

// -----------------------------------------------------------------------------

int test_staticFunctions ()
{
  std::cout << "\n[tCoordinateDomain::test_staticFunctions]\n" << std::endl;

  int nofFailedTests (0);

  std::cout << "[1] isSpatial() ..." << std::endl;
  try {
    std::cout << "-- Time      = "
	      << CoordinateDomain::isSpatial(CoordinateDomain::Time)
	      << std::endl;
    std::cout << "-- Frequency = "
	      << CoordinateDomain::isSpatial(CoordinateDomain::Frequency)
	      << std::endl;
    std::cout << "-- Space     = "
	      << CoordinateDomain::isSpatial(CoordinateDomain::Space)
	      << std::endl;
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

  std::cout << "[2] isTemporal() ..." << std::endl;
  try {
    std::cout << "-- Time      = "
	      << CoordinateDomain::isTemporal(CoordinateDomain::Time)
	      << std::endl;
    std::cout << "-- Frequency = "
	      << CoordinateDomain::isTemporal(CoordinateDomain::Frequency)
	      << std::endl;
    std::cout << "-- Space     = "
	      << CoordinateDomain::isTemporal(CoordinateDomain::Space)
	      << std::endl;
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

  std::cout << "[3] isSpectral() ..." << std::endl;
  try {
    std::cout << "-- Time      = "
	      << CoordinateDomain::isSpectral(CoordinateDomain::Time)
	      << std::endl;
    std::cout << "-- Frequency = "
	      << CoordinateDomain::isSpectral(CoordinateDomain::Frequency)
	      << std::endl;
    std::cout << "-- Space     = "
	      << CoordinateDomain::isSpectral(CoordinateDomain::Space)
	      << std::endl;
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

  std::cout << "[4] baseUnit() ..." << std::endl;
  try {
    std::cout << "-- Time      = "
	      << CoordinateDomain::baseUnit(CoordinateDomain::Time)
	      << std::endl;
    std::cout << "-- Frequency = "
	      << CoordinateDomain::baseUnit(CoordinateDomain::Frequency)
	      << std::endl;
    std::cout << "-- Space     = "
	      << CoordinateDomain::baseUnit(CoordinateDomain::Space)
	      << std::endl;
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
  nofFailedTests += test_constructors ();
  // Test the static functions provided by the class
  nofFailedTests += test_staticFunctions ();

  return nofFailedTests;
}
