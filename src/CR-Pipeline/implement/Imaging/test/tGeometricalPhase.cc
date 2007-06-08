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

  \ingroup CR
  \ingroup Imaging

  \brief A collection of test routines for GeometricalPhase
 
  \author Lars B&auml;hren
 
  \date 2007/01/15
*/

// -----------------------------------------------------------------------------

void summary (GeometricalPhase &phase)
{
  std::cout << "-- Frequencies       : " << phase.frequencies()  << std::endl;
  std::cout << "-- Antenna positions : " << phase.antPositions() << std::endl;
  std::cout << "-- Sky positions     : " << phase.skyPositions() << std::endl;
  std::cout << "-- Phases            : " << phase.phases()       << std::endl;
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
    phase.summary();
    summary(phase);
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  std::cout << "[2] Testing argumented constructor ..." << std::endl;
#ifdef HAVE_CASA
  try {
    unsigned int nofChannels (10);
    casa::Vector<double> frequencies (nofChannels);

    for (unsigned int n(0); n<nofChannels; n++) {
      frequencies(n) = n*1.0e06;
    }

    GeometricalPhase phase (frequencies);
    phase.summary();
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
#else 
#ifdef HAVE_BLITZ
  try {
    unsigned int nofChannels (10);
    blitz::Array<double,1> frequencies (nofChannels);

    for (unsigned int n(0); n<nofChannels; n++) {
      frequencies(n) = n*1.0e06;
    }

    GeometricalPhase phase (frequencies);
    phase.summary();
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
#endif
#endif

  std::cout << "[3] Testing argumented constructor ..." << std::endl;
#ifdef HAVE_CASA
  try {
    unsigned int nofCoordinates (3);
    unsigned int nofAntennas (2);
    unsigned int nofPositions (2);
    unsigned int nofChannels (10);
    casa::Matrix<double> antennaPositions (nofAntennas,nofCoordinates);
    casa::Matrix<double> skyPositions (nofPositions,nofCoordinates);
    casa::Vector<double> frequencies (nofChannels);

    for (unsigned int n(0); n<nofChannels; n++) {
      frequencies(n) = n*1.0e06;
    }

    GeometricalPhase phase (frequencies);
    phase.summary();
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
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
