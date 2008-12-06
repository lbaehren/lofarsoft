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

#include <Coordinates/TimeFreqSkymap.h>

// Namespace usage
using CR::CoordinateType;
using CR::TimeFreqSkymap;

/*!
  \file tTimeFreqSkymap.cc

  \ingroup CR_Coordinates

  \brief A collection of test routines for the CR::TimeFreqSkymap class
 
  \author Lars B&auml;hren
 
  \date 2008/09/06
*/

// -----------------------------------------------------------------------------

/*!
  \brief Test constructors for a new CR::TimeFreqSkymap object

  \return nofFailedTests -- The number of failed tests within this function.
*/
int test_constructors ()
{
  int nofFailedTests (0);
  
  std::cout << "\n[tTimeFreqSkymap::test_constructors]\n" << std::endl;

  std::cout << "[1] Default constructor ..." << std::endl;
  try {
    TimeFreqSkymap coord;
    //
    coord.summary(); 
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  std::cout << "[2] Simple argumented constructor ..." << std::endl;
  try {
    uint blocksPerFrame (2);
    uint nofFrames (10);
    TimeFreqSkymap coord (blocksPerFrame,
			  nofFrames);
    //
    coord.summary(); 
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  std::cout << "[3] Fully argumented constructor ..." << std::endl;
  try {
    uint blocksize (1024);
    double sampleFreq (200e06);
    int nyquistZone (2);
    uint blocksPerFrame (2);
    uint nofFrames (10);
    TimeFreqSkymap coord (blocksize,
			  sampleFreq,
			  nyquistZone,
			  blocksPerFrame,
			  nofFrames);
    //
    coord.summary(); 
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  std::cout << "[4] Copy constructor ..." << std::endl;
  try {
    uint blocksize (1024);
    double sampleFreq (200e06);
    int nyquistZone (2);
    uint blocksPerFrame (2);
    uint nofFrames (10);
    TimeFreqSkymap coord1 (blocksize,
			   sampleFreq,
			   nyquistZone,
			   blocksPerFrame,
			   nofFrames);
    //
    coord1.summary();
    /* create second object from the first one */
    TimeFreqSkymap coord2 (coord1);
    /* Summary of object properties */
    coord2.summary();
  } catch (std::string message) {
    std::cerr << message << std::endl;
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
  std::cout << "\n[tTimeFreqSkymap::test_parameters]\n" << std::endl;

  int nofFailedTests (0);

  TimeFreqSkymap timeFreq;

  //________________________________________________________
  // Test of the methods inherited from the base class

  std::cout << "[1] Blocksize ..." << std::endl;
  try {
    /* current value */
    std::cout << "-- blocksize = " << timeFreq.blocksize() << std::endl;
    /* Adjust value */
    timeFreq.setBlocksize (1024);
    /* New value */
    std::cout << "-- blocksize = " << timeFreq.blocksize() << std::endl;
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

  std::cout << "[2] Sample frequency ..." << std::endl;
  try {
    /* current value */
    std::cout << "-- sampleFrequency = " << timeFreq.sampleFrequency() << std::endl;
    /* Adjust value */
    timeFreq.setSampleFrequency (120e06);
    /* New value */
    std::cout << "-- sampleFrequency = " << timeFreq.sampleFrequency() << std::endl;
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

  std::cout << "[3] Nyquist zone ..." << std::endl;
  try {
    /* current value */
    std::cout << "-- nyquistZone = " << timeFreq.nyquistZone() << std::endl;
    /* Adjust value */
    timeFreq.setNyquistZone (2);
    /* New value */
    std::cout << "-- nyquistZone = " << timeFreq.nyquistZone() << std::endl;
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

  std::cout << "[4] Reference time ..." << std::endl;
  try {
    /* current value */
    std::cout << "-- referenceTime = " << timeFreq.referenceTime() << std::endl;
    /* Adjust value */
    timeFreq.setReferenceTime (-0.5);
    /* New value */
    std::cout << "-- referenceTime = " << timeFreq.referenceTime() << std::endl;
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

  //________________________________________________________
  // Test of the methods added by TimeFreqSkymap

  std::cout << "[5] Input data blocks per time-frame ..." << std::endl;
  try {
    /* current value */
    std::cout << "-- blocksPerFrame = " << timeFreq.blocksPerFrame() << std::endl;
    /* Adjust value */
    timeFreq.setBlocksPerFrame (10);
    /* New value */
    std::cout << "-- blocksPerFrame = " << timeFreq.blocksPerFrame() << std::endl;
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

  std::cout << "[6] NofFrames ..." << std::endl;
  try {
    /* current value */
    std::cout << "-- nofFrames = " << timeFreq.nofFrames() << std::endl;
    /* Adjust value */
    timeFreq.setNofFrames (100);
    /* New value */
    std::cout << "-- nofFrames = " << timeFreq.nofFrames() << std::endl;
  } catch (std::string message) {
    std::cerr << message << std::endl;
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
  std::cout << "\n[tTimeFreqSkymap::test_methods]\n" << std::endl;

  int nofFailedTests (0);
  
  uint blocksize (1024);
  double sampleFreq (200e06);
  int nyquistZone (2);
  uint blocksPerFrame (2);
  uint nofFrames (10);
  TimeFreqSkymap tf (blocksize,
		     sampleFreq,
		     nyquistZone,
		     blocksPerFrame,
		     nofFrames);
  
  std::cout << "[1] Coordinate axis reference value ..." << std::endl;
  try {
#ifdef HAVE_CASA
    std::cout << "-- referenceValue = " << tf.referenceValue() << std::endl;
#else 
    std::cout << "-- referenceValue = [" << tf.referenceValue()[0] << ","
	      << tf.referenceValue()[1] << std::endl;
#endif
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  std::cout << "[2] Coordinate axis increment ..." << std::endl;
  try {
#ifdef HAVE_CASA
    std::cout << "-- increment = " << tf.increment() << std::endl;
#else 
    std::cout << "-- increment = [" << tf.increment()[0] << ","
	      << tf.increment()[1] << std::endl;
#endif
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  std::cout << "[3] Coordinate axis length ..." << std::endl;
  try {
#ifdef HAVE_CASA
    std::cout << "-- shape = " << tf.shape() << std::endl;
#else 
    std::cout << "-- shape = [" << tf.shape()[0] << ","
	      << tf.shape()[1] << std::endl;
#endif
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

#ifdef HAVE_CASA
  std::cout << "[4] Coordinate objects for the axes .." << std::endl;
  try {
    std::cout << "--> time axis" << std::endl;
    casa::LinearCoordinate time = tf.timeAxisCoordinate();
    CoordinateType::summary (std::cout,time);
    //
    std::cout << "--> frequency axis" << std::endl;
    casa::SpectralCoordinate freq = tf.frequencyAxisCoordinate();
    CoordinateType::summary (std::cout,freq);
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
#endif
  
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
