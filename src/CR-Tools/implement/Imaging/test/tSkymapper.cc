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

#include <fstream>

#include <casa/aips.h>
#include <casa/Arrays/ArrayIO.h>
#include <casa/Arrays/Vector.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Exceptions/Error.h>
#include <casa/System/Aipsrc.h>
#include <casa/System/ProgressMeter.h>
#include <casa/OS/Directory.h>
#include <coordinates/Coordinates/ObsInfo.h>
#include <images/Images/PagedImage.h>
#include <images/Images/ImageFITSConverter.h>
#ifdef HAVE_HDF5
#include <images/Images/HDF5Image.h>
#endif

using casa::AipsError;
using casa::Array;
using casa::IPosition;
using casa::ObsInfo;
using casa::PagedImage;
using casa::TiledShape;

#include <Coordinates/SkymapCoordinate.h>
#include <Data/LopesEvent.h>
#include <Imaging/Beamformer.h>
#include <Imaging/Skymapper.h>
#include <Utilities/ProgressBar.h>
#include <Utilities/TestsCommon.h>

using CR::Beamformer;
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

*/

//_______________________________________________________________________________
//                                                              cleanup_directory

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

//_______________________________________________________________________________
//                                                                     getObsInfo

/*!
  \brief Get a ObsInfo object to attach to coordinates

  \param telescope -- Name of the telescope
  \param observer  -- Name of the observer

  \return info -- A casa::ObsInfo object, containing additional information on an
          observation.
*/
casa::ObsInfo getObsInfo (std::string const &telescope="LOFAR",
			  std::string const &observer="Lars Baehren")
{
  casa::ObsInfo info;
  info.setTelescope (telescope);
  info.setObserver (observer);
  return info;
}

//_______________________________________________________________________________
//                                                            test_ImageInterface

/*!
  \brief Test working with implementations of the casa::ImageInterface class

  \param blocksize       -- Number of samples per block of data.

  \return nofFailedTests --  The number of failed tests encountered within this
          fucntion.
*/
int test_ImageInterface (uint const &blocksize=1024)
{
  cout << "\n[tSkymapper::test_ImageInterface]\n" << endl;
  
  int nofFailedTests (0);
  // Time-frequency domain data
  TimeFreqCoordinate timeFreq (blocksize,80e06,2);
  // Coordinates 
  SkymapCoordinate coord;
  coord.setTimeFreqCoordinate(timeFreq);
  coord.setObsInfo(getObsInfo());

  casa::CoordinateSystem csys (coord.coordinateSystem());
  casa::TiledShape tile (coord.shape());
  
  cout << "[1] Testing constructor for ImageInterface<T> ..." << endl;
  try {
    cout << "-- creating PagedImage<float> ..." << endl;
    casa::PagedImage<float> img_paged_float (tile,csys,"skymap_paged_float.img");
    cout << "-- creating PagedImage<double> ..." << endl;
    casa::PagedImage<double> img_paged_double (tile,csys,"skymap_paged_double.img");
#ifdef HAVE_HDF5
    cout << "-- creating HDF5Image<float> ..." << endl;
    casa::HDF5Image<float> img_hdf5_float (tile,csys,"skymap_hdf5_float.h5");
    cout << "-- creating HDF5Image<double> ..." << endl;
    casa::HDF5Image<double> img_hdf5_double (tile,csys,"skymap_hdf5_double.h5");
#endif
  } catch (AipsError x) {
    cerr << x.getMesg() << endl;
    nofFailedTests++;
  }
  
  cout << "[2] Test attachement to pointer ..." << endl;
  try {
    cout << "-- Create pointer ..." << endl;
    casa::ImageInterface<float> *image;
    //
    cout << "-- Create new object attaching to pointer ..." << endl;
    image = new casa::PagedImage<float> (tile,
					 csys,
					 "skymap_paged_float.img");
    //
    cout << "-- release assigned pointer ..." << endl;
    delete image;
  } catch (AipsError x) {
    cerr << x.getMesg() << endl;
    nofFailedTests++;
  }

  return nofFailedTests;
}

//_______________________________________________________________________________
//                                                                test_Beamformer

