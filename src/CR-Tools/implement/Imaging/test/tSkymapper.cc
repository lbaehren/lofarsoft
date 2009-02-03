/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2006                                                    *
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

#include <casa/aips.h>
#include <casa/Arrays/ArrayIO.h>
#include <casa/Arrays/Vector.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Exceptions/Error.h>
#include <casa/System/Aipsrc.h>
#include <casa/System/ProgressMeter.h>
#include <casa/OS/Directory.h>
#include <images/Images/PagedImage.h>

using casa::AipsError;
using casa::Array;
using casa::IPosition;
using casa::PagedImage;
using casa::TiledShape;

#include <Coordinates/SkymapCoordinate.h>
#include <Imaging/Beamformer.h>
#include <Imaging/Skymapper.h>
#include <Utilities/ProgressBar.h>
#include "create_data.h"

using CR::Beamformer;
using CR::ObservationData;
using CR::ProgressBar;
using CR::SkymapCoordinate;
using CR::Skymapper;
using CR::TimeFreq;

/*!
  \file tSkymapper.cc

  \ingroup CR_Imaging

  \brief A collection of test routines for Skymapper
 
  \author Lars B&auml;hren
 
  \date 2006/01/23

  <h3>Synopsis</h3>

  There are quite a number of tests for better understanding the functionality
  of the classes derived from casa::ImageInterface:
  <ol>
    <li>Basic creation of a new image, either of type casa::PagedImage or
    casa::HDF5Image.
  </ol>
*/

// -----------------------------------------------------------------------------

/*!
  \brief Display some basic characteristics of an AIPS++ image tool

  \param myimage -- An AIPS++ PagedImage object, usually used for storing 
                    astronomical images.

  \verbatim
  [1] Opening AIPS++ test image from disk ... 
  -- Name           : /dop71_0/aips++/weekly/data/demo/Images/test_image
  -- Shape          : [32, 32, 8]
  -- is persistent  : 1
  -- is paged       : 1
  -- is writable    : 0
  -- has pixel mask : 0
  \endverbatim
*/
template <class T>
void show_PagedImage (const casa::PagedImage<T>& myimage)
{
  casa::IPosition shape (myimage.shape());
  int nofAxes (shape.nelements());
  double nofPixes (1.0);
  bool stripPath (true);

  for (int n(0); n<nofAxes; n++) {
    nofPixes *= shape(n);
  }

//   cout.precision(10);
  cout << " -- Name             : " << myimage.name(stripPath) << endl;
  cout << " -- Image shape      : " << shape << endl;
  cout << " -- Number of pixels : " << nofPixes << endl;
  cout << " -- is persistent    : " << myimage.isPersistent() << endl;
  cout << " -- is paged         : " << myimage.isPaged() << endl;
  cout << " -- is writable      : " << myimage.isWritable() << endl;
  cout << " -- has pixel mask   : " << myimage.hasPixelMask() << endl;
}

template void show_PagedImage (const casa::PagedImage<Float>& myimage);
template void show_PagedImage (const casa::PagedImage<Double>& myimage);

// -----------------------------------------------------------------------------

/*!
  \brief Read in the antenna positions from an ASCII table on disk

  \return antennaPositions -- Matrix with the 3-dim antenna positions
*/
Matrix<Double> readAntennaPositions ()
{
  Matrix<Double> pos;
  readAsciiMatrix (pos,positions_lopes08.c_str());

  return pos;
}

// -----------------------------------------------------------------------------

/*!
  \brief Clean up the directory once the tests have completed

  \return nofFailedTests -- The number of failed operations; this e.g. might
          happen if we try to remove a file/directory which does not exist
	  because the test creating it in the first place failed.
*/
int cleanup_directory ()
{
  int nofFailedTests (0);

  std::vector<std::string> images;

  images.push_back ("tPagedImage01.img");
  images.push_back ("tPagedImage02.img");
  images.push_back ("tPagedImage03.img");
  images.push_back ("tPagedImage04.img");
  
  for (unsigned int n(0); n<images.size(); n++) {
    try {
      casa::Directory dir (images[n].c_str());
      dir.removeRecursive(); 
    } catch (AipsError x) {
      cerr << "[tSkymapper::cleanup_directory] " << x.getMesg() << endl;
      nofFailedTests++;
    }
  }
  
  return nofFailedTests;
}

