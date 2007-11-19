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
  casa::Vector<double> frequencies;
  
  cout << "[1] Testing default constructor ..." << endl;
  try {
    GeometricalWeight weight;
    weight.summary();
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
    weight.summary();
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
    weight.summary();
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
    weight.summary();
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  cout << "[5] Testing copy constructor ..." << endl;
  try {
    cout << "-- creating original GeometricalWeights object ..." << endl;
    GeometricalPhase weights (antennaPositions,
			      skyPositions,
			      frequencies);
    weights.summary();
    cout << "-- creating GeometricalWeights object as copy ..." << endl;
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
  casa::Matrix<double> antennaPositions = get_antennaPositions();
  casa::Matrix<double> skyPositions     = get_skyPositions();
  casa::Vector<double> frequencies      = get_frequencies();
  
  cout << "[1] Creating new object via default constructor..." << endl;
  GeometricalWeight w;
  w.summary();
  {
    casa::Cube<casa::DComplex> weights = w.weights();
    cout << "-- Weighting factors : " << weights.shape() << endl;
  }

  cout << "[2] Setting antenna positions..." << endl;
  w.setAntennaPositions (antennaPositions,false);
  w.summary();
  {
    casa::Cube<casa::DComplex> weights = w.weights();
    cout << "-- Weighting factors : " << weights.shape() << endl;
  }
  
  cout << "[3] Setting sky positions..." << endl;
  w.setSkyPositions (skyPositions,false);
  w.summary();
  {
    casa::Cube<casa::DComplex> weights = w.weights();
    cout << "-- Weighting factors : " << weights.shape() << endl;
  }

  cout << "[4] Setting frequency values..." << endl;
  w.setFrequencies (frequencies,false);
  w.summary();
  {
    casa::Cube<casa::DComplex> weights = w.weights();
    cout << "-- Weighting factors : " << weights.shape() << endl;
  }
  
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
  } 

  return nofFailedTests;
}