/*!
  \brief Test setting up the Beamformer using a SkymapCoordinate object

  \return nofFailedTests -- The number of failed tests.

  Since the Skymapper class uses a Beamformer object as private data, all
  interaction with the latter is taking place indirectly. The basic method by
  which the Beamformer object is set up and configured is through extracting the
  required data from the also embedded SkymapCoordinate object.
*/
int test_Beamformer (uint const &blocksize=1024)
{
  cout << "\n[tSkymapper::test_Beamformer]\n" << endl << std::flush;

  int nofFailedTests (0);
  SkymapCoordinate coord;
  Beamformer bf;

  /* Initial state of the objects to be embedded into the Skymapper */
  coord.summary();
  bf.summary();

  cout << "[1] Add observation data to the coordinate ..." << endl << std::flush;
  try {
    coord.setObsInfo (getObsInfo());
  } catch (AipsError x) {
    cerr << x.getMesg() << endl;
    nofFailedTests++;
  }

  cout << "[2] Set the time-frequency coordinate ..." << endl << std::flush;
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

  cout << "[3] Set the spatial coordinate ..." << endl << std::flush;
  try {
    IPosition shape (3,10,10,5);
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

  cout << "[4] Set positions and frequencies ..." << endl << std::flush;
  try {
    Vector<MVPosition> antPos (3);
    Matrix<double> skyPos = coord.spatialCoordinate().positionValues();
    Vector<double> freq   = coord.timeFreqCoordinate().frequencyValues();
    //
    antPos(0) = casa::MVPosition (100,0,0);
    antPos(1) = casa::MVPosition (0,100,0);
    antPos(2) = casa::MVPosition (100,100,0);
    // 
    bf.setAntPositions(antPos);
    bf.setSkyPositions(skyPos);
    bf.setFrequencies(freq);
    bf.summary();
    //
    cout << "-- geometrical delays  [0,]  = " << bf.delays().row(0)      << endl;
    cout << "-- geometrical phases  [1,,] = " << bf.phases().yzPlane(1)  << endl;
    cout << "-- geometrical weights [1,,] = " << bf.weights().yzPlane(1) << endl;
    cout << "-- Beamformer weights  [1,,] = " << bf.beamformerWeights().yzPlane(1) << endl;
  } catch (AipsError x) {
    cerr << x.getMesg() << endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}

//_______________________________________________________________________________
//                                                                 test_Skymapper

/*!
  \brief Test constructors for a new Skymapper object

  \return nofFailedTests -- The number of failed tests.
*/
int test_Skymapper (uint const &blocksize=1024,
		    double const &sampleFreq=80e06,
		    uint const &nyquistZone=2)
{
  cout << "\n[tSkymapper::test_Skymapper]\n" << endl;

  int nofFailedTests (0);

  cout << "[1] Skymapper() ..." << endl << std::flush;
  try {
    Skymapper hdf5_image ("skymap01.h5",
			  CR::DataType::HDF5);
    hdf5_image.summary();
    //
    Skymapper casa_image ("skymap01.img",
			  CR::DataType::CASA_IMAGE);
    casa_image.summary();
  } catch (AipsError x) {
    cerr << "[tSkymapper::test_Skymapper] " << x.getMesg() << endl << std::flush;
    nofFailedTests++;
  }
  
  cout << "[2] Skymapper(SkymapCoordinate) ..." << endl << std::flush;
  try {
    // Time-frequency domain data
    TimeFreqCoordinate timeFreq (blocksize,
				 sampleFreq,
				 nyquistZone);
    /* Skymapper to compute power in frequency */
    SkymapCoordinate coord1 (getObsInfo(),
			     timeFreq,
			     CR::SkymapQuantity::FREQ_POWER);
    Skymapper skymapper1 (coord1,
			 "skymap02a.h5");
    skymapper1.summary();

    /* Skymapper to compute cc-beam */
    SkymapCoordinate coord2 (getObsInfo(),
			     timeFreq,
			     CR::SkymapQuantity::TIME_CC);
    Skymapper skymapper2 (coord2,
			 "skymap02b.h5");
    skymapper2.summary();
  } catch (AipsError x) {
    cerr << "[tSkymapper::test_Skymapper] " << x.getMesg() << endl << std::flush;
    nofFailedTests++;
  }

  cout << "[3] Skymapper(SkymapCoordinate,Matrix<double>,SkymapQuantity) ..."
       <<endl << std::flush;
  try {
    // Spatial coordinates
    IPosition shape (3,20,20,10);
    SpatialCoordinate spatial (CoordinateType::DirectionRadius,
			       "AZEL"
			       "SIN");
    spatial.setShape(shape);
    // Skymap coordinate
    SkymapCoordinate coord (getObsInfo(),
			    spatial,
			    getTimeFreq(blocksize));
    // Antenna positions
    uint nofAntennas (10);
    Matrix<double> antPositions (nofAntennas,3);
    //
    cout << "-- blocksize         = " << blocksize            << endl << std::flush;
    cout << "-- 3D shape          = " << shape                << endl << std::flush;
    cout << "-- antenna positions = " << antPositions.shape() << endl << std::flush;
    //
    Skymapper skymapper (coord,
			 antPositions,
			 "skymap03.h5");
    skymapper.summary();
  } catch (AipsError x) {
    cerr << "[tSkymapper::test_Skymapper] " << x.getMesg() << endl << std::flush;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}

//_______________________________________________________________________________
//                                                                   test_methods

/*!
  \brief Test processing of the data to generate an image

  \return nofFailedTests -- The number of failed tests encountered within this
          fucntion.
*/
int test_methods (uint const &blocksize=1024)
{
  cout << "\n[tSkymapper::test_methods]\n" << endl << std::flush;
  
  int nofFailedTests (0);
  
  // Time-frequency domain data
  TimeFreqCoordinate timeFreq (blocksize,80e06,2);
  // Coordinates 
  SkymapCoordinate coord;
  coord.setTimeFreqCoordinate(timeFreq);
  coord.setObsInfo(getObsInfo());
  // Skymapper object to work with
  Skymapper skymapper (coord);

  cout << "[1] Access to internal parameters ..." << endl << std::flush;
  try {
    cout << "-- Filename of image     = " << skymapper.filename()   << endl;
    cout << "-- Image shape           = " << skymapper.imageShape() << endl;
    cout << "-- nof. processed blocks = " << skymapper.nofProcessedBlocks() << endl;
  } catch (AipsError x) {
    cerr << "[tSkymapper::test_methods] " << x.getMesg() << endl;
    nofFailedTests++;
  }

  cout << "[2] Access to Beamformer parameters ..." << endl << std::flush;
  try{
    cout << "-- setting antenna positions ..." << endl;
    Vector<MVPosition> antPos (48);
    skymapper.setAntPositions (antPos);
    //
    cout << "-- retrieving the embedded Beamformer object ..." << endl;
    CR::Beamformer bf = skymapper.beamformer();
    cout << "-- shape of the weights = " << bf.shapeWeights() << endl;
    cout << "-- shape of the beam    = " << bf.shapeBeam()    << endl;
  } catch (AipsError x) {
    cerr << "[tSkymapper::test_methods] " << x.getMesg() << endl;
    nofFailedTests++;
  }

  cout << "[3] Test access to coordinates ..." << endl;
  try {
    CR::SkymapCoordinate coord  = skymapper.skymapCoordinate();
    casa::CoordinateSystem csys = coord.coordinateSystem();
    //
    coord.summary();
    //
    cout << "[CoordinateSystem]" << endl;
    cout << "-- nof. coordinates = " << csys.nCoordinates()   << endl;
    cout << "-- nof. pixel axes  = " << csys.nPixelAxes()     << endl;
    cout << "-- nof. world axes  = " << csys.nWorldAxes()     << endl;
    cout << "-- World axis names = " << csys.worldAxisNames() << endl;
    cout << "-- World axis units = " << csys.worldAxisUnits() << endl;
    cout << "-- Reference value  = " << csys.referenceValue() << endl;
    cout << "-- Reference pixel  = " << csys.referencePixel() << endl;
    cout << "-- Increment        = " << csys.increment()      << endl;
  } catch (AipsError x) {
    cerr << "[tSkymapper::test_methods] " << x.getMesg() << endl;
    nofFailedTests++;
  }

  return nofFailedTests;
}

//_______________________________________________________________________________
//                                                                test_processing

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

  std::string refcode    = "AZEL";
  std::string projection = "SIN";
  uint nofFrames         = 10;
  uint nofBlocksPerFrame = 1;
  uint nofAntennas       = 5;

  // Spatial coordinates
  IPosition shape (3,30,30,5);
  SpatialCoordinate spatial (CoordinateType::DirectionRadius,
			     refcode,
			     projection);
  spatial.setShape(shape);
  Vector<double> cdelt = spatial.increment();
  cdelt(2) = 100;
  spatial.setIncrement(cdelt);
  // Time-Frequency coordinate
  TimeFreqCoordinate timeFreq (blocksize,
			       nofBlocksPerFrame,
			       nofFrames,
			       true);
  
  // Skymap coordinate
  SkymapCoordinate coord (getObsInfo(),
			  spatial,
			  timeFreq);
  
  // Antenna positions
  Matrix<double> antPositions (nofAntennas,3);
  for (uint ant(0); ant<nofAntennas; ant++) {
    antPositions(ant,0) = ant*100;
    antPositions(ant,1) = ant*50;
    antPositions(ant,2) = ant;
  }
  // EM quantity for which the skymap is computed
  CR::SkymapQuantity skymapQuantity (SkymapQuantity::TIME_CC);

  //__________________________________________________________________
  // Process some mock-up data to test the functionality of the
  // Skymapper to properly set up its internals.
  
  cout << "[1] Test processData() function ..." << endl;
  try {
    /* Create Skymapper object to work with */
    Skymapper skymapper (coord,
			 antPositions,
			 "skymap_test1.img",
			 CR::DataType::CASA_IMAGE);
    skymapper.summary();
    /* Export the internal settings of embedded Beamformer object */
    {
      Beamformer bf = skymapper.beamformer();
      CR::export_Beamformer (bf,"skymap");
    }
    /* Prepare some test data to process */
    Matrix<casa::DComplex> data (timeFreq.fftLength(),
				 nofAntennas);
    
    for (uint block(0); block<nofBlocksPerFrame*nofFrames; block++) {
      /* Put some values into the data array passed to the method */
      for (uint freq(0); freq<timeFreq.fftLength(); freq++) {
	data.row(freq) = (block+1)*double(freq);
      }
      /* Process a block of data and write out the result */
      skymapper.processData(data);
    }
  } catch (AipsError x) {
    cerr << "[tSkymapper::test_processing] " << x.getMesg() << endl;
    nofFailedTests++;
  }

  //__________________________________________________________________
  // Read back in the previously created image file and inspect it
  
  cout << "[2] Reading back in previously created image file ..." << endl;
  try {
    bool status (true);
    casa::String error;
    casa::PagedImage<float> image ("skymap_test1.img");
    //
    cout << "-- Image name       = " << image.name()      << endl;
    cout << "-- Image shape      = " << image.shape()     << endl;
    cout << "-- Image type       = " << image.imageType() << endl;
    cout << "-- World axis names = " << image.coordinates().worldAxisNames() << endl;
    cout << "-- World axis units = " << image.coordinates().worldAxisUnits() << endl;
    // Convert the image to FITS
    status = casa::ImageFITSConverter::ImageToFITS(error, image, "!skymap_test1.fits");
    if (!status) {
      std::cout << error << std::endl;
    }
  } catch (AipsError x) {
    cerr << "[tSkymapper::test_processing] " << x.getMesg() << endl;
    nofFailedTests++;
  }
  
  //__________________________________________________________________
  // Test processing of am example LOPES data set

  cout << "[3] Process example LOPES data-set ..." << endl;
  try {
    // set up DataReader for input of data
    CR::LopesEvent dr (dataset_lopes_example,
		       blocksize);
    dr.summary();
    // update SkymapCoordinate
    coord.setObsInfo (getObsInfo("LOPES"));
    //
    timeFreq.setBlocksPerFrame(1);
    timeFreq.setNofFrames(10);
    coord.setTimeFreqCoordinate(timeFreq);
    // Create Skymapper object to work with
    Skymapper skymapper (coord,
			 antPositions,
			 "skymap_lopes.img",
			 CR::DataType::CASA_IMAGE);
    skymapper.summary();
  } catch (AipsError x) {
    cerr << "[tSkymapper::test_processing] " << x.getMesg() << endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}

//_______________________________________________________________________________
//                                                                           main

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
  
  // Test working with instantiations of casa::ImageInterface
  nofFailedTests += test_ImageInterface();
  // Test feeding SkymapCoordinate information into the Beamformer
  nofFailedTests += test_Beamformer (blocksize);
  // Test the various constructors for a Skymapper object
  nofFailedTests += test_Skymapper ();
  // Test the various methods for accessing internal data
  nofFailedTests += test_methods();
  // Test processing of data
  nofFailedTests += test_processing (infile,
				     blocksize,
				     have_dataset);
  
  return nofFailedTests;
}
