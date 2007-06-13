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

/* $Id$*/

#include <Imaging/Beam.h>

using std::cerr;
using std::cout;
using std::endl;
using CR::Beam;

/*!
  \file tBeam.cc

  \ingroup Imaging

  \brief A collection of test routines for Beam
 
  \author Lars B&auml;hren
 
  \date 2007/01/29
*/

// -----------------------------------------------------------------------------

/*!
  \brief Test constructors for a new Beam object

  \return nofFailedTests -- The number of failed tests.
*/
int test_Beam ()
{
  int nofFailedTests (0);
  
  std::cout << "\n[test_Beam]\n" << std::endl;

  std::cout << "[1] Testing default constructor ..." << std::endl;
  try {
    Beam newObject;
  } catch (std::string message) {
    cerr << message << endl;
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
    nofFailedTests += test_Beam ();
  }

  return nofFailedTests;
}
