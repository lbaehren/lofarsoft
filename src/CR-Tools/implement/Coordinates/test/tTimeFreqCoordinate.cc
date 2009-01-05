/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2008                                                    *
 *   Lars B"ahren (lbaehren@gmail.com)                                     *
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

#include <Coordinates/TimeFreqCoordinate.h>

// Namespace usage
using std::endl;
using CR::CoordinateType;
using CR::TimeFreq;
using CR::TimeFreqCoordinate;

/*!
  \file tTimeFreqCoordinate.cc

  \ingroup CR_Coordinates

  \brief A collection of test routines for the CR::TimeFreqCoordinate class
 
  \author Lars B&auml;hren
 
  \date 2008/09/06
*/

// -----------------------------------------------------------------------------

/*!
  \brief Test constructors for a new CR::TimeFreqCoordinate object

  \return nofFailedTests -- The number of failed tests within this function.
*/
int test_constructors ()
{
  int nofFailedTests (0);
  
  std::cout << "\n[tTimeFreqCoordinate::test_constructors]\n" << endl;

  std::cout << "[1] Default constructor ..." << endl;
  try {
    TimeFreqCoordinate coord;
    //
    coord.summary(); 
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  std::cout << "[2] Simple argumented constructor ..." << endl;
  try {
    uint blocksPerFrame (2);
    uint nofFrames (10);
    TimeFreqCoordinate coord (blocksPerFrame,
			      nofFrames);
    //
    coord.summary(); 
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  std::cout << "[3] Fully argumented constructor ..." << endl;
  try {
    uint blocksize (1024);
    double sampleFreq (200e06);
    int nyquistZone (2);
    uint blocksPerFrame (2);
    uint nofFrames (10);
    TimeFreqCoordinate coord (blocksize,
			      sampleFreq,
			      nyquistZone,
			      blocksPerFrame,
			      nofFrames);
    //
    coord.summary(); 
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  std::cout << "[4] Construction using TimeFreq object ..." << endl;
  try {
    uint blocksize (1024);
    double sampleFreq (200e06);
    int nyquistZone (2);
    uint blocksPerFrame (2);
    uint nofFrames (10);
    
    TimeFreq tf (blocksize,
		 sampleFreq,
		 nyquistZone);
    TimeFreqCoordinate coord1 (tf);
    TimeFreqCoordinate coord2 (tf,
			       blocksPerFrame,
			       nofFrames);
    //
    std::cout << "--> extra parameters at default values ..." << endl;
    coord1.summary(); 
    std::cout << "--> custom values for extra parameters ..." << endl;
    coord2.summary(); 
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  std::cout << "[5] Copy constructor ..." << endl;
  try {
    uint blocksize (1024);
    double sampleFreq (200e06);
    int nyquistZone (2);
    uint blocksPerFrame (2);
    uint nofFrames (10);
    TimeFreqCoordinate coord1 (blocksize,
			       sampleFreq,
			       nyquistZone,
			       blocksPerFrame,
			       nofFrames);
    //
    coord1.summary();
    /* create second object from the first one */
    TimeFreqCoordinate coord2 (coord1);
    /* Summary of object properties */
    coord2.summary();
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}

// -----------------------------------------------------------------------------

/*!
  \brief Test access to the internal parameters and derived quantities

  \return nofFailedTests -- The number of failed tests within this function.
*/
int test_parameters ()
{
  std::cout << "\n[tTimeFreqCoordinate::test_parameters]\n" << endl;

  int nofFailedTests (0);

  TimeFreqCoordinate coord;

  //________________________________________________________
  // Test of the methods inherited from the base class

  std::cout << "[1] Blocksize ..." << endl;
  try {
    /* current value */
    std::cout << "-- blocksize       = " << coord.blocksize()      << endl;
    std::cout << "-- reference value = " << coord.referenceValue() << endl;
    std::cout << "-- Increment       = " << coord.increment()      << endl;
    std::cout << "-- Shape           = " << coord.shape()          << endl;
    /* Adjust value */
    coord.setBlocksize (1024);
    /* New value */
    std::cout << "--> new values" << endl;
    std::cout << "-- blocksize       = " << coord.blocksize() << endl;
    std::cout << "-- reference value = " << coord.referenceValue() << endl;
    std::cout << "-- Increment       = " << coord.increment()      << endl;
    std::cout << "-- Shape           = " << coord.shape()          << endl;
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  std::cout << "[2] Sample frequency ..." << endl;
  try {
    /* current value */
    std::cout << "-- sampleFrequency = " << coord.sampleFrequency() << endl;
    std::cout << "-- reference value = " << coord.referenceValue() << endl;
    std::cout << "-- Increment       = " << coord.increment()      << endl;
    std::cout << "-- Shape           = " << coord.shape()          << endl;
    /* Adjust value */
    coord.setSampleFrequency (120e06);
    /* New value */
    std::cout << "--> new values" << endl;
    std::cout << "-- sampleFrequency = " << coord.sampleFrequency() << endl;
    std::cout << "-- reference value = " << coord.referenceValue() << endl;
    std::cout << "-- Increment       = " << coord.increment()      << endl;
    std::cout << "-- Shape           = " << coord.shape()          << endl;
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  std::cout << "[3] Nyquist zone ..." << endl;
  try {
    /* current value */
    std::cout << "-- nyquistZone     = " << coord.nyquistZone() << endl;
    std::cout << "-- reference value = " << coord.referenceValue() << endl;
    std::cout << "-- Increment       = " << coord.increment()      << endl;
    std::cout << "-- Shape           = " << coord.shape()          << endl;
    /* Adjust value */
    coord.setNyquistZone (2);
    /* New value */
    std::cout << "--> new values" << endl;
    std::cout << "-- nyquistZone     = " << coord.nyquistZone() << endl;
    std::cout << "-- reference value = " << coord.referenceValue() << endl;
    std::cout << "-- Increment       = " << coord.increment()      << endl;
    std::cout << "-- Shape           = " << coord.shape()          << endl;
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  std::cout << "[4] Reference time ..." << endl;
  try {
    /* current value */
    std::cout << "-- referenceTime   = " << coord.referenceTime() << endl;
    std::cout << "-- reference value = " << coord.referenceValue() << endl;
    std::cout << "-- Increment       = " << coord.increment()      << endl;
    std::cout << "-- Shape           = " << coord.shape()          << endl;
    /* Adjust value */
    coord.setReferenceTime (-0.5);
    /* New value */
    std::cout << "--> new values" << endl;
    std::cout << "-- referenceTime   = " << coord.referenceTime() << endl;
    std::cout << "-- reference value = " << coord.referenceValue() << endl;
    std::cout << "-- Increment       = " << coord.increment()      << endl;
    std::cout << "-- Shape           = " << coord.shape()          << endl;
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  //________________________________________________________
  // Test of the methods added by TimeFreqCoordinate

  std::cout << "[5] Input data blocks per time-frame ..." << endl;
  try {
    /* current value */
    std::cout << "-- blocksPerFrame  = " << coord.blocksPerFrame() << endl;
    std::cout << "-- reference value = " << coord.referenceValue() << endl;
    std::cout << "-- Increment       = " << coord.increment()      << endl;
    std::cout << "-- Shape           = " << coord.shape()          << endl;
    /* Adjust value */
    coord.setBlocksPerFrame (10);
    /* New value */
    std::cout << "--> new values" << endl;
    std::cout << "-- blocksPerFrame  = " << coord.blocksPerFrame() << endl;
    std::cout << "-- reference value = " << coord.referenceValue() << endl;
    std::cout << "-- Increment       = " << coord.increment()      << endl;
    std::cout << "-- Shape           = " << coord.shape()          << endl;
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  std::cout << "[6] NofFrames ..." << endl;
  try {
    /* current value */
    std::cout << "-- nofFrames       = " << coord.nofFrames() << endl;
    std::cout << "-- reference value = " << coord.referenceValue() << endl;
    std::cout << "-- Increment       = " << coord.increment()      << endl;
    std::cout << "-- Shape           = " << coord.shape()          << endl;
    /* Adjust value */
    coord.setNofFrames (100);
    /* New value */
    std::cout << "--> new values" << endl;
    std::cout << "-- nofFrames       = " << coord.nofFrames() << endl;
    std::cout << "-- reference value = " << coord.referenceValue() << endl;
    std::cout << "-- Increment       = " << coord.increment()      << endl;
    std::cout << "-- Shape           = " << coord.shape()          << endl;
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  return nofFailedTests;
}

// -----------------------------------------------------------------------------

/*!
  \brief Test the various methods provded by the class

  Test the methods, which not simply provide access to the private data, but
  which perform operations or computations based on them.

  \return nofFailedTests -- The number of failed tests within this function.
*/
int test_methods ()
{
  std::cout << "\n[tTimeFreqCoordinate::test_methods]\n" << endl;

  int nofFailedTests (0);
  
  uint blocksize (1024);
  double sampleFreq (200e06);
  int nyquistZone (2);
  uint blocksPerFrame (2);
  uint nofFrames (10);

  TimeFreqCoordinate coord (blocksize,
			    sampleFreq,
			    nyquistZone,
			    blocksPerFrame,
			    nofFrames);
  
  std::cout << "[1] Coordinate objects for the axes .." << endl;
  try {
    std::cout << "--> time axis" << endl;
    casa::LinearCoordinate time = coord.timeAxisCoordinate();
    CoordinateType::summary (std::cout,time);
    //
    std::cout << "--> frequency axis" << endl;
    casa::SpectralCoordinate freq = coord.frequencyAxisCoordinate();
    CoordinateType::summary (std::cout,freq);
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  std::cout << "[2] Add coordinates objects to a coordinate system ..." << endl;
  try {
    casa::CoordinateSystem csys;
    // 
    coord.toCoordinateSystem (csys);
    //
    std::cout << "-- nof. coordinates = " << csys.nCoordinates()   << endl;
    std::cout << "-- Reference pixel  = " << csys.referencePixel() << endl;
    std::cout << "-- Increment        = " << csys.increment()      << endl;
    std::cout << "-- Reference value  = " << csys.referenceValue() << endl;
    // 
    coord.toCoordinateSystem (csys);
    //
    std::cout << "-- nof. coordinates = " << csys.nCoordinates()   << endl;
    std::cout << "-- Reference pixel  = " << csys.referencePixel() << endl;
    std::cout << "-- Increment        = " << csys.increment()      << endl;
    std::cout << "-- Reference value  = " << csys.referenceValue() << endl;
    // 
    coord.toCoordinateSystem (csys,false);
    //
    std::cout << "-- nof. coordinates = " << csys.nCoordinates()   << endl;
    std::cout << "-- Reference pixel  = " << csys.referencePixel() << endl;
    std::cout << "-- Increment        = " << csys.increment()      << endl;
    std::cout << "-- Reference value  = " << csys.referenceValue() << endl;
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
  // Test access to the internal parameters
  nofFailedTests += test_parameters ();
  // Test provided methods
  nofFailedTests += test_methods ();
  
  return nofFailedTests;
}
