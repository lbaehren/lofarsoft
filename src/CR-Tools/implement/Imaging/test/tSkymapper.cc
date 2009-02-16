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
    std::string telescope ("UNKNOWN");
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
    Skymapper skymapper ("skymap01.img");
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
    ObservationData obsData ("UNKNOWN");
    obsData.setObserver ("Lars Baehren");
    // Coordinates 
    SkymapCoordinate coord;
    coord.setTimeFreqCoordinate(timeFreq);
    coord.setObservationData(obsData);
    
    Skymapper skymapper (coord,
			 "skymap02.img");
    
    skymapper.summary();
  } catch (AipsError x) {
    cerr << "[tSkymapper::test_Skymapper] " << x.getMesg() << endl;
    nofFailedTests++;
  }
  
  cout << "[3] Skymapper(SkymapCoordinate,Matrix<double>,SkymapQuantity) ..."
       << endl;
  try {
    // Observation data
    std::string telescope ("UNKNOWN");
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
    // EM quantity for which the skymap is computed
    CR::SkymapQuantity skymapQuantity (CR::SkymapQuantity::TIME_CC);
    //
    Skymapper skymapper (coord,
			 antPositions,
			 skymapQuantity,
			 "skymap03.img");
    skymapper.summary();
  } catch (AipsError x) {
    cerr << "[tSkymapper::test_Skymapper] " << x.getMesg() << endl;
    nofFailedTests++;
  }

  return nofFailedTests;
}

// -----------------------------------------------------------------------------

/*!
  \brief Test processing of the data to generate an image

  \return nofFailedTests -- The number of failed tests encountered within this
          fucntion.
*/
int test_methods ()
{
  cout << "\n[tSkymapper::test_methods]\n" << endl;
  
  int nofFailedTests (0);
  
  // Time-frequency domain data
  TimeFreqCoordinate timeFreq (2048,80e06,2);
  // Observation info
  ObservationData obsData;
  obsData.setObserver ("Lars Baehren");
  // Coordinates 
  SkymapCoordinate coord;
  coord.setTimeFreqCoordinate(timeFreq);
  coord.setObservationData(obsData);
  // Skymapper object to work with
  Skymapper skymapper (coord);

  cout << "[1] Access to internal parameters ..." << endl;
  try {
    cout << "-- Filename of image     = " << skymapper.filename()   << endl;
    cout << "-- Image shape           = " << skymapper.imageShape() << endl;
    cout << "-- nof. processed blocks = " << skymapper.nofProcessedBlocks() << endl;
  } catch (AipsError x) {
    cerr << "[tSkymapper::test_methods] " << x.getMesg() << endl;
    nofFailedTests++;
  }

  cout << "[2] Access to Beamformer parameters ..." << endl;
  try{
    cout << "-- setting antenna positions ..." << endl;
    Vector<MVPosition> antPos (48);
    skymapper.setAntPositions (antPos);
    skymapper.summary();
  } catch (AipsError x) {
    cerr << "[tSkymapper::test_methods] " << x.getMesg() << endl;
    nofFailedTests++;
  }

  return nofFailedTests;
}

// -----------------------------------------------------------------------------

/*!
  \brief Test processing of the data to generate an image

  \param infile       -- LOPES data set to use as data input
  \param blocksize    -- Number of samples per block of data.
  \param have_dataset -- Do we have a dataset available to work with? If this is
         not the case the best we can do is to either use simulated data of blank
	 input arrays.

  \return nofFailedTests --  The number of failed tests encountered within this
          fucntion.
*/
int test_processing (string const &infile,
		     uint const &blocksize=1024,
		     bool have_dataset=false)
{
  cout << "\n[tSkymapper::test_processing]\n" << endl;
  
  int nofFailedTests     = 0;

  std::string telescope  = "UNKNOWN";
  std::string observer   = "Lars Baehren";
  std::string refcode    = "AZEL";
  std::string projection = "SIN";
  uint nofFrames         = 10;
  uint nofBlocksPerFrame = 2;
  uint nofAntennas       = 10;

  // Observation data
  CR::ObservationData obsData (telescope,
			       observer);
  // Spatial coordinates
  IPosition shape (3,20,20,10);
  SpatialCoordinate spatial (CoordinateType::DirectionRadius,
			     refcode,
			     projection);
  spatial.setShape(shape);
  // Time-Frequency coordinate
  TimeFreqCoordinate timeFreq (blocksize,
			       nofBlocksPerFrame,
			       nofFrames,
			       true);
  
  // Skymap coordinate
  SkymapCoordinate coord (obsData,
			  spatial,
			  timeFreq);
  
  // Antenna positions
  Matrix<double> antPositions (nofAntennas,3);
  // EM quantity for which the skymap is computed
  CR::SkymapQuantity skymapQuantity (CR::SkymapQuantity::TIME_CC);
//   CR::SkymapQuantity skymapQuantity;

  //________________________________________________________
  // Display the chosen control parameters

  cout << "-- Telescope name        = " << telescope         << endl;
  cout << "-- Observer name         = " << observer          << endl;
  cout << "-- Reference code        = " << refcode           << endl;
  cout << "-- Map projection        = " << projection        << endl;
  cout << "-- Blocksize             = " << timeFreq.blocksize() << endl;
  cout << "-- FFT length            = " << timeFreq.fftLength() << endl;
  cout << "-- nof. blocks per frame = " << nofBlocksPerFrame << endl;
  cout << "-- nof. frames           = " << nofFrames         << endl;
  cout << "-- nof. antennas         = " << nofAntennas       << endl;

  //________________________________________________________
  // Run the processing tests
  
  cout << "[1] Process a single block of data ..." << endl;
  try {
    Skymapper skymapper (coord,
			 antPositions,
			 skymapQuantity,
			 "skymap_test1.img");

    Matrix<casa::DComplex> data (timeFreq.fftLength(),
				 nofAntennas);
    data = 1.0;

    cout << "-- Output filename     = " << skymapper.filename() << endl;
    cout << "-- shape(antPositions) = " << antPositions.shape() << endl;
    cout << "-- shape(data)         = " << data.shape()         << endl;

    skymapper.processData(data);

  } catch (AipsError x) {
    cerr << "[tSkymapper::test_processing] " << x.getMesg() << endl;
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
  std::string infile;

  /*
    Check if filename of the dataset is provided on the command line; if only
    a fraction of the possible tests can be carried out.
  */
  if (argc < 2) {
    std::cerr << "[tSkymapper]"
	      << " No input dataset provided - skipping related tests."
	      << endl;
    have_dataset = false;
  } else {
    infile = argv[1];
  }
  
  // Test feeding SkymapCoordinate information into the Beamformer
  nofFailedTests += test_Beamformer (blocksize);
  // Test the various constructors for a Skymapper object
  nofFailedTests += test_Skymapper ();
  // Test the various methods for accessing internal data
  nofFailedTests += test_methods();

  nofFailedTests += test_processing (infile,
				     blocksize,
				     have_dataset);
  
  return nofFailedTests;
}
