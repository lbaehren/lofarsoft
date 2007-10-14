/*-------------------------------------------------------------------------*
 | $Id                                                                     |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2007                                                  *
 *   Andreas Horneffer (<mail>)                                                     *
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

#include <IO/TSmatrix2Event.h>

using CR::TSmatrix2Event;  // Namespace usage

/*!
  \file tTSmatrix2Event.cc

  \ingroup IO

  \brief A collection of test routines for the TSmatrix2Event class
 
  \author Andreas Horneffer
 
  \date 2007/10/12
*/

// -----------------------------------------------------------------------------

/*!
  \brief Test constructors for a new TSmatrix2Event object

  \return nofFailedTests -- The number of failed tests.
*/
int test_TSmatrix2Event ()
{
  int nofFailedTests (0);
  
  std::cout << "\n[test_TSmatrix2Event]\n" << std::endl;

  std::cout << "[1] Testing default constructor ..." << std::endl;
  try {
    TSmatrix2Event newTSmatrix2Event;
    //
    newTSmatrix2Event.summary(); 
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
    nofFailedTests += test_TSmatrix2Event ();
  }

  return nofFailedTests;
}
