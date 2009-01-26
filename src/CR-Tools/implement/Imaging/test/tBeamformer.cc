/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
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

using CR::GeomDelay;
using CR::GeomPhase;
using CR::GeomWeight;
using CR::Beamformer;
using CR::SkymapQuantity;

/*!
  \file Imaging/test/tBeamformer.cc

  \ingroup CR_Imaging

  \brief A collection of test routines for the Beamformer class
 
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
  CR::Beamformer bf (get_antennaPositions(),
		     CR::CoordinateType::Cartesian,
		     get_skyPositions(),
		     CR::CoordinateType::Cartesian,
		     false,
		     false,
		     false,
		     get_frequencies(),
		     false);

  std::cout << "[1] Field in the frequency domain..." << std::endl;
  ok = bf.setSkymapType (SkymapQuantity::FREQ_FIELD);
  
  std::cout << "[2] Power in the frequency domain..." << std::endl;
  ok = bf.setSkymapType (SkymapQuantity::FREQ_POWER);
  
  std::cout << "[3] Field in the time domain..." << std::endl;
  ok = bf.setSkymapType (SkymapQuantity::TIME_FIELD);
  
  std::cout << "[4] Power in the time domain..." << std::endl;
  ok = bf.setSkymapType (SkymapQuantity::TIME_POWER);
  
  std::cout << "[5] cc-beam in the time domain..." << std::endl;
  ok = bf.setSkymapType (SkymapQuantity::TIME_CC);
  
  std::cout << "[6] Power-beam in the time domain..." << std::endl;
  ok = bf.setSkymapType (SkymapQuantity::TIME_P);
  
  std::cout << "[7] Excess-beam in the time domain..." << std::endl;
  ok = bf.setSkymapType (SkymapQuantity::TIME_X);
  
  return nofFailedTests;
}

// -----------------------------------------------------------------------------

/*!
  \brief Test modification of the Beamformer weights

  \return nofFailedTests -- The number of failed tests.
*/
int test_weights ()
{
  std::cout << "\n[test_weights]\n" << std::endl;

  int nofFailedTests (0);

  /* Create beamformer object to work with */
  CR::Beamformer bf (get_antennaPositions(),
		     CR::CoordinateType::Cartesian,
		     get_skyPositions(),
		     CR::CoordinateType::Cartesian,
		     false,
		     false,
		     false,
		     get_frequencies(),
		     false);

  /* Work with default settings (geometrical weights only) */
  try {
    bf.summary();
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

  /* Redo computation after setting vales for the antenna gains */
  try {
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

  /* Unset effect of the antenna gains, reverting to geometrical weights only */
  try {
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

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

  uint nofAntennas (4);

  // Beamformer object for the subsequent testing
  CR::Beamformer bf (get_antennaPositions(),
		     CR::CoordinateType::Cartesian,
		     get_skyPositions(),
		     CR::CoordinateType::Cartesian,
		     false,
		     false,
		     false,
		     get_frequencies(),
		     false);
  bf.summary();

  // Some data to test the processing
  Matrix<DComplex> data (get_data(nofAntennas));
  // Array to store the beamformed data
  Matrix<double> beam;

  std::cout << "[1] Power in the frequency domain (FREQ_POWER)" << std::endl;
  try {
    bf.setSkymapType(SkymapQuantity::FREQ_POWER);
    bf.summary();
    status = bf.processData (beam,data);
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  std::cout << "[2] cc-beam in the time domain (TIME_CC)" << std::endl;
  try {
    bf.setSkymapType(SkymapQuantity::TIME_CC);
    bf.summary();
    status = bf.processData (beam,data);
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  std::cout << "[3] powerbeam in the time domain (TIME_P)" << std::endl;
  try {
    bf.setSkymapType(SkymapQuantity::TIME_P);
    bf.summary();
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
  nofFailedTests += test_Beamformer ();
  
  if (!nofFailedTests) {
    nofFailedTests += test_beamType ();
    nofFailedTests += test_processing ();
  }
  
  return nofFailedTests;
}
