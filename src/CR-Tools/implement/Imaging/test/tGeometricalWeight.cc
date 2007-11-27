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

#include "create_data.h"
#include <Imaging/GeometricalWeight.h>

using CR::GeometricalDelay;
using CR::GeometricalPhase;
using CR::GeometricalWeight;

using std::cout;
using std::endl;

/*!
  \file tGeometricalWeight.cc

  \ingroup Imaging

  \brief A collection of test routines for GeometricalWeight
 
  \author Lars B&auml;hren
 
  \date 2007/01/15
*/

// -----------------------------------------------------------------------------

/*!
  \brief Export the positions and weights to an ASCII table

  \param weight  -- GeometricalWeight object for which the data are exported.
  \param filename -- Name of the file to which the data will be written
*/
void export_weights (GeometricalWeight &weight,
		     std::string const &filename="tGeometricalWeight.data")
{
  int freq (0);
  int numAntenna(0);
  int numSky (0);
  int coord(0);
  casa::Matrix<double> antPositions  = weight.antennaPositions();
  casa::Matrix<double> skyPositions  = weight.skyPositions();
  casa::Vector<double> frequencies   = weight.frequencies();
  casa::Cube<casa::DComplex> weights = weight.weights();

  casa::IPosition shape(weights.shape());
  
  std::ofstream outfile (filename.c_str(),std::ios::out);
  
  for (numAntenna=0; numAntenna<shape(1); numAntenna++) {
    for (numSky=0; numSky<shape(2); numSky++) {
      for (freq=0; freq<shape(0); freq++) {
	// export antenna position
	outfile << antPositions(numAntenna,0) << "  "
		<< antPositions(numAntenna,1) << "  "
		<< antPositions(numAntenna,2) << "  ";
	// export sky position
	outfile << skyPositions(numSky,0) << "  "
		<< skyPositions(numSky,1) << "  "
		<< skyPositions(numSky,2) << "  ";
	// export frequency value & geometrical phase
	outfile << frequencies(freq) << "  " 
		<< weights(freq,numAntenna,numSky)
		<< std::endl;
      }
      // insert newline after set of frequency channels
      outfile << std::endl;
    }
    // insert newline after set of sky positions
    outfile << std::endl;
  }

  outfile.close();
}

// ------------------------------------------------------------------------------

/*!
  \brief Test constructors for a new GeometricalWeight object

  \return nofFailedTests -- The number of failed tests.
*/
int test_GeometricalWeight ()
{
  cout << "\n[test_GeometricalWeight]\n" << endl;

  int nofFailedTests (0);
  casa::Matrix<double> antennaPositions = get_antennaPositions();
  casa::Matrix<double> skyPositions     = get_skyPositions();
  casa::Vector<double> frequencies      = get_frequencies();
  
  cout << "[1] Testing default constructor ..." << endl;
  try {
    GeometricalWeight weight;
    // get summary of object properties
    weight.summary();
    // retrieve geometrical weights
    casa::Cube<casa::DComplex> weights = weight.weights();
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  cout << "[2] Testing argumented constructor ..." << endl;
  try {
    // Create new object
    GeometricalWeight weight (antennaPositions,
			      skyPositions,
			      frequencies);
    // get summary of object properties
    weight.summary();
    // retrieve geometrical weights
    casa::Cube<casa::DComplex> weights = weight.weights();
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  cout << "[3] Testing constructor using GeometricalDelay object ..." << endl;
  try {
    cout << "-- creating GeometricalDelay object ..." << endl;
    GeometricalDelay delay (antennaPositions,
			    skyPositions);
    cout << "-- creating GeometricalWeights object ..." << endl;
    GeometricalWeight weight (delay,
			      frequencies);
    // get summary of object properties
    weight.summary();
    // retrieve geometrical weights
    casa::Cube<casa::DComplex> weights = weight.weights();
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  cout << "[4] Testing constructor using GeometricalPhase object ..." << endl;
  try {
    cout << "-- creating GeometricalPhase object ..." << endl;
    GeometricalPhase phase (antennaPositions,
			    skyPositions,
			    frequencies);
    cout << "-- creating GeometricalWeights object ..." << endl;
    GeometricalWeight weight (phase);
    // get summary of object properties
    weight.summary();
    // retrieve geometrical weights
    casa::Cube<casa::DComplex> weights = weight.weights();
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  cout << "[5] Testing copy constructor ..." << endl;
  try {
    cout << "-- creating original GeometricalWeight object ..." << endl;
    GeometricalWeight weights (antennaPositions,
			       skyPositions,
			       frequencies);
    weights.summary();
    cout << "-- creating GeometricalWeight object as copy ..." << endl;
    GeometricalWeight weightsCopy (weights);
    weightsCopy.summary();
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}

// ------------------------------------------------------------------------------

/*!
  \brief Test setting and retrieving of the various parameters

  \return nofFailedTests -- The number of failed tests.
*/
int test_parameters ()
{
  cout << "\n[test_parameters]\n" << endl;

  int nofFailedTests (0);
  bool status (true);

  /* Create GeometricalWeight object to work with */

  GeometricalWeight w;
  w.summary();

  cout << "[1] Setting antenna positions..." << endl;
  try {
    status = w.setAntennaPositions (get_antennaPositions(),
				    false);
    w.summary();
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  cout << "[2] Setting sky positions..." << endl;
  try {
    status = w.setSkyPositions (get_skyPositions(),
				false);
    w.summary();
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  cout << "[3] Setting frequency values..." << endl;
  try {
    status = w.setFrequencies (get_frequencies(),
			       false);
    w.summary();
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  cout << "[4] Switch on buffering of weights ..." << endl;
  try {
    w.bufferWeights(true);
    w.summary();
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}

// ------------------------------------------------------------------------------

/*!
  \brief Test some more realistic usage as taking place e.g. with the pipeline

  \return nofFailedTests -- The number of failed tests within the function.
*/
int test_processing ()
{
  int nofFailedTests (0);

  return nofFailedTests;
}

// ------------------------------------------------------------------------------

int main ()
{
  int nofFailedTests (0);
  
  // Test for the constructor(s)
  nofFailedTests += test_GeometricalWeight ();

  /* if construction work we can continue with the rest */
  
  if (!nofFailedTests) {
    nofFailedTests += test_parameters ();
//     nofFailedTests += test_processing ();
  } 

  return nofFailedTests;
}
