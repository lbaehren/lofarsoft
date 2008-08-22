/***************************************************************************
 *   Copyright (C) 2007                                                    *
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

/* $Id: template-tclass.cc,v 1.7 2007/03/06 14:53:26 bahren Exp $*/

#include <Math/RotationMatrix3D.h>

using CR::RotationMatrix3D;

/*!
  \file tRotationMatrix3D.cc

  \ingroup CR_Math

  \brief A collection of test routines for RotationMatrix3D
 
  \author Lars Baehren
 
  \date 2007/05/29
*/

// -----------------------------------------------------------------------------

/*!
  \brief Test constructors for a new RotationMatrix3D object

  \return nofFailedTests -- The number of failed tests.
*/
int test_RotationMatrix3D ()
{
  int nofFailedTests (0);
  
  std::cout << "\n[test_RotationMatrix3D]\n" << std::endl;

  std::cout << "[1] Testing default constructor ..." << std::endl;
  try {
    RotationMatrix3D newObject;
    newObject.summary();
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
    nofFailedTests += test_RotationMatrix3D ();
  }

  return nofFailedTests;
}
