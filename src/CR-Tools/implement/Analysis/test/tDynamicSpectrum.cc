/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
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

#include <Analysis/DynamicSpectrum.h>

// Namespace usage
using CR::DynamicSpectrum;

/*!
  \file tDynamicSpectrum.cc

  \ingroup CR_Analysis

  \brief A collection of test routines for the DynamicSpectrum class
 
  \author Lars B&auml;hren
 
  \date 2008/03/10
*/

// -----------------------------------------------------------------------------

/*!
  \brief Test constructors for a new DynamicSpectrum object

  \return nofFailedTests -- The number of failed tests.
*/
int test_construction ()
{
  std::cout << "\n[test_construction]\n" << std::endl;
  
  int nofFailedTests (0);
  
  std::cout << "[1] Testing default constructor ..." << std::endl;
  try {
    DynamicSpectrum ds;
    //
    ds.summary(); 
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  std::cout << "[2] Testing argumented constructor ..." << std::endl;
  try {
    // Coordinate axes
    uint nofAxes (2);
    casa::Vector<double> crval (nofAxes);
    casa::Vector<double> cdelt (nofAxes);
    casa::Vector<casa::String> units (nofAxes);

    crval    = 0.0;
    cdelt    = 1.0;
    units(0) = "Hz";
    units(1) = "s";
    // create object
    DynamicSpectrum ds (crval,cdelt,units);
    //
    ds.summary(); 
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
  nofFailedTests += test_construction ();
  
  return nofFailedTests;
}
