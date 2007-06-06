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

/* $Id: template-tclass.cc,v 1.6 2006/09/20 09:56:53 bahren Exp $*/

#include <iostream>

#include <Imaging/GeometricalPhase.h>

using CR::GeometricalPhase;

/*!
  \file tGeometricalPhase.cc

  \ingroup Imaging

  \brief A collection of test routines for GeometricalPhase
 
  \author Lars Baehren
 
  \date 2007/01/15
*/

// -----------------------------------------------------------------------------

/*!
  \brief Test constructors for a new GeometricalPhase object

  \return nofFailedTests -- The number of failed tests.
*/
int test_GeometricalPhase ()
{
  int nofFailedTests (0);
  
  std::cout << "\n[test_GeometricalPhase]\n" << std::endl;

  std::cout << "[1] Testing default constructor ..." << std::endl;
  try {
    GeometricalPhase phase;
    phase.summary();
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  std::cout << "[2] Testing argumented constructor ..." << std::endl;
#ifdef HAVE_CASA
  try {
    GeometricalPhase phase;
    phase.summary();
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
#else 
#ifdef HAVE_BLITZ
  try {
    GeometricalPhase phase;
    phase.summary();
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
#endif
#endif
  
  return nofFailedTests;
}

// -----------------------------------------------------------------------------

int main ()
{
  int nofFailedTests (0);

  // Test for the constructor(s)
  {
    nofFailedTests += test_GeometricalPhase ();
  }

  return nofFailedTests;
}