// -----------------------------------------------------------------------------

/*!
  \brief Test working with AIPS++ CoordinateSystem objects

  \return nofFailedTests

  CoordinateSystem is the normal interface to coordinate systems, typically
  attached to an ImageInterface, however the coordinate system can be manipulated
  on its own. CoordinateSystem is in turn composed from various classes derived
  from the base class Coordinate.
*/
int test_CoordinateSystem ()
{
  cout << "\n[tSkymapper::test_CoordinateSystem]\n" << endl;

  int nofFailedTests (0);

  cout << "\n[1] Construct linear coordinate and afterwards adjust parameters ..."
       << endl;
  try {
    Vector<String> names (1,"Time");
    Vector<String> units (1,"s");
    Vector<Double> crval (1,0.0);
    Vector<Double> cdelt (1,1.0);
    Matrix<Double> pc    (1,1,1.0);
    Vector<Double> crpix (1,0.0);
    
    LinearCoordinate axis (names,
			   units,
			   crval,
			   cdelt,
			   pc,
			   crpix);

    cout << " - initial values:" << endl;
    cout << " -- names = " << axis.worldAxisNames() << endl;
    cout << " -- units = " << axis.worldAxisUnits() << endl;
    cout << " -- crpix = " << axis.referencePixel() << endl;
    cout << " -- crval = " << axis.referenceValue() << endl;
    cout << " -- cdelt = " << axis.increment()      << endl;

    // adjust the axis parameters
    crval(0) = 3600.0;
    cdelt(0) = 60.0;

    axis.setReferenceValue (crval);
    axis.setIncrement (cdelt);

    cout << " - modified values:" << endl;
    cout << " -- names = " << axis.worldAxisNames() << endl;
    cout << " -- units = " << axis.worldAxisUnits() << endl;
    cout << " -- crpix = " << axis.referencePixel() << endl;
    cout << " -- crval = " << axis.referenceValue() << endl;
    cout << " -- cdelt = " << axis.increment()      << endl;
    
  } catch (AipsError x) {
    cerr << "[tSkymapper::test_CoordinateSystem] " << x.getMesg() << endl;
    nofFailedTests++;
  }
    

  return nofFailedTests;
}

// -----------------------------------------------------------------------------

