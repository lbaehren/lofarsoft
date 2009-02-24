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
using std::cout;
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
  
  cout << "\n[tTimeFreqCoordinate::test_constructors]\n" << endl;

  cout << "[1] Default constructor ..." << endl;
  try {
    TimeFreqCoordinate coord1;
    TimeFreqCoordinate coord2(true);
    //
    coord1.summary(); 
    coord2.summary(); 
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  cout << "[2] Simple argumented constructor ..." << endl;
  try {
    uint blocksize (1024);
    uint blocksPerFrame (2);
    uint nofFrames (10);
    //
    TimeFreqCoordinate coord1 (blocksize,
			      blocksPerFrame,
			      nofFrames,
			      false);
    TimeFreqCoordinate coord2 (blocksize,
			      blocksPerFrame,
			      nofFrames,
			      true);
    coord1.summary(); 
    coord2.summary();
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  cout << "[3] Fully argumented constructor ..." << endl;
  try {
    uint blocksize (1024);
    double sampleFreq (200e06);
    int nyquistZone (2);
    uint blocksPerFrame (2);
    uint nofFrames (10);
    //
    TimeFreqCoordinate coord1 (blocksize,
			       sampleFreq,
			       nyquistZone,
			       blocksPerFrame,
			       nofFrames);
    coord1.summary(); 
    //
    TimeFreqCoordinate coord2 (blocksize,
			       sampleFreq,
			       nyquistZone,
			       blocksPerFrame,
			       nofFrames,
			       true);
    coord2.summary(); 
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  cout << "[4] Construction using TimeFreq object ..." << endl;
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
    TimeFreqCoordinate coord3 (tf,
			       blocksPerFrame,
			       nofFrames,
			       true);
    //
    cout << "--> extra parameters at default values ..." << endl;
    coord1.summary(); 
    cout << "--> custom values for extra parameters ..." << endl;
    coord2.summary(); 
    coord3.summary(); 
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  cout << "[5] Copy constructor ..." << endl;
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
  cout << "\n[tTimeFreqCoordinate::test_parameters]\n" << endl;

  int nofFailedTests (0);

  TimeFreqCoordinate coord;

  //________________________________________________________
  // Test of the methods inherited from the base class

  cout << "[1] Blocksize ..." << endl;
  try {
    /* current value */
    cout << "-- blocksize       = " << coord.blocksize()      << endl;
    cout << "-- reference value = " << coord.referenceValue() << endl;
    cout << "-- Increment       = " << coord.increment()      << endl;
    cout << "-- Shape           = " << coord.shape()          << endl;
    /* Adjust value */
    coord.setBlocksize (1024);
    /* New value */
    cout << "--> new values" << endl;
    cout << "-- blocksize       = " << coord.blocksize() << endl;
    cout << "-- reference value = " << coord.referenceValue() << endl;
    cout << "-- Increment       = " << coord.increment()      << endl;
    cout << "-- Shape           = " << coord.shape()          << endl;
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  cout << "[2] Sample frequency ..." << endl;
  try {
    /* current value */
    cout << "-- sampleFrequency = " << coord.sampleFrequency() << endl;
    cout << "-- reference value = " << coord.referenceValue() << endl;
    cout << "-- Increment       = " << coord.increment()      << endl;
    cout << "-- Shape           = " << coord.shape()          << endl;
    /* Adjust value */
    coord.setSampleFrequency (120e06);
    /* New value */
    cout << "--> new values" << endl;
    cout << "-- sampleFrequency = " << coord.sampleFrequency() << endl;
    cout << "-- reference value = " << coord.referenceValue() << endl;
    cout << "-- Increment       = " << coord.increment()      << endl;
    cout << "-- Shape           = " << coord.shape()          << endl;
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  cout << "[3] Nyquist zone ..." << endl;
  try {
    /* current value */
    cout << "-- nyquistZone     = " << coord.nyquistZone() << endl;
    cout << "-- reference value = " << coord.referenceValue() << endl;
    cout << "-- Increment       = " << coord.increment()      << endl;
    cout << "-- Shape           = " << coord.shape()          << endl;
    /* Adjust value */
    coord.setNyquistZone (2);
    /* New value */
    cout << "--> new values" << endl;
    cout << "-- nyquistZone     = " << coord.nyquistZone() << endl;
    cout << "-- reference value = " << coord.referenceValue() << endl;
    cout << "-- Increment       = " << coord.increment()      << endl;
    cout << "-- Shape           = " << coord.shape()          << endl;
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  cout << "[4] Reference time ..." << endl;
  try {
    /* current value */
    cout << "-- referenceTime   = " << coord.referenceTime() << endl;
    cout << "-- reference value = " << coord.referenceValue() << endl;
    cout << "-- Increment       = " << coord.increment()      << endl;
    cout << "-- Shape           = " << coord.shape()          << endl;
    /* Adjust value */
    coord.setReferenceTime (-0.5);
    /* New value */
    cout << "--> new values" << endl;
    cout << "-- referenceTime   = " << coord.referenceTime() << endl;
    cout << "-- reference value = " << coord.referenceValue() << endl;
    cout << "-- Increment       = " << coord.increment()      << endl;
    cout << "-- Shape           = " << coord.shape()          << endl;
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  //________________________________________________________
  // Test of the methods added by TimeFreqCoordinate

  cout << "[5] Coordinate type of the target domain ..." << endl;
  try {
    /* current settings */
    cout << "-- Target domain   = " << coord.coordType().name() << endl;
    cout << "-- blocksPerFrame  = " << coord.blocksPerFrame() << endl;
    cout << "-- reference value = " << coord.referenceValue() << endl;
    cout << "-- Increment       = " << coord.increment()      << endl;
    cout << "-- Shape           = " << coord.shape()          << endl;
    /* adjust the value */
    coord.setCoordType(CoordinateType::Time);
    /* new settings */
    cout << "--> new values" << endl;
    cout << "-- Target domain   = " << coord.coordType().name() << endl;
    cout << "-- blocksPerFrame  = " << coord.blocksPerFrame() << endl;
    cout << "-- reference value = " << coord.referenceValue() << endl;
    cout << "-- Increment       = " << coord.increment()      << endl;
    cout << "-- Shape           = " << coord.shape()          << endl;
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  cout << "[6] Input data blocks per time-frame ..." << endl;
  try {
    /* current value */
    cout << "-- Target domain   = " << coord.coordType().name() << endl;
    cout << "-- blocksPerFrame  = " << coord.blocksPerFrame() << endl;
    cout << "-- reference value = " << coord.referenceValue() << endl;
    cout << "-- Increment       = " << coord.increment()      << endl;
    cout << "-- Shape           = " << coord.shape()          << endl;
    /* Adjust value */
    coord.setBlocksPerFrame (10,false);
    /* New value */
    cout << "--> new values" << endl;
    cout << "-- Target domain   = " << coord.coordType().name() << endl;
    cout << "-- blocksPerFrame  = " << coord.blocksPerFrame() << endl;
    cout << "-- reference value = " << coord.referenceValue() << endl;
    cout << "-- Increment       = " << coord.increment()      << endl;
    cout << "-- Shape           = " << coord.shape()          << endl;
    /* adjust value */
    coord.setBlocksPerFrame (10,true);
    /* New value */
    cout << "--> new values" << endl;
    cout << "-- Target domain   = " << coord.coordType().name() << endl;
    cout << "-- blocksPerFrame  = " << coord.blocksPerFrame() << endl;
    cout << "-- reference value = " << coord.referenceValue() << endl;
    cout << "-- Increment       = " << coord.increment()      << endl;
    cout << "-- Shape           = " << coord.shape()          << endl;
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  cout << "[7] Number of frames ..." << endl;
  try {
    /* current value */
    cout << "-- nofFrames       = " << coord.nofFrames() << endl;
    cout << "-- reference value = " << coord.referenceValue() << endl;
    cout << "-- Increment       = " << coord.increment()      << endl;
    cout << "-- Shape           = " << coord.shape()          << endl;
    /* Adjust value */
    coord.setNofFrames (100);
    /* New value */
    cout << "--> new values" << endl;
    cout << "-- nofFrames       = " << coord.nofFrames() << endl;
    cout << "-- reference value = " << coord.referenceValue() << endl;
    cout << "-- Increment       = " << coord.increment()      << endl;
    cout << "-- Shape           = " << coord.shape()          << endl;
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  cout << "[8] Switch ordering of axes to have time last ..." << endl;
  try {
    /* current value */
    cout << "-- Time axis last?  = " << coord.timeAxisLast()   << endl;
    cout << "-- World axis names = " << coord.worldAxisNames() << endl;
    cout << "-- World axis units = " << coord.worldAxisUnits() << endl;
    cout << "-- Reference value  = " << coord.referenceValue() << endl;
    cout << "-- Increment        = " << coord.increment()      << endl;
    cout << "-- Shape            = " << coord.shape()          << endl;
    /* Adjust value */
    coord.setTimeAxisLast (true);
    /* New value */
    cout << "--> new values" << endl;
    cout << "-- Time axis last?  = " << coord.timeAxisLast()   << endl;
    cout << "-- World axis names = " << coord.worldAxisNames() << endl;
    cout << "-- World axis units = " << coord.worldAxisUnits() << endl;
    cout << "-- Reference value  = " << coord.referenceValue() << endl;
    cout << "-- Increment        = " << coord.increment()      << endl;
    cout << "-- Shape            = " << coord.shape()          << endl;
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
  cout << "\n[tTimeFreqCoordinate::test_methods]\n" << endl;

  int nofFailedTests (0);
  
  uint blocksize (1024);
  double sampleFreq (200e06);
  int nyquistZone (2);
  uint blocksPerFrame (2);
  uint nofFrames (25);

  TimeFreqCoordinate coord (blocksize,
			    sampleFreq,
			    nyquistZone,
			    blocksPerFrame,
			    nofFrames);
  
  cout << "[1] Coordinate objects for the axes .." << endl;
  try {
    cout << "--> time axis" << endl;
    casa::LinearCoordinate time = coord.timeAxisCoordinate();
    CoordinateType::summary (cout,time);
    //
    cout << "--> frequency axis" << endl;
    casa::SpectralCoordinate freq = coord.frequencyAxisCoordinate();
    CoordinateType::summary (cout,freq);
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  cout << "[2] Add coordinates objects to a coordinate system ..." << endl;
  try {
    casa::CoordinateSystem csys;
    // 
    coord.toCoordinateSystem (csys);
    //
    cout << "--> add coordinates to new coordinate system ..." << endl;
    cout << "-- nof. coordinates = " << csys.nCoordinates()    << endl;
    cout << "-- World axis names = " << csys.worldAxisNames()  << endl;
    cout << "-- Reference pixel  = " << csys.referencePixel()  << endl;
    cout << "-- Increment        = " << csys.increment()       << endl;
    cout << "-- Reference value  = " << csys.referenceValue()  << endl;
    //
    coord.toCoordinateSystem (csys);
    //
    cout << "--> append coordinates to coordinate system ..." << endl;
    cout << "-- nof. coordinates = " << csys.nCoordinates()   << endl;
    cout << "-- World axis names = " << csys.worldAxisNames()  << endl;
    cout << "-- Reference pixel  = " << csys.referencePixel() << endl;
    cout << "-- Increment        = " << csys.increment()      << endl;
    cout << "-- Reference value  = " << csys.referenceValue() << endl;
    // 
    coord.toCoordinateSystem (csys,false);
    //
    cout << "--> overwrite contents of existing coordinate system ..." << endl;
    cout << "-- nof. coordinates = " << csys.nCoordinates()   << endl;
    cout << "-- World axis names = " << csys.worldAxisNames()  << endl;
    cout << "-- Reference pixel  = " << csys.referencePixel() << endl;
    cout << "-- Increment        = " << csys.increment()      << endl;
    cout << "-- Reference value  = " << csys.referenceValue() << endl;
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  cout << "[3] Conversion between pixel and world coordinates...." << endl;
  try {
    casa::Vector<double> pixel (2);
    casa::Vector<double> world (2);
    //
    cout << "-- World axis names = " << coord.worldAxisNames() << endl;
    cout << "-- Reference pixel  = " << coord.referencePixel() << endl;
    cout << "-- Increment        = " << coord.increment()      << endl;
    cout << "-- Reference value  = " << coord.referenceValue() << endl;
    //
    cout << "--> conversion from pixel to world ..." << endl;
    //
    pixel(0) = 0;
    pixel(1) = 0;
    coord.toWorld (world,pixel);
    cout << "\t" << pixel << " -> " << world << endl;
    //
    pixel(0) = 1;
    pixel(1) = 0;
    coord.toWorld (world,pixel);
    cout << "\t" << pixel << " -> " << world << endl;
    //
    pixel(0) = 0;
    pixel(1) = 1;
    coord.toWorld (world,pixel);
    cout << "\t" << pixel << " -> " << world << endl;
    //
    pixel(0) = 1;
    pixel(1) = 1;
    coord.toWorld (world,pixel);
    cout << "\t" << pixel << " -> " << world << endl;
    //
    cout << "--> conversion from world to pixel ..." << endl;
    //
    world(0) = 0;
    world(1) = 0;
    coord.toPixel(pixel,world);
    cout << "\t" << world << " -> " << pixel << endl;
    //
    world(0) = 1;
    world(1) = 0;
    coord.toPixel(pixel,world);
    cout << "\t" << world << " -> " << pixel << endl;
    //
    world(0) = 2;
    world(1) = 0;
    coord.toPixel(pixel,world);
    cout << "\t" << world << " -> " << pixel << endl;
    //
    world(0) = 0;
    world(1) = 100e06;
    coord.toPixel(pixel,world);
    cout << "\t" << world << " -> " << pixel << endl;
    //
    world(0) = 0;
    world(1) = 200e06;
    coord.toPixel(pixel,world);
    cout << "\t" << world << " -> " << pixel << endl;
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  cout << "[4] Retrieve time values ..." << endl;
  try {
    Vector<double> times = coord.timeValues();
    uint nelem           = times.nelements();
    //
    cout << "-- nof. frames = " << nelem << endl;
    cout << "-- Time values = [ "
	 << times(0) << " "
	 << times(1) << " "
	 << times(2) << " ... "
	 << times(nelem-2) << " "
	 << times(nelem-1) << " ]" << endl;
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  cout << "[5] Retrieve frequency values ..." << endl;
  try {
    Vector<double> frequencies = coord.frequencyValues();
    uint nelem                 = frequencies.nelements();
    //
    cout << "-- nof. channels    = " << nelem << endl;
    cout << "-- Frequency values = [ "
	 << frequencies(0) << " "
	 << frequencies(1) << " "
	 << frequencies(2) << " ... "
	 << frequencies(nelem-2) << " "
	 << frequencies(nelem-1) << " ]" << endl;
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  cout << "[6] Retrieve world axis values of both axes ..." << endl;
  try {
    Matrix<double> values1 = coord.worldAxisValues(true);
    casa::IPosition shape1 = values1.shape();

    cout << "-- fastest varying axis first ..." << endl;
    cout << "\t" << values1.row(0) << endl;
    cout << "\t" << values1.row(1) << endl;
    cout << "\t" << values1.row(2) << endl;
    cout << "\t" << values1.row(3) << endl;
    cout << "\t..." << endl;
    cout << "\t" << values1.row(shape1(0)-3) << endl;
    cout << "\t" << values1.row(shape1(0)-2) << endl;
    cout << "\t" << values1.row(shape1(0)-1) << endl;

    Matrix<double> values2 = coord.worldAxisValues(false);
    casa::IPosition shape2 = values2.shape();

    cout << "-- fastest varying axis last ..." << endl;
    cout << "\t" << values2.row(0) << endl;
    cout << "\t" << values2.row(1) << endl;
    cout << "\t" << values2.row(2) << endl;
    cout << "\t" << values2.row(3) << endl;
    cout << "\t..." << endl;
    cout << "\t" << values2.row(shape2(0)-3) << endl;
    cout << "\t" << values2.row(shape2(0)-2) << endl;
    cout << "\t" << values2.row(shape2(0)-1) << endl;
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
