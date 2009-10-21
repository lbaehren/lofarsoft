/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
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
    GeomWeight w;
    //
    w.summary(); 
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  cout << "[2] Construction using (GeomPhase,bool) ..." << endl;
  try {
    Vector<MVFrequency> frequencies (10);
    CR::GeomPhase phase (frequencies,false);
    //
    GeomWeight weight (phase,false);
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

    // set geometrical delays
    indgen(delays);
    // set frequency values
    frequencies(0) = 20e06;
    frequencies(1) = 40e06;
    frequencies(2) = 80e06;
    frequencies(3) = 160e06;
    // compute the geometrical weights
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

    indgen(phases);

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

/*!
  \brief Test the public methods of the GeomWeight class

  \return nofFailedTests -- The number of failed tests encountered within this
          function. 
*/
int test_methods ()
{
  cout << "\n[tGeomWeight::test_methods]\n" << endl;

  int nofFailedTests (0);
  Vector<casa::MVPosition> antPositions (3);
  Matrix<double> pos (2,3);
  Vector<double> freq (10);
  GeomWeight w;

  cout << "[1] Storing parameters to GeomWeight object ..." << endl;
  try {
    pos(0,0) = 1;
    pos(0,1) = 90;
    pos(0,2) = 0;
    pos(1,0) = 1;
    pos(1,1) = 0;
    pos(1,2) = 90;
    std::cout << "-- setting sky positions ..." << std::endl;
    w.setSkyPositions (pos,
		       CR::CoordinateType::Spherical,
		       true);
    //
    antPositions(0) = casa::MVPosition (100,0,0);
    antPositions(1) = casa::MVPosition (0,100,0);
    antPositions(2) = casa::MVPosition (100,100,0);
    std::cout << "-- setting antenna positions ..." << std::endl;
    w.setAntPositions (antPositions);
    //
    for (uint n(0); n<freq.nelements(); n++) { freq(n) = n; } 
    std::cout << "-- setting frequency values ..." << std::endl;
    w.setFrequencies(freq);
    //
    std::cout << "-- shape(weights) = " << w.shape() << std::endl;
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

  std::cout << "[2] Retrieve geometrical delays ..." << std::endl;
  try {
    Matrix<double> delays;
    //
    delays = w.delays();
    std::cout << "-- delays by return = " << delays << std::endl;
    //
    delays = 1;
    w.delays(delays);
    std::cout << "-- delays by ref    = " << delays << std::endl;
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

  std::cout << "[3] Retrieve geometrical phases ..." << std::endl;
  try {
    Cube<double> phases;
    //
    phases = w.phases();
    std::cout << "-- phases by return = " << phases << std::endl;
    //
    phases = 1;
    w.phases(phases);
    std::cout << "-- phases by ref    = " << phases << std::endl;
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

  std::cout << "[4] Retrieve geometrical weights ..." << std::endl;
  try {
    Cube<casa::DComplex> weights;
    //
    weights = w.weights();
    std::cout << "-- weights by return = " << weights << std::endl;
    //
    weights = 1;
    w.weights(weights);
    std::cout << "-- weights by ref    = " << weights << std::endl;
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
  nofFailedTests += test_constructors();
  // Test the static functions provided by the class
  nofFailedTests += test_static_functions();
  // Test public methods of the clss
  nofFailedTests += test_methods();

  return nofFailedTests;
}
