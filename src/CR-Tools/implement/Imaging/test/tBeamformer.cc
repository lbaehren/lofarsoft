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

using std::cout;
using std::endl;

using casa::DComplex;
using casa::Matrix;
using casa::Vector;

using CR::GeomDelay;
using CR::GeomPhase;
using CR::GeomWeight;
using CR::Beamformer;
using CR::SkymapQuantity;

/*!
  \file tBeamformer.cc

  \ingroup CR_Imaging

  \brief A collection of test routines for the Beamformer class
 
  \author Lars B&auml;hren
 
  \date 2007/06/13
*/

// -----------------------------------------------------------------------------

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
  
  cout << "[2] Construction using (GeomWeight) ..." << endl;
  try {
    Vector<MVFrequency> frequencies (10);
    CR::GeomPhase phase (frequencies,false);
    GeomWeight weight (phase);
    //
    Beamformer bf (weight);
    bf.summary();
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  cout << "[3] Construction using (GeomPhase,bool) ..." << endl;
  try {
    // parameters for GeomPhase
    Vector<MVFrequency> frequencies (10);
    CR::GeomPhase phase (frequencies,false);
    //
    Beamformer bf (phase);
    bf.summary();
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  cout << "[4] Construction using (GeomDelay,Vector<double>,bool,bool) ..." << endl;
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

    Beamformer bf (geomDelay,
		   frequencies,
		   bufferPhases,
		   bufferWeights);
    bf.summary(); 
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  cout << "[5] Construction using (GeomDelay,Vector<MVFrequency>,bool,bool) ..."
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

    Beamformer bf (geomDelay,
		   frequencies,
		   bufferPhases,
		   bufferWeights);
    bf.summary(); 
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  cout << "[6] Testing fully argumented constructor ..." << endl;
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

    Beamformer bf (antPositions,
		   CR::CoordinateType::Cartesian,
		   skyPositions,
		   CR::CoordinateType::Cartesian,
		   anglesInDegrees,
		   farField,
		   bufferDelays,
		   frequencies,
		   bufferPhases,
		   bufferWeights);
    
    bf.summary();
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  cout << "[7] Testing fully argumented constructor ..." << endl;
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
    Beamformer bf (antPositions,
		   skyPositions,
		   farField,
		   bufferDelays,
		   frequencies,
		   bufferPhases,
		   bufferWeights);
    //
    bf.summary();
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  cout << "[8] Testing copy constructor ..." << endl;
  try {
    Vector<MVFrequency> frequencies (10);
    CR::GeomPhase phase (frequencies,false);
    GeomWeight weight (phase);
    //
    Beamformer bf (weight);
    cout << "--> original object:" << endl;
    bf.summary();
    //
    Beamformer bfCopy (bf);
    cout << "-> object constructed as copy:" << endl;
    bfCopy.summary();
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  return nofFailedTests;
}

// -----------------------------------------------------------------------------

/*!
  \brief Test the methods providing access to internal parameters

  \return nofFailedTests -- The number of failed test encountered within this
          function.
*/
int test_parameters ()
{
  std::cout << "\n[test_parameters]\n" << std::endl;

  int nofFailedTests (0);
  Beamformer bf;

  cout << "[1] Set antenna positions ..." << endl;
  try {
    uint nelem (10);
    Vector<MVPosition> antPositions (nelem);
    //
    cout << "-- old values = " << bf.antPositions().shape() << endl;
    cout << "-- new values = " << antPositions.shape()      << endl;
    //
    bf.setAntPositions (antPositions);
    //
    bf.summary();
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  cout << "[2] Set sky positions ..." << endl;
  try {
    uint nelem (50);
    Vector<MVPosition> skyPositions (nelem);
    //
    cout << "-- old values = " << bf.skyPositions().shape() << endl;
    cout << "-- new values = " << skyPositions.shape()      << endl;
    //
    bf.setSkyPositions (skyPositions);
    //
    bf.summary();
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  cout << "[3] Set frequency values ..." << endl;
  try {
    uint nelem (1024);
    Vector<MVFrequency> freq (nelem);
    //
    cout << "-- old values = " << bf.frequencies().shape() << endl;
    cout << "-- new values = " << freq.shape()             << endl;
    //
    bf.setFrequencies (freq);
    //
    bf.summary();
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  cout << "[4] Set the skymap quantity for which to form the beam ..." << endl;
  try {
    bool ok (true);

    cout << "-- Field in the frequency domain..." << std::endl;
    ok = bf.setSkymapType (SkymapQuantity::FREQ_FIELD);
    cout << "--> Beam domain name   = " << bf.domainName()        << endl;
    cout << "--> Beamforming method = " << bf.skymapType().name() << endl;
    
    std::cout << "-- Power in the frequency domain..." << std::endl;
    ok = bf.setSkymapType (SkymapQuantity::FREQ_POWER);
    cout << "--> Beam domain name   = " << bf.domainName()        << endl;
    cout << "--> Beamforming method = " << bf.skymapType().name() << endl;
    
    std::cout << "-- Field in the time domain..." << std::endl;
    ok = bf.setSkymapType (SkymapQuantity::TIME_FIELD);
    cout << "--> Beam domain name   = " << bf.domainName()        << endl;
    cout << "--> Beamforming method = " << bf.skymapType().name() << endl;
    
    std::cout << "-- Power in the time domain..." << std::endl;
    ok = bf.setSkymapType (SkymapQuantity::TIME_POWER);
    cout << "--> Beam domain name   = " << bf.domainName()        << endl;
    cout << "--> Beamforming method = " << bf.skymapType().name() << endl;
    
    std::cout << "-- cc-beam in the time domain..." << std::endl;
    ok = bf.setSkymapType (SkymapQuantity::TIME_CC);
    cout << "--> Beam domain name   = " << bf.domainName()        << endl;
    cout << "--> Beamforming method = " << bf.skymapType().name() << endl;
    
    std::cout << "-- Power-beam in the time domain..." << std::endl;
    ok = bf.setSkymapType (SkymapQuantity::TIME_P);
    cout << "--> Beam domain name   = " << bf.domainName()        << endl;
    cout << "--> Beamforming method = " << bf.skymapType().name() << endl;
    
    std::cout << "-- Excess-beam in the time domain..." << std::endl;
    ok = bf.setSkymapType (SkymapQuantity::TIME_X);
    cout << "--> Beam domain name   = " << bf.domainName()        << endl;
    cout << "--> Beamforming method = " << bf.skymapType().name() << endl;
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
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
    nofFailedTests += test_parameters();
    nofFailedTests += test_processing ();
  }
  
  return nofFailedTests;
}
