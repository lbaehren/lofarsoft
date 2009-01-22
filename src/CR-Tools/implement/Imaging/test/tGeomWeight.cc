/*-------------------------------------------------------------------------*
 | $Id:: tNewClass.cc 1964 2008-09-06 17:52:38Z baehren                  $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2009                                                    *
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

#include <Imaging/GeomWeight.h>

// Namespace usage
using std::cout;
using std::endl;
using CR::GeomDelay;
using CR::GeomPhase;
using CR::GeomWeight;

/*!
  \file tGeomWeight.cc

  \ingroup CR_Imaging

  \brief A collection of test routines for the GeomWeight class
 
  \author Lars B&auml;hren
 
  \date 2009/01/15
*/

// -----------------------------------------------------------------------------

/*!
  \brief Test constructors for a new GeomWeight object

  \return nofFailedTests -- The number of failed tests encountered within this
          function.
*/
int test_constructors ()
{
  cout << "\n[tGeomWeight::test_constructors]\n" << endl;

  int nofFailedTests (0);
  
  cout << "[1] Testing default constructor ..." << endl;
  try {
    GeomWeight weight;
    //
    weight.summary(); 
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  cout << "[2] Construction using (GeomPhase,bool) ..." << endl;
  try {
    Vector<MVFrequency> frequencies (10);
    CR::GeomPhase phase (frequencies,false);
    //
    GeomWeight weight (phase);
    //
    weight.summary(); 
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  cout << "[3] Construction using (GeomDelay,Vector<double>,bool,bool) ..." << endl;
  try {
    // parameters for GeomDelay
    casa::Matrix<double> antPositions (2,3);
    casa::Matrix<double> skyPositions (10,3);
    bool anglesInDegrees (true);
    bool farField (false);
    bool bufferDelays (false);
    GeomDelay geomDelay (antPositions,
			 skyPositions,
			 anglesInDegrees,
			 farField,
			 bufferDelays);
    // parameters for GeomPhase
    Vector<double> frequencies (10);
    bool bufferPhases (false);
    // parameters for GeomWeights
    bool bufferWeights (false);

    GeomWeight weight (geomDelay,
		       frequencies,
		       bufferPhases,
		       bufferWeights);
    weight.summary(); 
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  cout << "[4] Construction using (GeomDelay,Vector<MVFrequency>,bool,bool) ..."
       << endl;
  try {
    // parameters for GeomDelay
    casa::Matrix<double> antPositions (2,3);
    casa::Matrix<double> skyPositions (10,3);
    bool anglesInDegrees (true);
    bool farField (false);
    bool bufferDelays (false);
    GeomDelay geomDelay (antPositions,
			 skyPositions,
			 anglesInDegrees,
			 farField,
			 bufferDelays);
    // parameters for GeomPhase
    Vector<MVFrequency> frequencies (10);
    bool bufferPhases (false);
    // parameters for GeomWeights
    bool bufferWeights (false);

    GeomWeight weight (geomDelay,
		       frequencies,
		       bufferPhases,
		       bufferWeights);
    weight.summary(); 
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  cout << "[5] Testing fully argumented constructor ..." << endl;
  try {
    // parameters for GeomDelay
    casa::Matrix<double> antPositions (2,3);
    casa::Matrix<double> skyPositions (10,3);
    bool anglesInDegrees (true);
    bool farField (false);
    bool bufferDelays (false);
    // parameters for GeomPhase
    Vector<double> frequencies (100);
    bool bufferPhases (false);
    // parameters for GeomWeights
    bool bufferWeights (false);

    GeomWeight w (antPositions,
		  CR::CoordinateType::Cartesian,
		  skyPositions,
		  CR::CoordinateType::Cartesian,
		  anglesInDegrees,
		  farField,
		  bufferDelays,
		  frequencies,
		  bufferPhases,
		  bufferWeights);

    w.summary();
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  cout << "[6] Testing fully argumented constructor ..." << endl;
  try {
    // parameters for GeomDelay
    casa::Vector<casa::MVPosition> antPositions (2);
    casa::Vector<casa::MVPosition> skyPositions (10);
    bool farField (false);
    bool bufferDelays (false);
    // parameters for GeomPhase
    casa::Vector<casa::MVFrequency> frequencies (100);
    bool bufferPhases (false);
    // parameters for GeomWeights
    bool bufferWeights (false);
    //
    GeomWeight w (antPositions,
		  skyPositions,
		  farField,
		  bufferDelays,
		  frequencies,
		  bufferPhases,
		  bufferWeights);
    //
    w.summary();
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  cout << "[7] Testing copy constructor ..." << endl;
  try {
    Vector<MVFrequency> frequencies (10);
    CR::GeomPhase phase (frequencies,false);

    GeomWeight weight1 (phase);
    weight1.bufferPhases(true);

    GeomWeight weight2 (weight1);

    cout << "--> original object:" << endl;
    weight1.summary();
    cout << "--> object created through copy constructor:" << endl;
    weight2.summary();
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}

// -----------------------------------------------------------------------------

/*!
  \brief Test the static functions provided by the class

  \return nofFailedTests -- The number of failed tests encountered within this
          function.
*/
int test_static_functions ()
{
  cout << "\n[tGeomWeight::test_static_functions]\n" << endl;

  int nofFailedTests (0);
  
  cout << "[1] calcWeights (Matrix<double>,Vector<double>)" << endl;
  try {
    Matrix<double> delays (3,3,1.0);
    Vector<double> frequencies (4);
    
    frequencies(0) = 20e06;
    frequencies(1) = 40e06;
    frequencies(2) = 80e06;
    frequencies(3) = 160e06;
    
    Cube<DComplex> weights = GeomWeight::calcWeights (delays,frequencies);
    
    cout << "-- delays      = " << delays      << endl;
    cout << "-- frequencies = " << frequencies << endl;
    cout << "-- weights     = " << weights     << endl;
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  cout << "[2] calcWeights (Cube<double>)" << endl;
  try {
    Cube<double> phases (3,3,3,1.0);

    Cube<DComplex> weights = GeomWeight::calcWeights(phases);

    cout << "-- phases  = " << phases  << endl;
    cout << "-- weights = " << weights << endl;
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  return nofFailedTests;
}

// -----------------------------------------------------------------------------

int main ()
{
  int nofFailedTests (0);

  // Test for the constructor(s)
  nofFailedTests += test_constructors();
  // Test the static functions provided by the class
  nofFailedTests += test_static_functions();

  return nofFailedTests;
}
