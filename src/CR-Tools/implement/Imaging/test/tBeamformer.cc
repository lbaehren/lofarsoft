/*-------------------------------------------------------------------------*
 | $Id$ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
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

#include <casa/BasicMath/Random.h>

#include <Imaging/Beamformer.h>
#include "create_data.h"

using casa::DComplex;
using casa::Matrix;
using casa::Vector;

using CR::GeometricalDelay;
using CR::GeometricalPhase;
using CR::GeometricalWeight;
using CR::Beamformer;

/*!
  \file tBeamformer.cc

  \ingroup Imaging

  \brief A collection of test routines for Beamformer
 
  \author Lars B&auml;hren
 
  \date 2007/06/13
*/

// ==============================================================================
//
//  Test functions
//
// ==============================================================================

/*!
  \brief Test constructors for a new Beamformer object

  \return nofFailedTests -- The number of failed tests.
*/
int test_Beamformer ()
{
  std::cout << "\n[test_Beamformer]\n" << std::endl;

  int nofFailedTests (0);
  
  std::cout << "[1] Testing default constructor ..." << std::endl;
  try {
    Beamformer bf;
    bf.summary();
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  std::cout << "[2] Testing construction with GeometricalWeight ..." << std::endl;
  try {
    std::cout << "-- creating GeometricalWeight object ..." << std::endl;
    GeometricalWeight weight (get_antennaPositions(),
			      get_skyPositions(),
			      get_frequencies());
    
    std::cout << "-- creating Beamformer object ..." << std::endl;
    Beamformer bf (weight);
    bf.summary();
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  std::cout << "[3] Testing construction with GeometricalPhase ..." << std::endl;
  try {
    std::cout << "-- creating GeometricalPhase object ..." << std::endl;
    GeometricalPhase phase (get_antennaPositions(),
			    get_skyPositions(),
			    get_frequencies());
    
    std::cout << "-- creating Beamformer object ..." << std::endl;
    Beamformer bf (phase);
    bf.summary();
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  std::cout << "[4] Testing construction with GeometricalDelay ..." << std::endl;
  try {
    std::cout << "-- creating GeometricalDelay object ..." << std::endl;
    GeometricalDelay delay (get_antennaPositions(),
			    get_skyPositions());
    
    std::cout << "-- creating Beamformer object ..." << std::endl;
    Beamformer bf (delay,
		   get_frequencies());
    bf.summary();
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  std::cout << "[5] Testing argumented constructor ..." << std::endl;
  try {
    Beamformer bf (get_antennaPositions(),
		   CR::Cartesian,
		   get_skyPositions(),
		   CR::Cartesian,
		   get_frequencies());
    bf.summary();
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  std::cout << "[4] Testing copy constructor ..." << std::endl;
  try {
    std::cout << "-- creating original Beamformer object ..." << std::endl;
    Beamformer bf (get_antennaPositions(),
		   CR::Cartesian,
		   get_skyPositions(),
		   CR::Cartesian,
		   get_frequencies());
    //
    bf.bufferDelays (true);
    //
    std::cout << "-- creating new Beamformer object as copy ..." << std::endl;
    Beamformer bfCopy (bf);
    bfCopy.summary();
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}

// -----------------------------------------------------------------------------

/*!
  \brief Test assigning different beam-types for the handling of the data

  \return nofFailedTests -- The number of failed tests.
*/
int test_beamType ()
{
  std::cout << "\n[test_beamType]\n" << std::endl;

  int nofFailedTests (0);
  bool ok (true);

  // Create Beamformer object for testing its functionality
  Beamformer bf (get_antennaPositions(),
		 CR::Cartesian,
		 get_skyPositions(),
		 CR::Cartesian,
		 get_frequencies());

  std::cout << "[1] Field in the frequency domain..." << std::endl;
  ok = bf.setBeamType (CR::FREQ_FIELD);
  
  std::cout << "[2] Power in the frequency domain..." << std::endl;
  ok = bf.setBeamType (CR::FREQ_POWER);
  
  std::cout << "[3] Field in the time domain..." << std::endl;
  ok = bf.setBeamType (CR::TIME_FIELD);
  
  std::cout << "[4] Power in the time domain..." << std::endl;
  ok = bf.setBeamType (CR::TIME_POWER);
  
  std::cout << "[5] cc-beam in the time domain..." << std::endl;
  ok = bf.setBeamType (CR::TIME_CC);
  
  std::cout << "[6] Power-beam in the time domain..." << std::endl;
  ok = bf.setBeamType (CR::TIME_P);
  
  std::cout << "[7] Excess-beam in the time domain..." << std::endl;
  ok = bf.setBeamType (CR::TIME_X);
  
  return nofFailedTests;
}

// -----------------------------------------------------------------------------

/*!
  \brief Test processing of incoming data

  Test the <tt>processData</tt> routines, which is responsible for converting
  incoming antennas (in the frequency domain) into beamformed data.

  \return nofFailedTests -- The number of failed tests.
*/
int test_processing ()
{
  std::cout << "\n[test_processing]\n" << std::endl;

  int nofFailedTests (0);
  bool status (true);
  Matrix<double> beam;
  // Beamformer object for the subsequent testing
  Beamformer bf (get_antennaPositions(),
		 CR::Cartesian,
		 get_skyPositions(),
		 CR::Cartesian,
		 get_frequencies());
  bf.summary();
  // Some data to test the processing
  Matrix<DComplex> data (get_data());

  std::cout << "[1] Power in the frequency domain (FREQ_POWER)" << std::endl;
  try {
    bf.setBeamType(CR::FREQ_POWER);
    status = bf.processData (beam,data);
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  std::cout << "[2] cc-beam in the time domain (TIME_CC)" << std::endl;
  try {
    bf.setBeamType(CR::TIME_CC);
    status = bf.processData (beam,data);
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
    nofFailedTests += test_Beamformer ();
  }

  if (!nofFailedTests) {
    nofFailedTests += test_beamType ();
    nofFailedTests += test_processing ();
  }

  return nofFailedTests;
}
