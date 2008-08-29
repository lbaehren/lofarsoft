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

#include <Coordinates/CoordinateTypes.h>

// Namespace usage
using CR::CoordinateType;

/*!
  \file tCoordinateTypes.cc

  \ingroup CR_Coordinates

  \brief A collection of test routines for the CoordinateTypes class
 
  \author Lars B&auml;hren
 
  \date 2008/08/29
*/

// -----------------------------------------------------------------------------

/*!
  \brief Test constructors for a new CoordinateType object

  \return nofFailedTests -- The number of failed tests.
*/
int test_CoordinateType ()
{
  int nofFailedTests (0);
  
  std::cout << "\n[test_CoordinateType]\n" << std::endl;

  std::cout << "[1] Testing default constructor ..." << std::endl;
  try {
    CoordinateType newCoordinateType;
    //
    newCoordinateType.summary(); 
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
  {
    nofFailedTests += test_CoordinateType ();
  }

  return nofFailedTests;
}
