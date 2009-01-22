/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2008                                                    *
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

#include <Imaging/GeomPhase.h>

// Namespace usage
using std::cout;
using std::endl;
using CR::GeomPhase;

/*!
  \file tGeomPhase.cc

  \ingroup CR_Imaging

  \brief A collection of test routines for the GeomPhase class
 
  \author Lars B&auml;hren
 
  \date 2008/12/17
*/

// -----------------------------------------------------------------------------

/*!
  \brief Test constructors for a new GeomPhase object

  \return nofFailedTests -- The number of failed tests encountered within this
          function.
*/
int test_constructors ()
{
  cout << "\n[tGeomPhase::test_constructors]\n" << endl;

  int nofFailedTests (0);
  
  cout << "[1] Testing default constructor ..." << endl;
  try {
    GeomPhase phase;
    //
    phase.summary(); 
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  cout << "[2] Construction using Vector<double> ..." << endl;
  try {
    Vector<double> frequencies (10);
    GeomPhase phase1 (frequencies,false);
    GeomPhase phase2 (frequencies,true);
    //
    cout << "--> no buffering of delay values:" << endl;
    phase1.summary(); 
    cout << "--> buffering of delay values:" << endl;
    phase2.summary(); 
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  cout << "[3] Construction using Vector<MVFrequency> ..." << endl;
  try {
    Vector<MVFrequency> frequencies (10);
    GeomPhase phase1 (frequencies,false);
    GeomPhase phase2 (frequencies,true);
    //
    cout << "--> no buffering of delay values:" << endl;
    phase1.summary(); 
    cout << "--> buffering of delay values:" << endl;
    phase2.summary(); 
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  cout << "[4] Construction using (GeomDelay,Vector<double>) ..." << endl;
  try {
    casa::Matrix<double> antPositions (2,3);
    casa::Matrix<double> skyPositions (10,3);
    CR::GeomDelay geomDelay (antPositions,skyPositions);
    Vector<double> frequencies (100);
    //
    cout << "-- nof. antenna positions = " << antPositions.nrow() << endl;
    cout << "-- nof. sky positions     = " << skyPositions.nrow() << endl;
    cout << "-- nof. frequencies       = " << frequencies.nelements() << endl;
    //
    GeomPhase phase (geomDelay,
		     frequencies);
    //
    phase.summary(); 
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  cout << "[5] Construction using (GeomDelay,Vector<MVFrequency>) ..." << endl;
  try {
    casa::Matrix<double> antPositions (2,3);
    casa::Matrix<double> skyPositions (10,3);
    CR::GeomDelay geomDelay (antPositions,skyPositions);
    Vector<MVFrequency> frequencies (100);
    //
    cout << "-- nof. antenna positions = " << antPositions.nrow() << endl;
    cout << "-- nof. sky positions     = " << skyPositions.nrow() << endl;
    cout << "-- nof. frequencies       = " << frequencies.nelements() << endl;
    //
    GeomPhase phase (geomDelay,
		     frequencies);
    //
    phase.summary(); 
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  cout << "[6] Construction using full set of parameters ..." << endl;
  try {
    // parameters for GeomDelay
    casa::Matrix<double> antPositions (2,3);
    casa::Matrix<double> skyPositions (10,3);
    bool anglesInDegrees (true);
    bool farField (false);
    bool bufferDelays (false);
    // additional parameters for GeomPhase
    Vector<double> frequencies (100);
    bool bufferPhases (false);
    //
    GeomPhase phase (antPositions,
		     CR::CoordinateType::Cartesian,
		     skyPositions,
		     CR::CoordinateType::Cartesian,
		     anglesInDegrees,
		     farField,
		     bufferDelays,
		     frequencies,
		     bufferPhases);
    //
    phase.summary(); 
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  cout << "[7] Construction using full set of parameters ..." << endl;
  try {
    // parameters for GeomDelay
    casa::Vector<casa::MVPosition> antPositions (2);
    casa::Vector<casa::MVPosition> skyPositions (10);
    bool farField (false);
    bool bufferDelays (false);
    // additional parameters for GeomPhase
    casa::Vector<casa::MVFrequency> frequencies (100);
    bool bufferPhases (false);
    //
    GeomPhase phase (antPositions,
		     skyPositions,
		     farField,
		     bufferDelays,
		     frequencies,
		     bufferPhases);
    //
    phase.summary();
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  cout << "[8] Testing copy constructor..." << endl;
  try {
    casa::Matrix<double> antPositions (2,3);
    casa::Matrix<double> skyPositions (10,3);
    Vector<double> frequencies (100);
    //
    CR::GeomDelay geomDelay (antPositions,skyPositions);
    geomDelay.farField (true);
    //
    GeomPhase phase (geomDelay,
		     frequencies,
		     true);
    GeomPhase phase_copy (phase);
    //
    cout << "--> original object:" << endl;
    phase.summary();
    cout << "--> object constructed as copy:" << endl;
    phase_copy.summary();
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}

// -----------------------------------------------------------------------------

/*!
  \brief Test the static functions provided by the GeomPhase class

  \return nofFailedTests -- The number of failed tests encountered within this
          function.
*/
int test_static_functions ()
{
  cout << "\n[tGeomPhase::test_static_functions]\n" << endl;

  int nofFailedTests (0);

  cout << "[1] GeomPhase(double,double) ..." << endl;
  try {
    double delay (0.1);
    double freq (40e06);
    //
    double phase = GeomPhase::calcPhases(delay,freq);
    //
    cout << "-- Delay     [s ] = " << delay << endl;
    cout << "-- Frequency [Hz] = " << freq  << endl;
    cout << "-- Phase          = " << phase << endl;
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  cout << "[2] GeomPhase(double,Matrix<double>) ..." << endl;
  try {
    Matrix<double> delay (3,3,0.1);
    double freq (40e06);
    //
    Matrix<double> phase = GeomPhase::calcPhases(delay,freq);
    //
    cout << "-- Delay     [s ] = " << delay << endl;
    cout << "-- Frequency [Hz] = " << freq  << endl;
    cout << "-- Phase          = " << phase << endl;
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  cout << "[3] GeomPhase(Vector<double>,Matrix<double>) ..." << endl;
  try {
    Matrix<double> delay (2,2,0.1);
    Vector<double> freq (3);
    //
    freq(0) = 20e6;
    freq(1) = 40e6;
    freq(2) = 80e6;
    //
    Cube<double> phase = GeomPhase::calcPhases(delay,freq);
    //
    cout << "-- Delay     [s ] = " << delay << endl;
    cout << "-- Frequency [Hz] = " << freq  << endl;
    cout << "-- Phase          = " << phase << endl;
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  return nofFailedTests;
}

// -----------------------------------------------------------------------------

/*!
  \brief Test the various methods of the GeomPhase class

  \return nofFailedTests -- The number of failed tests encountered within this
          function.
*/
int test_methods ()
{
  cout << "\n[tGeomPhase::test_methods]\n" << endl;

  int nofFailedTests (0);
  GeomPhase phase;

  cout << "[1] setFrequencies (Vector<double>) ..." << endl;
  try {
    Vector<double> freq (100);
    phase.setFrequencies(freq);
    //
    phase.summary();
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  cout << "[2] setFrequencies (Vector<MVFrequency>) ..." << endl;
  try {
    Vector<MVFrequency> freq (50);
    //
    phase.setFrequencies(freq);
    //
    phase.summary();
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  cout << "[3] setAntPositions (Matrix<double>) ..." << endl;
  try {
    Matrix<double> antPositions (10,3);
    //
    phase.setAntPositions (antPositions);
    //
    phase.summary();
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  cout << "[4] setSkyPositions (Matrix<double>) ..." << endl;
  try {
    Matrix<double> skyPositions (100,3);
    //
    phase.setSkyPositions (skyPositions);
    //
    phase.summary();
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  cout << "[5] Retrieve values of the geometrical phases ..." << endl;
  try {
    Cube<double> val = phase.phases();
    //
    cout << "-- Shape of the array of values = " << val.shape() << endl;
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
  nofFailedTests += test_constructors ();
  // Test the static functions provided by the GeomPhase class
  nofFailedTests += test_static_functions ();
  // Test the various methods
  nofFailedTests += test_methods();

  return nofFailedTests;
}
