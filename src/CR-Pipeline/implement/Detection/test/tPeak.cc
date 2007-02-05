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

#include <string>
#include <iostream>

#include <Detection/Peak.h>

/*!
  \file tPeak.cc

  \ingroup Detection

  \brief A collection of test routines for Peak
 
  \author Lars B&auml;hren
 
  \date 2007/02/05
*/

// -----------------------------------------------------------------------------

void show_peak (CR::Peak &peak)
{
  std::cout << " - time        = " << peak.time()         << std::endl;
  std::cout << " - duration    = " << peak.duration()     << std::endl;
  std::cout << " - height      = " << peak.height()       << std::endl;
  std::cout << " - significane = " << peak.significance() << std::endl;
}

// -----------------------------------------------------------------------------

/*!
  \brief Test constructors for a new Peak object

  \return nofFailedTests -- The number of failed tests.
*/
int test_Peak ()
{
  std::cout << "\n[test_Peak]\n" << std::endl;

  int nofFailedTests (0);

  std::cout << "[1] Testing default constructor ..." << std::endl;
  try {
    CR::Peak peak;
    // show contents
    show_peak (peak);
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  std::cout << "[2] Testing argumented constructor ..." << std::endl;
  try {
    uint time (2);
    int height (10);
    uint sigma (3);
    
    CR::Peak peak (time, height, sigma);
    
    show_peak (peak);
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
    nofFailedTests += test_Peak ();
  }

  return nofFailedTests;
}
