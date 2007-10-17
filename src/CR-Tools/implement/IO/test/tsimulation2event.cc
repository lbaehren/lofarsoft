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

#include <IO/simulation2event.h>

using CR::simulation2event;  // Namespace usage

using namespace CR;

/*!
  \file tsimulation2event.cc

  \ingroup IO

  \brief A collection of test routines for the simulation2event class
 
  \author Andreas Horneffer
 
  \date 2007/10/17
*/

// -----------------------------------------------------------------------------

/*!
  \brief Test constructors for a new simulation2event object

  \return nofFailedTests -- The number of failed tests.
*/
int test_simulation2event ()
{
  int nofFailedTests (0);
  
  std::cout << "\n[test_simulation2event]\n" << std::endl;

  std::cout << "[1] Testing default constructor ..." << std::endl;
  try {
    simulation2event newsimulation2event;
    //

    CalTableReader CTread("/home/horneff/lopescasa/data/LOPES/LOPES-CalTable");
    newsimulation2event.setCalTable(&CTread);

    std::cout << "[2] Generating test event ..." << std::endl;
    newsimulation2event.generateEvent("tsimulation2event.event","30deg1e17_shift",
				      "lopes30", "/home/horneff/testreas",
				      1104580800, 32768, True, True);

    newsimulation2event.summary(); 
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
    nofFailedTests += test_simulation2event ();
  }

  return nofFailedTests;
}
