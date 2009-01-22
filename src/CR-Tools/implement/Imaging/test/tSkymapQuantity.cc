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

#include <Imaging/SkymapQuantity.h>

// Namespace usage
using std::endl;
using CR::SkymapQuantity;

/*!
  \file tSkymapQuantity.cc

  \ingroup Imaging

  \brief A collection of test routines for the SkymapQuantity class
 
  \author Lars B&auml;hren
 
  \date 2009/01/13
*/

// -----------------------------------------------------------------------------

/*!
  \brief Test constructors for a new SkymapQuantity object

  \return nofFailedTests -- The number of failed tests encountered within this
          function.
*/
int test_constructors ()
{
  std::cout << "\n[tSkymapQuantity::test_constructors]\n" << endl;

  int nofFailedTests (0);
  
  std::cout << "[1] Testing default constructor ..." << endl;
  try {
    SkymapQuantity newObject;
    //
    newObject.summary(); 
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  std::cout << "[2] Testing argumented constructor ..." << endl;
  try {
    SkymapQuantity newObject (SkymapQuantity::TIME_CC);
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
  \brief Test the various methods

  \return nofFailedTests -- The number of failed tests encountered within this
          function.
*/
int test_methods ()
{
  std::cout << "\n[tSkymapQuantity::test_methods]\n" << endl;

  int nofFailedTests (0);
  SkymapQuantity mapQuant (SkymapQuantity::TIME_CC);

  std::cout << "[1] Retrieve values of the internal parameters..." << endl;
  try {
    std::cout << "-- Class name          = " << mapQuant.className()  << endl;
    std::cout << "-- Domain type         = " << mapQuant.domainType() << endl;
    std::cout << "-- Domain name         = " << mapQuant.domainName() << endl;
    std::cout << "-- Electrical quantity = " << mapQuant.quantity()   << endl;
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
  // Test the various methods
  nofFailedTests =+ test_methods();

  return nofFailedTests;
}