/*!
  \brief Test setting up the Beamformer using a SkymapCoordinate object

  \return nofFailedTests -- The number of failed tests.
*/
int test_Beamformer (uint const &blocksize)
{
  cout << "\n[tSkymapper::test_Beamformer]\n" << endl;

  int nofFailedTests (0);
  SkymapCoordinate coord;
  Beamformer bf;

  /* Initial state of the objects to be embedded into the Skymapper */
  coord.summary();
  bf.summary();

  cout << "[1] Add observation data to the coordinate ..." << endl;;
  try {
    std::string telescope ("LOFAR");
    std::string observer ("Lars Baehren");
    CR::ObservationData obsData (telescope,observer);
    //
    cout << "-- Telescope = " << telescope << endl;
    cout << "-- Observer  = " << observer  << endl;
    //
    coord.setObservationData (obsData);
  } catch (AipsError x) {
    cerr << x.getMesg() << endl;
    nofFailedTests++;
  }

  cout << "[2] Set the time-frequency coordinate ..." << endl;
  try {
    casa::Quantity sampleFreq (200,"MHz");
    uint nyquistZone (1);
    uint blocksPerFrame (1);
    uint nofFrames (10);
    //
    cout << "-- Blocksize        = " << blocksize      << endl;
    cout << "-- Sample frequency = " << sampleFreq     << endl;
    cout << "-- Nyquist zone     = " << nyquistZone    << endl;
    cout << "-- Blocks per frame = " << blocksPerFrame << endl;
    cout << "-- nof. frames      = " << nofFrames      << endl;
    //
    TimeFreqCoordinate timeFreq (blocksize,
				 sampleFreq,
				 nyquistZone,
				 blocksPerFrame,
				 nofFrames);
    //
    coord.setTimeFreqCoordinate(timeFreq);
  } catch (AipsError x) {
    cerr << x.getMesg() << endl;
    nofFailedTests++;
  }

  cout << "[3] Set the spatial coordinate ..." << endl;
  try {
    IPosition shape (3,20,20,10);
    //
    SpatialCoordinate azel (CoordinateType::DirectionRadius,
			    "AZEL"
			    "SIN");
    azel.setShape(shape);
    //
    cout << "-- Reference system      = " << azel.directionRefcode() << endl;
    cout << "-- Map projection        = " << azel.projection()       << endl;
    cout << "-- Coordinate axes shape = " << azel.shape()            << endl;
    //
    coord.setSpatialCoordinate(azel);
    coord.summary();
  } catch (AipsError x) {
    cerr << x.getMesg() << endl;
    nofFailedTests++;
  }

  cout << "[4] Set positions and frequencies ..." << endl;
  try {
    Vector<MVPosition> antPos (10);
    Matrix<double> skyPos = coord.spatialCoordinate().positionValues();
    Vector<double> freq   = coord.timeFreqCoordinate().frequencyValues();
    // 
    bf.setAntPositions(antPos);
    bf.setSkyPositions(skyPos);
    bf.setFrequencies(freq);
    bf.summary();
  } catch (AipsError x) {
    cerr << x.getMesg() << endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}

// -----------------------------------------------------------------------------

/*!
  \brief Test constructors for a new Skymapper object

  \return nofFailedTests -- The number of failed tests.
*/
int test_Skymapper ()
{
  cout << "\n[tSkymapper::test_Skymapper]\n" << endl;

  int nofFailedTests (0);

  cout << "[1] Skymapper() ..." << endl;
  try {
    Skymapper skymapper;
    //
    skymapper.summary();
  } catch (AipsError x) {
    cerr << "[tSkymapper::test_Skymapper] " << x.getMesg() << endl;
    nofFailedTests++;
  }
  
  cout << "[2] Skymapper(SkymapCoordinate) ..." << endl;
  try {
    // Time-frequency domain data
    TimeFreqCoordinate timeFreq (2048,80e06,2);
    // Observation info
    ObservationData obsData ("LOFAR");
    obsData.setObserver ("Lars Baehren");
    // Coordinates 
    SkymapCoordinate coord;
    coord.setTimeFreqCoordinate(timeFreq);
    coord.setObservationData(obsData);
    
    Skymapper skymapper (coord);
    
    skymapper.summary();
  } catch (AipsError x) {
    cerr << "[tSkymapper::test_Skymapper] " << x.getMesg() << endl;
    nofFailedTests++;
  }
  
  cout << "[3] Skymapper(SkymapCoordinate,Matrix<double>,SkymapQuantity) ..."
       << endl;
  try {
    // Observation data
    std::string telescope ("LOFAR");
    std::string observer ("Lars Baehren");
    CR::ObservationData obsData (telescope,observer);
    // Spatial coordinates
    IPosition shape (3,20,20,10);
    SpatialCoordinate spatial (CoordinateType::DirectionRadius,
			       "AZEL"
			       "SIN");
    spatial.setShape(shape);
    // Time-Frequency coordinate
    TimeFreqCoordinate timeFreq;
    // Skymap coordinate
    SkymapCoordinate coord (obsData,
			    spatial,
			    timeFreq);
    // Antenna positions
    uint nofAntennas (10);
    Matrix<double> antPositions (nofAntennas,3);
    //
    Skymapper skymapper (coord,
			 antPositions,
			 CR::SkymapQuantity());
    skymapper.summary();
  } catch (AipsError x) {
    cerr << "[tSkymapper::test_Skymapper] " << x.getMesg() << endl;
    nofFailedTests++;
  }

  cout << "[4] Logging of internal parameters ..." << endl;
  try {
    std::ofstream logfile;
    Skymapper skymapper;

    logfile.open("tSkymapper.log");

    skymapper.summary(logfile);

    logfile.close();
  } catch (AipsError x) {
    cerr << "[tSkymapper::test_Skymapper] " << x.getMesg() << endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}

// -----------------------------------------------------------------------------

/*!
  \brief Test processing of the data to generate an image

  \param infile -- LOPES data set to use as data input
  \param blocksize -- Number of samples per block of data.

  \return nofFailedTests -- The number of failed tests.
*/
int test_processing (string const &infile,
		     uint const &blocksize)
{
  cout << "\n[tSkymapper::test_processing]\n" << endl;
  
  int nofFailedTests (0);
  bool status (true);

  std::cout << "[1] Test init function for default object..." << std::endl;
  try {
    Skymapper skymapper;
    // set the name of the created image file
    skymapper.setFilename ("skymap01.img");
    // provide a summary of the internal settings
    skymapper.summary();
    // initialize the Skymapper for operation on data
    status = skymapper.initSkymapper();
  } catch (std::string message) {
    cerr << message << endl;
    nofFailedTests++;
  }
  
  std::cout << "[2] Test init function for custom object..." << std::endl;
  try {
    Skymapper skymapper (get_SkymapCoordinate());
    // enable additional feedback during processing
    skymapper.setVerboseLevel(1);
    // set the name of the created image file
    skymapper.setFilename ("skymap02.img");
    // provide a summary of the internal settings
    skymapper.summary();
    // initialize the Skymapper for operation on data
    status = skymapper.initSkymapper();
  } catch (std::string message) {
    cerr << message << endl;
    nofFailedTests++;
  }
  
  std::cout << "[3] Test processData function with generated data..." << std::endl;
  try {
    uint blocksize (1024);
    uint nofDataBlocks (3);
    uint nofAntennas (2);
    // --- create SkymapCoordinates object ---
    std::cout << "-- creating SkymapCoordinates object..." << std::endl;
    CR::SkymapCoordinate skymapCoordinates (get_SkymapCoordinate(blocksize));
    IPosition shape (skymapCoordinates.shape());
    // create Skymapper object
    std::cout << "-- creating Skymapper object..." << std::endl;
    string filename ("skymap03.img");
    Skymapper skymapper (skymapCoordinates);
    std::cout << "-- updating Skymapper parameters..." << std::endl;
    skymapper.setAntennaPositions(get_antennaPositions(nofAntennas));
    skymapper.setFilename (filename);
    skymapper.setVerboseLevel (1);
    // if initialization goes well, start processing a block of data data
    std::cout << "-- initializing Skymapper for processing..." << std::endl;
    if (skymapper.initSkymapper()) {
      for (uint datablock(0); datablock<nofDataBlocks; datablock++) {
	Matrix<DComplex> data (get_data(nofAntennas,
					shape(4)));
	std::cout << "-- processing datablock " << datablock << " ..." << std::endl;
	status = skymapper.processData (data);
      }
    }
  } catch (std::string message) {
    cerr << message << endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}

// -----------------------------------------------------------------------------

int main (int argc,
	  char *argv[])
{
  int nofFailedTests = 0;
  bool have_dataset  = true;
  uint blocksize     = 1024;

  /*
    Check if filename of the dataset is provided on the command line; if only
    a fraction of the possible tests can be carried out.
  */
  if (argc < 2) {
    std::cerr << "[tSkymapper]"
	      << " No input dataset provided - skipping related tests."
	      << endl;
    have_dataset = false;
  }

  std::string filename = argv[1];
  
//   nofFailedTests += test_CoordinateSystem ();
  // Test feeding SkymapCoordinate information into the Beamformer
  nofFailedTests += test_Beamformer (blocksize);
  // Test the various constructors for a Skymapper object
  nofFailedTests += test_Skymapper ();
  
//   if (have_dataset) {
//     nofFailedTests += test_processing (filename,
// 				       blocksize);
//     nofFailedTests += cleanup_directory ();
//   }
  
  return nofFailedTests;
}
