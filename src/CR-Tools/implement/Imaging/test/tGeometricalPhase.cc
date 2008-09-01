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

/* $Id$*/

#include <iostream>

#include "create_data.h"
#include <Imaging/GeometricalPhase.h>

using std::cout;

using CR::GeometricalDelay;
using CR::GeometricalPhase;

/*!
  \file tGeometricalPhase.cc

  \ingroup CR_Imaging

  \brief A collection of test routines for GeometricalPhase
 
  \author Lars B&auml;hren
 
  \date 2007/01/15
*/

// -----------------------------------------------------------------------------

/*!
  \brief Export the positions and phases to an ASCII table

  \param antPositions -- [antenna,3] Three-dimensional positions of the antennas.
  \param skyPositions -- [position,3] Three-dimensional positions towards which
         the phases are computed.
  \param frequencies  -- Frequency values, [Hz]
  \param phases       -- [antenna,sky] Array storing the values of the
         geometrical phases.
  \param filename     -- Name of the file to which the data will be written
*/
void export_phases (casa::Matrix<double> const &antPositions,
		    casa::Matrix<double> const &skyPositions,
		    casa::Vector<double> const &frequencies,
		    casa::Cube<double> const &phases,
		    std::string const &filename="tGeometricalPhase.data")
{
  int freq (0);
  int numAntenna(0);
  int numSky (0);
  casa::IPosition shape(phases.shape());

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
		<< phases(freq,numAntenna,numSky)
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

// -----------------------------------------------------------------------------

/*!
  \brief Test constructors for a new GeometricalPhase object

  \return nofFailedTests -- The number of failed tests.
*/
int test_GeometricalPhase ()
{
  std::cout << "\n[test_GeometricalPhase]\n" << std::endl;

  int nofFailedTests (0);
  
  std::cout << "[1] Testing default constructor ..." << std::endl;
  try {
    GeometricalPhase phase;
    // summary of object's internal settings
    phase.summary();
    // retrieve phases
    casa::Matrix<double> delays = phase.delays();
    casa::Cube<double> phases   = phase.phases();
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  std::cout << "[2] Testing simple argumented constructor ..." << std::endl;
  try {
    casa::Vector<double> frequencies = get_frequencies (20);

    GeometricalPhase phase (frequencies);
    // summary of object's internal settings
    phase.summary();
    // retrieve delays and phases
    casa::Matrix<double> delays = phase.delays();
    casa::Cube<double> phases   = phase.phases();
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

  std::cout << "[3] Testing argumented constructor ..." << std::endl;
  try {
    casa::Matrix<double> antennaPositions = get_antennaPositions();
    casa::Matrix<double> skyPositions = get_skyPositions();
    casa::Vector<double> frequencies = get_frequencies (20);

    std::cout << "-- antenna positions array = " << antennaPositions.shape()
	      << std::endl;
    std::cout << "-- sky positions array     = " << skyPositions.shape() << std::endl;
    std::cout << "-- frequency array         = " << frequencies.shape()  << std::endl;

    GeometricalPhase phase (antennaPositions,
			    skyPositions,
			    frequencies);
    // summary of object's internal settings
    phase.summary();
    // retrieve delays and phases
    casa::Matrix<double> delays = phase.delays();
    casa::Cube<double> phases   = phase.phases();
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  std::cout << "[4] Testing argumented constructor ..." << std::endl;
  try {
    uint nofAntennas (4);
    uint nofSkyPositions (5);
    bool antennaIndexFirst (true);
    CR::CoordinateType coordType (CR::CoordinateType::Spherical);
    std::cout << "-- getting antenna positions ..." << std::endl;
    casa::Matrix<double> antennaPositions = get_antennaPositions(nofAntennas,
								 antennaIndexFirst,
								 coordType.type());
    std::cout << "-- getting sky positions ..." << std::endl;
    casa::Matrix<double> skyPositions = get_skyPositions(nofSkyPositions,
							 coordType.type());
    std::cout << "-- getting frequency values ..." << std::endl;
    casa::Vector<double> frequencies = get_frequencies (20);
    std::cout << "-- creating GeometricalPhase object ..." << std::endl;
    GeometricalPhase phase (antennaPositions,
			    coordType.type(),
			    skyPositions,
			    coordType.type(),
			    frequencies);
    // summary of object's internal settings
    phase.summary();
    // retrieve delays and phases
    casa::Matrix<double> delays = phase.delays();
    casa::Cube<double> phases = phase.phases();
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  std::cout << "[5] Testing constructor using GeometricalDelay object ..." << std::endl;
  try {
    uint nofAntennas (4);
    uint nofSkyPositions (5);
    bool antennaIndexFirst (true);
    CR::CoordinateType antCoordType (CR::CoordinateType::Spherical);
    CR::CoordinateType skyCoordType (CR::CoordinateType::Spherical);
    std::cout << "-- getting antenna positions ..." << std::endl;
    casa::Matrix<double> antennaPositions = get_antennaPositions(nofAntennas,
								 antennaIndexFirst,
								 antCoordType.type());
    std::cout << "-- getting sky positions ..." << std::endl;
    casa::Matrix<double> skyPositions = get_skyPositions(nofSkyPositions,
							 skyCoordType.type());
    std::cout << "-- getting frequency values ..." << std::endl;
    casa::Vector<double> frequencies = get_frequencies (20);
    std::cout << "-- creating GeometricalPhase object ..." << std::endl;
    GeometricalDelay delay (antennaPositions,
			    antCoordType.type(),
			    skyPositions,
			    skyCoordType.type());
    delay.summary();
    std::cout << "-- creating GeometricalPhase object ..." << std::endl;
    GeometricalPhase phase (delay,
			    frequencies);
    // summary of object's internal settings
    phase.summary();
    // retrieve delays and phases
    casa::Matrix<double> delays = phase.delays();
    casa::Cube<double> phases   = phase.phases();
    
    /* Create a second GeometricalPhase object to compare its internal settings
       with the one of the created before. */
    GeometricalPhase phase2 (antennaPositions,
			     antCoordType.type(),
			     skyPositions,
			     skyCoordType.type(),
			     frequencies);
    
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  std::cout << "[6] Testing copy constructor ..." << std::endl;
  try {
    casa::Matrix<double> antennaPositions = get_antennaPositions();
    casa::Matrix<double> skyPositions     = get_skyPositions();
    casa::Vector<double> frequencies      = get_frequencies (20);

    GeometricalPhase phase (antennaPositions,
			    skyPositions,
			    frequencies);
    // summary of object's internal settings
    phase.summary();
    // create new object from already existing one
    GeometricalPhase phaseCopy (phase);
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}

// -----------------------------------------------------------------------------

/*!
  \brief Test the computation of the geometrical phases
*/
int test_phaseComputation ()
{
  std::cout << "\n[test_phaseComputation]\n" << std::endl;

  int nofFailedTests (0);
  
  try {
    double incr (100);
    uint nofAntennas (2);
    uint nofSkyPositions (10);
    
    /* Antenna positions */
    CR::CoordinateType antCoordType (CR::CoordinateType::Cartesian);
    casa::Matrix<double> antennaPositions (nofAntennas,3,0.0);
    antennaPositions(0,0) = 100.0;
    antennaPositions(1,0) = -100.0;
    
    /* Sky positions */
    CR::CoordinateType skyCoordType (CR::CoordinateType::Spherical);
    casa::Matrix<double> skyPositions (nofSkyPositions,3);
    for (uint n(0); n<nofSkyPositions; n++) {
      skyPositions(n,0) = n*incr;
      skyPositions(n,1) = skyPositions(n,2) = CR::deg2rad(45);
    }

    /* Frequency values */
    casa::Vector<double> frequencies = get_frequencies (64);

    std::cout << "-- creating GeometricalPhase object ..." << std::endl;
    GeometricalPhase phase (antennaPositions,
			    antCoordType.type(),
			    skyPositions,
			    skyCoordType.type(),
			    frequencies);
    // summary of object's internal settings
    phase.summary();

    std::cout << "-- exporting calculated values to file for closer inspection ..."
	      << std::endl;
    export_phases (phase.antennaPositions(),
		   phase.skyPositions(),
		   phase.frequencies(),
		   phase.phases());
    std::cout << "-- export completed." << std::endl;

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
  nofFailedTests += test_GeometricalPhase ();

  /* Test the actual computation of the phase values */
  nofFailedTests += test_phaseComputation ();

  return nofFailedTests;
}
