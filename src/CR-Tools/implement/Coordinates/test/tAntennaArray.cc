/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2009                                                    *
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

#include <Coordinates/AntennaArray.h>

// Namespace usage
using CR::AntennaArray;
using casa::Quantity;

/*!
  \file tAntennaArray.cc

  \ingroup CR_Coordinates

  \brief A collection of test routines for the AntennaArray class
 
  \author Lars B&auml;hren
 
  \date 2009/02/27
*/

//_______________________________________________________________________________
//                                                              test_constructors

/*!
  \brief Test constructors for a new AntennaArray object

  \return nofFailedTests -- The number of failed tests encountered within this
          function.
*/
int test_constructors ()
{
  std::cout << "\n[tAntennaArray::test_constructors]\n" << std::endl;

  int nofFailedTests (0);
  
  std::cout << "[1] Testing default constructor ..." << std::endl;
  try {
    AntennaArray arr;
    //
    arr.summary(); 
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  std::cout << "[2] Testing argumented constructor ..." << std::endl;
  try {
    // Arguments
    std::string telescope ("LOPES");
    casa::MPosition location ( casa::MVPosition(Quantity(100,"m"),
						Quantity(8.437484,"deg"),
						Quantity(49.099477, "deg")),
			       casa::MPosition::WGS84);    
    // Object construction
    AntennaArray arr;
    //
    arr.summary(); 
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}

//_______________________________________________________________________________
//                                                                   test_methods

/*!
  \brief Test the various methods of an AntennaArray object

  \return nofFailedTests -- The number of failed tests encountered within this
          function.
*/
int test_methods ()
{
  std::cout << "\n[tAntennaArray::test_methods]\n" << std::endl;

  int nofFailedTests (0);
  
  
  return nofFailedTests;
}

//_______________________________________________________________________________

int main ()
{
  int nofFailedTests (0);

  // Test for the constructor(s)
  nofFailedTests += test_constructors ();
  // Test for the methods
  nofFailedTests += test_methods ();

  return nofFailedTests;
}
