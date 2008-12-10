/*-------------------------------------------------------------------------*
 | $Id                                                                     |
 *-------------------------------------------------------------------------*
 ***************************************************************************
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

#include <Utilities/MConversions.h>

using std::endl;

using casa::MDirection;
using casa::MPosition;
using casa::MVPosition;

/*!
  \file tMConversions.cc

  \ingroup CR_Utilities

  \brief A collection of test routines for the MConversions class
 
  \author Lars B&auml;hren
 
  \date 2007/09/07
*/

//_______________________________________________________________________________
//                                                                test_MDirection

/*!
  \brief Test working with Measures of type MDirection

  \return nofFailedTests -- The number of failed tests encountered within this
          function
*/
int test_MDirection ()
{
  std::cout << "\n[tMConversions::test_MDirection]\n" << endl;
  
  int nofFailedTests (0);
  
  std::cout << "[1] Default constructors ..." << endl;
  try {
    casa::MDirection dir;
    //
    std::cout << "-- MDirection() = " << dir << endl;
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  std::cout << "[2] Argumented constructor for MDirection ..." << endl;
  try {
    MDirection azel (MDirection(Quantity(0,"deg"),
				Quantity(90,"deg"),
				MDirection::Ref(MDirection::AZEL)));
    MDirection b1950 (MDirection(Quantity(0,"deg"),
				 Quantity(90,"deg"),
				 MDirection::Ref(MDirection::B1950)));
    MDirection j2000 (MDirection(Quantity(0,"deg"),
				 Quantity(90,"deg"),
				 MDirection::Ref(MDirection::J2000)));
    //
    std::cout << "-- MDirection ( AZEL) = " << azel << endl;
    std::cout << "-- MDirection (B1950) = " << b1950 << endl;
    std::cout << "-- MDirection (J2000) = " << j2000 << endl;
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  std::cout << "[3] Get MDirection type from the reference code ..." << endl;
  try {
    std::cout << "-- AZEL  -> " << CR::MDirectionType ("AZEL") << endl;
    std::cout << "-- J2000 -> " << CR::MDirectionType ("J2000") << endl;
    std::cout << "-- ITRF  -> " << CR::MDirectionType ("ITRF") << endl;
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  std::cout << "[4] Get MDirection reference code from the type ..." << endl;
  try {
    std::cout << "-- AZEL  -> " << CR::MDirectionName (MDirection::AZEL) << endl;
    std::cout << "-- J2000 -> " << CR::MDirectionName (MDirection::J2000) << endl;
    std::cout << "-- JTRUE -> " << CR::MDirectionName (MDirection::JTRUE) << endl;
    std::cout << "-- JMEAN -> " << CR::MDirectionName (MDirection::JMEAN) << endl;
    std::cout << "-- ITRF  -> " << CR::MDirectionName (MDirection::ITRF) << endl;
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  return nofFailedTests;
}

//_______________________________________________________________________________
//                                                                 test_MPosition

/*!
  \brief Test working with Measures of type MPosition

  \return nofFailedTests -- The number of failed tests encountered within this
          function
*/
int test_MPosition ()
{
  std::cout << "\n[tMConversions::test_MPosition]\n" << endl;

  int nofFailedTests (0);

  std::cout << "[1] Default constructors ..." << endl;
  try {
    casa::MPosition pos;
    //
    std::cout << "-- MPosition()  = " << pos << endl;
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  std::cout << "[2] Argumented constructor for MPosition ..." << endl;
  try {
    MPosition pos_wgs84 (MVPosition(Quantity(25, "m"),     // Height
				    Quantity(20, "deg"),   // East longitude
				    Quantity(53, "deg")),  // Lattitude
			 MPosition::WGS84);                // Reference type
    // 45 foot diameter antenna, Green Bank, West Virginia
    MPosition pos_itrf (MVPosition(884084.2636,     // X
				   -4924578.7481,   // Y
				   3943734.3354),   // Z
			MPosition::ITRF);           // Reference type
    //
    std::cout << "-- MPosition (WGS84) = " << pos_wgs84 << endl;
    std::cout << "-- MPosition (ITRF)  = " << pos_itrf  << endl;
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}

//_______________________________________________________________________________
//                                                               test_conversions

int test_conversions ()
{
  std::cout << "\n[tMConversions::test_conversions]\n" << endl;

  int nofFailedTests (0);

  return nofFailedTests;
}

//_______________________________________________________________________________
//                                                                   main routine

int main ()
{
  int nofFailedTests (0);

  nofFailedTests += test_MDirection();
  nofFailedTests += test_MPosition();
  nofFailedTests += test_conversions ();

  return nofFailedTests;
}
