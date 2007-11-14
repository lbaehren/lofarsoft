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

#include <iostream>

#include "create_data.h"
#include <Imaging/GeometricalPhase.h>

using std::cout;

using CR::GeometricalPhase;

/*!
  \file tGeometricalPhase.cc

  \ingroup CR
  \ingroup Imaging

  \brief A collection of test routines for GeometricalPhase
 
  \author Lars B&auml;hren
 
  \date 2007/01/15
*/

// -----------------------------------------------------------------------------

void summary (GeometricalPhase &phase)
{
  cout << "-- Frequencies       : " << phase.frequencies()      << std::endl;
  cout << "-- Antenna positions : " << phase.antennaPositions() << std::endl;
  cout << "-- Sky positions     : " << phase.skyPositions()     << std::endl;
  cout << "-- Phases            : " << phase.phases()           << std::endl;
}

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
    // summary of object's internal settings
    phase.summary();
    // retrieve phases
    std::cout << phase.delays() << std::endl; 
    std::cout << phase.phases() << std::endl; 
 } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  std::cout << "[2] Testing simple argumented constructor ..." << std::endl;
  try {
    casa::Vector<double> frequencies = get_frequencies (20);

    GeometricalPhase phase (frequencies);
    // summary of object's internal settings
    phase.summary();
    // retrieve phases
    std::cout << phase.delays() << std::endl; 
    std::cout << phase.phases() << std::endl;
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

  std::cout << "[3] Testing argumented constructor ..." << std::endl;
  try {
    casa::Matrix<double> antennaPositions = get_antennaPositions();
    casa::Matrix<double> skyPositions = get_skyPositions();
    casa::Vector<double> frequencies = get_frequencies (20);

    std::cout << "-- antenna positions array = " << antennaPositions.shape()
	      << std::endl;
    std::cout << "-- sky positions array     = " << skyPositions.shape() << std::endl;
    std::cout << "-- frequency array         = " << frequencies.shape()  << std::endl;

    GeometricalPhase phase (antennaPositions,
			    skyPositions,
			    frequencies);
    // summary of object's internal settings
    phase.summary();
    // show geometrical phases for closer inspection
    std::cout << phase.delays() << std::endl; 
    std::cout << phase.phases() << std::endl;
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
    nofFailedTests += test_GeometricalPhase ();
  }

  return nofFailedTests;
}
