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

#include <Imaging/Beamformer.h>
#include <templates.h>

using CR::Beamformer;

/*!
  \file tBeamformer.cc

  \ingroup Imaging

  \brief A collection of test routines for Beamformer
 
  \author Lars Baehren
 
  \date 2007/06/13
*/

// -----------------------------------------------------------------------------

/*!
  \brief Create some antenna positions for the Beamformer

  \param nofAntennas -- The number of antennas

  \return positions -- 
*/
casa::Matrix<double> get_antennaPositions (uint const &nofAntennas=3)
{
  uint nofAxes (3);
  casa::Matrix<double> positions (nofAntennas,nofAxes,0.0);

  for (uint n(0); n<nofAntennas; n++) {
    positions(n,0) = positions(n,1) = n*100;
  }

  return positions;
}

/*!
  \brief Create some sky/pointing positions for the Beamformer

  \param nofSkyPositions -- The number of positions on the sky to which to point
                            to

  \return positions -- 
*/
casa::Matrix<double> get_skyPositions (uint const &nofSkyPositions=3)
{
  uint nofAxes (3);
  casa::Matrix<double> positions (nofSkyPositions,nofAxes,0.0);

  for (uint n(0); n<nofSkyPositions; n++) {
    positions(n,0) = positions(n,2) = (n+1)*1000;
  }

  return positions;
}

casa::Vector<double> get_frequencies (double const &freqMin=40e06,
				      double const &freqMax=80e06,
				      uint const &nofChannels=4096)
{
  casa::Vector<double> channels (nofChannels);
  double incr ((freqMax-freqMin)/(nofChannels+1));

  for (uint k(0); k<nofChannels; k++) {
    channels(k) = freqMin+k*incr;
  }

  return channels;
}

// -----------------------------------------------------------------------------

/*!
  \brief Test constructors for a new Beamformer object

  \return nofFailedTests -- The number of failed tests.
*/
int test_Beamformer ()
{
  int nofFailedTests (0);
  
  std::cout << "\n[test_Beamformer]\n" << std::endl;

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

int test_processing ()
{
  int nofFailedTests (0);

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

  if (nofFailedTests) {
    nofFailedTests += test_processing ();
  }

  return nofFailedTests;
}
