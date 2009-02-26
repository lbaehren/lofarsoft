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

#include <Coordinates/SkymapCoordinate.h>
#include <Observation/ObservationData.h>

// Namespace usage
using std::cout;
using std::endl;
using CR::ObservationData;
using CR::SkymapCoordinate;

/*!
  \file tSkymapCoordinate.cc

  \ingroup CR_Coordinates

  \brief A collection of test routines for the SkymapCoordinate class
 
  \author Lars B&auml;hren
 
  \date 2009/01/06
*/

// -----------------------------------------------------------------------------

/*!
  \brief Test constructors for a new SkymapCoordinate object

  \return nofFailedTests -- The number of failed tests encountered within this
          function.
*/
int test_constructors ()
{
  cout << "\n[tSkymapCoordinate::test_constructors]\n" << endl;

  int nofFailedTests     = 0;
  std::string telescope  = "LOFAR";
  std::string observer   = "Lars Baehren";
  std::string refcode    = "AZEL";
  std::string projection = "SIN";
  uint blocksize         = 1024;
  casa::Quantity sampleFreq (200,"MHz");
  uint nyquistZone (1);
  uint blocksPerFrame (1);
  uint nofFrames (10);
  
  cout << "[1] Testing default constructor ..." << endl;
  try {
    SkymapCoordinate coord;
    //
    coord.summary(); 
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  cout << "[2] Testing construction from observation data and time-freq ..." << endl;
  try {
    ObservationData obsData (telescope,observer);
    TimeFreqCoordinate timeFreq (blocksize,
				 sampleFreq,
				 nyquistZone,
				 blocksPerFrame,
				 nofFrames);
    //
    std::cout << "-- blocksize        = " << blocksize      << std::endl;
    std::cout << "-- sample frequency = " << sampleFreq     << std::endl;
    std::cout << "-- Nyquist zone     = " << nyquistZone    << std::endl;
    std::cout << "-- blocks per frame = " << blocksPerFrame << std::endl;
    //
    SkymapCoordinate coord1 (obsData,
			    timeFreq,
			     CR::SkymapQuantity::FREQ_POWER);
    coord1.summary(); 
    //
    SkymapCoordinate coord2 (obsData,
			    timeFreq,
			     CR::SkymapQuantity::TIME_CC);
    coord2.summary(); 
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  cout << "[3] Testing construction for (Direction,Radius) coordinate ..." << endl;
  try {
    ObservationData obsData (telescope,observer);
    TimeFreqCoordinate timeFreq (blocksize,
				 sampleFreq,
				 nyquistZone,
				 blocksPerFrame,
				 nofFrames);
    SpatialCoordinate spatial (CR::CoordinateType::DirectionRadius,
			       refcode,
			       projection);
    spatial.setShape(casa::IPosition(3,100,100,10));
    //
    SkymapCoordinate coord (obsData,
			    spatial,
			    timeFreq);
    //
    coord.summary();
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  cout << "[4] Testing construction for cartesian coordinate ..." << endl;
  try {
    ObservationData obsData (telescope,observer);
    TimeFreqCoordinate timeFreq (blocksize,
				 sampleFreq,
				 nyquistZone,
				 blocksPerFrame,
				 nofFrames);
    SpatialCoordinate spatial (CR::CoordinateType::Cartesian,
			       refcode,
			       projection);
    spatial.setShape(casa::IPosition(3,100,100,10));
    //
    SkymapCoordinate coord1 (obsData,
			     spatial,
			     timeFreq,
			     CR::SkymapQuantity::FREQ_POWER);
    coord1.summary();
    //
    SkymapCoordinate coord2 (obsData,
			     spatial,
			     timeFreq,
			     CR::SkymapQuantity::TIME_CC);
    coord2.summary();
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  cout << "[5] Testing copy constructor ..." << endl;
  try {
    ObservationData obsData (telescope,observer);
    TimeFreqCoordinate timeFreq (blocksize,
				 sampleFreq,
				 nyquistZone,
				 blocksPerFrame,
				 nofFrames);
    SpatialCoordinate spatial (CR::CoordinateType::DirectionRadius,
			       refcode,
			       projection);
    spatial.setShape(casa::IPosition(3,100,100,10));
    //
    SkymapCoordinate coord (obsData,
			    spatial,
			    timeFreq);
    coord.summary();
    //
    SkymapCoordinate coordCopy (coord);
    coordCopy.summary();
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  return nofFailedTests;
}

// -----------------------------------------------------------------------------

/*!
  \brief Test methods to access internal parameters

  \return nofFailedTests -- The number of failed tests encountered within this
          function.
*/
int test_methods ()
{
  cout << "\n[tSkymapCoordinate::test_methods]\n" << endl;

  int nofFailedTests (0);
  
  /* Construct objects to perform the tests with */
  uint blocksize         = 1024;
  casa::Quantity sampleFreq (200,"MHz");
  uint nyquistZone (1);
  uint blocksPerFrame (1);
  uint nofFrames (10);
  ObservationData obsData ("LOFAR",
			   "Lars Baehren");
  TimeFreqCoordinate timeFreq (blocksize,
			       sampleFreq,
			       nyquistZone,
			       blocksPerFrame,
			       nofFrames);
  SpatialCoordinate spatial (CR::CoordinateType::DirectionRadius,
			     "AZEL",
			     "SIN");
  spatial.setShape(casa::IPosition(3,100,100,10));
  // 
  SkymapCoordinate coord (obsData,
			  spatial,
			  timeFreq);
  
  /* Methods of the class itself */

  cout << "[1] Coordinate properties ..." << endl;
  try {
    cout << "-- nof. coordinate axes = " << coord.nofAxes()         << endl;
    cout << "-- nof. coordinates     = " << coord.nofCoordinates()  << endl;
    cout << "-- Shape of the axes    = " << coord.shape()           << endl;
    cout << "-- World axis names     = " << coord.worldAxisNames()  << endl;
    cout << "-- World axis units     = " << coord.worldAxisUnits()  << endl;
    cout << "-- Reference pixel      = " << coord.referencePixel()  << endl;
    cout << "-- Increment            = " << coord.increment()       << endl;
    cout << "-- Reference value      = " << coord.referenceValue()  << endl;
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  cout << "[2] Adjust SkymapQuantity ..." << endl;
  try {
    /* current value */
    cout << "--> parameter summary before adjustment:" << endl;
    coord.summary();
    /* assign new value */
    coord.setSkymapQuantity(CR::SkymapQuantity::TIME_CC);
    /* new value */
    cout << "--> parameter summary after adjustment:" << endl;
    coord.summary();
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  /* Methods of the embedded objects */

  cout << "[3] Parameters of the TimeFreqCoordinate ..." << endl;
  try {
    cout << "-- blocksize         = " << coord.timeFreqCoordinate().blocksize()
	 << endl;
    cout << "-- nof. axes         = " << coord.timeFreqCoordinate().nofAxes()
	 << endl;
    cout << "-- nof. coordinates  = " << coord.timeFreqCoordinate().nofCoordinates() 
	 << endl;
    cout << "-- Shape of the axes = " << coord.timeFreqCoordinate().shape()
	 << endl;
    cout << "-- World axis names  = " << coord.timeFreqCoordinate().worldAxisNames()
	 << endl;
    cout << "-- World axis units  = " << coord.timeFreqCoordinate().worldAxisUnits()
	 << endl;
    cout << "-- Reference pixel   = " << coord.timeFreqCoordinate().referencePixel()
	 << endl;
    cout << "-- Increment         = " << coord.timeFreqCoordinate().increment()
	 << endl;
    cout << "-- Reference value   = " << coord.timeFreqCoordinate().referenceValue()
	 << endl;
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  return nofFailedTests;
}

// -----------------------------------------------------------------------------

/*!
  \brief Tests for the embedded coordinate system object

  \return nofFailedTests -- The number of failed tests encountered within this
          function.
*/
int test_coordinateSystem ()
{
  cout << "\n[tSkymapCoordinate::test_coordinateSystem]\n" << endl;

  int nofFailedTests (0);

  // Parameters used for object creation
  std::string telescope  = "LOFAR";
  std::string observer   = "Lars Baehren";
  std::string refcode    = "AZEL";
  std::string projection = "SIN";
  uint blocksize         = 1024;
  casa::Quantity sampleFreq (200,"MHz");
  uint nyquistZone       = 1;
  uint blocksPerFrame    = 1;
  uint nofFrames         = 10;

  // Coordinate objects
  ObservationData obsData (telescope,
			   observer);
  TimeFreqCoordinate timeFreq (blocksize,
			       sampleFreq,
			       nyquistZone,
			       blocksPerFrame,
			       nofFrames);

  SpatialCoordinate spatial (CR::CoordinateType::DirectionRadius,
			     refcode,
			     projection);
  spatial.setShape(casa::IPosition(3,50,50,10));
  SkymapCoordinate coord (obsData,
			  spatial,
			  timeFreq);

  cout << "[1] Conversion from pixel to world coordinates ..." << endl;
  try {
    Vector<double> pixel (coord.nofAxes());
    Vector<double> world (coord.nofAxes());

    // conversion of reference pixel
    pixel = coord.referencePixel();
    coord.toWorld (world,pixel);
    cout << "\t" << pixel << " -> " << world << endl;

    // +1 offset from the reference pixel for each coordinate axis
    for (uint n(0); n<coord.nofAxes(); n++) {
      pixel = coord.referencePixel();
      pixel(n) += 1;
      coord.toWorld (world,pixel);
      cout << "\t" << pixel << " -> " << world << endl;
    }
    
    // simultaneous offset by +1 from reference pixel
    pixel = coord.referencePixel();
    for (uint n(0); n<coord.nofAxes(); n++) {
      pixel(n) += 1;
    }
    coord.toWorld (world,pixel);
    cout << "\t" << pixel << " -> " << world << endl;
    
    } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  cout << "[3] Conversion from world to pixel coordinates ..." << endl;
  try {
    Vector<double> pixel (coord.nofAxes());
    Vector<double> world (coord.nofAxes());

    // pixel coordinates of zero position in world coordintes
    world = 0;
    coord.toPixel(pixel,world);
    cout << "\t" << world << " -> " << pixel << endl;
    
    // conversion from reference value to reference pixel
    world = coord.referenceValue();
    coord.toPixel(pixel,world);
    cout << "\t" << world << " -> " << pixel << endl;
    
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}

// -----------------------------------------------------------------------------

/*!
  \brief Test retrieval of the world axis values

  \return nofFailedTests -- The number of failed tests encountered within this
          function.
*/
int test_worldAxisValues ()
{
  cout << "\n[tSkymapCoordinate::test_worldAxisValues]\n" << endl;

  int nofFailedTests (0);

  // Parameters used for object creation
  std::string telescope  = "LOFAR";
  std::string observer   = "Lars Baehren";
  std::string refcode    = "AZEL";
  std::string projection = "SIN";
  uint blocksize         = 1024;
  casa::Quantity sampleFreq (200,"MHz");
  uint nyquistZone (1);
  uint blocksPerFrame (1);
  uint nofFrames (10);
  
  // Coordinate objects
  ObservationData obsData (telescope,observer);
  TimeFreqCoordinate timeFreq (blocksize,
			       sampleFreq,
			       nyquistZone,
			       blocksPerFrame,
			       nofFrames);
  SpatialCoordinate spatial (CR::CoordinateType::DirectionRadius,
			     refcode,
			     projection);
  spatial.setShape(casa::IPosition(3,20,20,10));
  SkymapCoordinate coord (obsData,
			  spatial,
			  timeFreq);
  
  cout << "[1] Retrieve time values ..." << endl;
  try {
    Vector<double> times = coord.timeFreqCoordinate().timeValues();
    
    cout << "\t[" << times(0) << " " << times(1) << " " << times(2) << " ... "
	 << times(times.nelements()-1) << "]" << endl;
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  cout << "[2] Retrieve frequency values ..." << endl;
  try {
    Vector<double> freq = coord.timeFreqCoordinate().frequencyValues();
    
    cout << "\t[" << freq(0) << " " << freq(1) << " " << freq(2) << " ... "
	 << freq(freq.nelements()-1) << "]" << endl;
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  cout << "[3] Retrieve position values ..." << endl;
  try {
    Matrix<double> positions = coord.spatialCoordinate().positionValues();
    IPosition shape          = positions.shape();

    cout << "\t" << positions.row(0) << endl;
    cout << "\t" << positions.row(1) << endl;
    cout << "\t" << positions.row(2) << endl;
    cout << "\t" << positions.row(3) << endl;
    cout << "\t..." << endl;
    cout << "\t" << positions.row(shape(0)-3) << endl;
    cout << "\t" << positions.row(shape(0)-2) << endl;
    cout << "\t" << positions.row(shape(0)-1) << endl;
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  cout << "[4] Retrieve world axis values of skymap coordinates ..." << endl;
  try{
    // adjust internal parameters not to exceed memory
    timeFreq.setBlocksize(100);
    coord.setTimeFreqCoordinate(timeFreq);

    Matrix<double> values = coord.worldAxisValues();
    IPosition shape       = values.shape();

    cout << "\t" << values.row(0) << endl;
    cout << "\t" << values.row(1) << endl;
    cout << "\t" << values.row(2) << endl;
    cout << "\t" << values.row(3) << endl;
    cout << "\t" << values.row(4) << endl;
    cout << "\t" << values.row(5) << endl;
    cout << "\t..." << endl;
    cout << "\t" << values.row(shape(0)/2-1) << endl;
    cout << "\t" << values.row(shape(0)/2) << endl;
    cout << "\t" << values.row(shape(0)/2+1) << endl;
    cout << "\t..." << endl;
    cout << "\t" << values.row(shape(0)-3) << endl;
    cout << "\t" << values.row(shape(0)-2) << endl;
    cout << "\t" << values.row(shape(0)-1) << endl;
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
  // Test methods to access internal parameters
  nofFailedTests += test_methods();
  // Tests for the embedded coordinate system object
  nofFailedTests += test_coordinateSystem ();
  // Test retrieval of data from the embedded objects
  nofFailedTests += test_worldAxisValues();

  return nofFailedTests;
}
