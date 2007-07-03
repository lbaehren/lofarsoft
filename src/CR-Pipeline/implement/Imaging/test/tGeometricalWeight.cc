/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2007                                                    *
 *   Lars Baehren (bahren@astron.nl)                                       *
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

#include <Imaging/GeometricalWeight.h>

using CR::GeometricalWeight;

/*!
  \file tGeometricalWeight.cc

  \ingroup Imaging

  \brief A collection of test routines for GeometricalWeight
 
  \author Lars B&auml;hren
 
  \date 2007/01/15
*/

// -----------------------------------------------------------------------------

#ifdef HAVE_CASA
void getPositions (casa::Matrix<double> &skyPositions,
		   casa::Matrix<double> &antPositions)
{
  int nofCoordinates (3);
  int nofAntennas (2);
  int nofDirections (3);

  // Antenna positions

  antPositions.resize(nofAntennas,nofCoordinates);

  antPositions(0,0) = -100.0;
  antPositions(0,1) = 0.0;
  antPositions(0,2) = 0.0;
  antPositions(1,0) = 100.0;
  antPositions(1,1) = 0.0;
  antPositions(1,2) = 0.0;

  // Pointing directions (sky positions)
  
  skyPositions.resize(nofDirections,nofCoordinates);

  skyPositions            = 0.0;
  skyPositions.diagonal() = 100.0;

}

/*!
  \brief Get the frequency values for which the weights are computed

  \retval frequencies -- The frequency values for which the weights are computed,
                         [Hz]
  \param minFreq     -- Lower limit of the frequency range, [Hz]
  \param maxFreq     -- Upper limit of the frequency range, [Hz]
  \param nofChannels -- Number of channels, into which the frequency range is 
                        splitted.
*/
void getFrequencies (casa::Vector<double> &frequencies,
		     double const &minFreq=40e06,
		     double const &maxFreq=80e06,
		     int const &nofChannels=1024)
{
  double increment (0);

  increment = (maxFreq-minFreq)/(nofChannels+1.0);

  frequencies.resize (nofChannels);

  for (int n(0); n<nofChannels; n++) {
    frequencies(n) = minFreq + n*increment;
  }
}
#endif

// -----------------------------------------------------------------------------

/*!
  \brief Test constructors for a new GeometricalWeight object

  \return nofFailedTests -- The number of failed tests.
*/
int test_GeometricalWeight ()
{
  std::cout << "\n[test_GeometricalWeight]\n" << std::endl;

  int nofFailedTests (0);
  casa::Matrix<double> antennaPositions;
  casa::Matrix<double> skyPositions;
  casa::Vector<double> frequencies;
  
  std::cout << "[1] Testing default constructor ..." << std::endl;
  try {
    GeometricalWeight weight;
    weight.summary();
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  std::cout << "[2] Testing argumented constructor ..." << std::endl;
  try {
    // retrieve the parameters required for construction
    getPositions (skyPositions,antennaPositions);
    getFrequencies (frequencies);
    // Create new object
    GeometricalWeight weight (antennaPositions,
			      skyPositions,
			      frequencies);
    weight.summary();
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}

// -----------------------------------------------------------------------------

/*!
  \brief Test setting and retrieving of the various parameters

  \return nofFailedTests -- The number of failed tests.
*/
int test_parameters ()
{
  std::cout << "\n[test_parameters]\n" << std::endl;

  int nofFailedTests (0);
  casa::Matrix<double> antennaPositions;
  casa::Matrix<double> skyPositions;
  casa::Vector<double> frequencies;
  
  getPositions (skyPositions,antennaPositions);
  getFrequencies (frequencies);

  std::cout << "[1] Creating new object via default constructor..." << std::endl;
  GeometricalWeight w;
  w.summary();
  {
    casa::Cube<casa::DComplex> weights = w.weights();
    std::cout << "-- Weighting factors : " << weights.shape() << std::endl;
  }

  std::cout << "[2] Setting antenna positions..." << std::endl;
  w.setAntPositions (antennaPositions,false);
  w.summary();
  {
    casa::Cube<casa::DComplex> weights = w.weights();
    std::cout << "-- Weighting factors : " << weights.shape() << std::endl;
  }
  
  std::cout << "[3] Setting sky positions..." << std::endl;
  w.setSkyPositions (skyPositions,false);
  w.summary();
  {
    casa::Cube<casa::DComplex> weights = w.weights();
    std::cout << "-- Weighting factors : " << weights.shape() << std::endl;
  }

  std::cout << "[4] Setting frequency values..." << std::endl;
  w.setFrequencies (frequencies,false);
  w.summary();
  {
    casa::Cube<casa::DComplex> weights = w.weights();
    std::cout << "-- Weighting factors : " << weights.shape() << std::endl;
  }
  
  return nofFailedTests;
}

// -----------------------------------------------------------------------------

int main ()
{
  int nofFailedTests (0);

  // Test for the constructor(s)
  {
    nofFailedTests += test_GeometricalWeight ();
  }

  if (!nofFailedTests) {
    nofFailedTests += test_parameters ();
  } 

  return nofFailedTests;
}
