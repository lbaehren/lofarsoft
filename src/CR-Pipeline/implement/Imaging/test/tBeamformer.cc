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
#include <templates.h>

using casa::DComplex;
using casa::Matrix;
using casa::Vector;
using CR::Beamformer;

/*!
  \file tBeamformer.cc

  \ingroup Imaging

  \brief A collection of test routines for Beamformer
 
  \author Lars B&auml;hren
 
  \date 2007/06/13
*/

// -----------------------------------------------------------------------------

/*!
  \brief Create some antenna positions for the Beamformer

  \param nofAntennas -- The number of antennas

  \return positions -- [antenna,coordinate] Antenna positions, \f$ \vec x \f$
*/
Matrix<double> get_antennaPositions (uint const &nofAntennas=3)
{
  uint nofAxes (3);
  Matrix<double> positions (nofAntennas,nofAxes,0.0);

  for (uint n(0); n<nofAntennas; n++) {
    positions(n,0) = positions(n,1) = n*100;
  }

  return positions;
}

// -----------------------------------------------------------------------------

/*!
  \brief Create some sky/pointing positions for the Beamformer

  \param nofSkyPositions -- The number of positions on the sky to which to point
                            to

  \return positions -- [position,coordinate] Sky positions, \f$ \vec\rho \f$
*/
Matrix<double> get_skyPositions (uint const &nofSkyPositions=3)
{
  uint nofAxes (3);
  Matrix<double> positions (nofSkyPositions,nofAxes,0.0);

  for (uint n(0); n<nofSkyPositions; n++) {
    positions(n,0) = positions(n,2) = (n+1)*1000;
  }

  return positions;
}

// -----------------------------------------------------------------------------

/*!
  \brief Create frequency band values for the Beamformer

  \param freqMin     -- Lower limit of the frequency band, [Hz]
  \param freqMax     -- Upper limit of the frequency band, [Hz]
  \param nofChannels -- Number of frequency channels, into which the band is
                        split.

  \return frequencies -- 
*/
Vector<double> get_frequencies (double const &freqMin=40e06,
				double const &freqMax=80e06,
				uint const &nofChannels=4096)
{
  Vector<double> channels (nofChannels);
  double incr ((freqMax-freqMin)/(nofChannels+1));

  for (uint k(0); k<nofChannels; k++) {
    channels(k) = freqMin+k*incr;
  }

  return channels;
}

// -----------------------------------------------------------------------------

/*!
  \brief Create some data for the Beamformer to process

  \param nofAntennas -- The number of antennas
  \param nofChannels -- Number of frequency channels, into which the band is
                        split.

  \return data -- [antenna,channel]
*/
Matrix<DComplex> get_data (uint const &nofAntennas=3,
			   uint const &nofChannels=4096)
{
  // Indices
  uint antenna (0);
  uint channel (0);
  // Array of generated data
  Matrix<DComplex> data (nofAntennas,nofChannels,0.0);
  // Random number generator from CASA scimath/Mathematics module
  casa::ACG gen(10, 20);
  casa::Uniform random (&gen);

  for (antenna=0; antenna<nofAntennas; antenna++) {
    for (channel=0; channel<nofChannels; channel++) {
      data(antenna,channel) = DComplex(random(),random());
    }
  }

  return data;
}

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
  
  std::cout << "[2] Testing argumented constructor ..." << std::endl;
  try {
    Beamformer bf (get_antennaPositions(),
		   get_skyPositions(),
		   get_frequencies());
    bf.summary();
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  std::cout << "[3] Testing copy constructor ..." << std::endl;
  try {
    Beamformer bf1 (get_antennaPositions(),
		    get_skyPositions(),
		    get_frequencies());
    //
    bf1.bufferDelays (true);
    //
    Beamformer bf2 (bf1);
    bf2.summary();
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
		 get_skyPositions(),
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
		 get_skyPositions(),
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
