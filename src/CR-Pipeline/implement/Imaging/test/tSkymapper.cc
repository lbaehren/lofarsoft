/***************************************************************************
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
#include <images/Images/PagedImage.h>

using casa::IPosition;

#include <Imaging/Skymapper.h>
#include <Utilities/ProgressBar.h>
#include <create_data.h>

using CR::ObservationData;
using CR::ProgressBar;
using CR::SkymapCoordinates;
using CR::Skymapper;
using CR::TimeFreq;

/*!
  \file tSkymapper.cc

  \ingroup Imaging

  \brief A collection of test routines for Skymapper
 
  \author Lars B&auml;hren
 
  \date 2006/01/23
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
void show_PagedImage (const PagedImage<T>& myimage)
{
  IPosition shape (myimage.shape());
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

template void show_PagedImage (const PagedImage<Float>& myimage);
template void show_PagedImage (const PagedImage<Double>& myimage);

// -----------------------------------------------------------------------------

/*!
  \brief Read in the antenna positions from an ASCII table on disk

  \return antennaPositions -- Matrix with the 3-dim antenna positions
*/
Matrix<Double> readAntennaPositions ()
{
  Matrix<Double> pos;
  readAsciiMatrix (pos, "positionsLOPES08.data");

  return pos;
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
  cout << "\n[test_CoordinateSystem]" << endl;

  Int nofFailedTests (0);

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
  \brief Test working with AIPS++ paged images

  Since the main storage container for a skymap will be an AIPS++ image, we 
  should become familiar with the ways to actually use it without the Glish 
  interface. -- In fact this already provides a first the for the Skymapper
  class, as internally a default Skymapper object is created.

  Once the test image is created on disk, the following Glish code snippet
  can be used to load, inspect and display it:
  \code
  include 'image.g';
  img := image ("testimage-02.img");
  img.summary();
  img.statistics();
  if (is_fail(img.view())) {
    print "Unable to display image";
  } else {
    print "Everything done."
  }
  \endcode

  The summary of for the created image tool will look like this:
  \verbatim
  Image name       : tSkymapper.img
  Image type       : PagedImage
  Image quantity   : Intensity
  Pixel mask(s)    : None
  Region(s)        : None
  NORMAL: 
  Direction reference : AZEL
  Spectral  reference : TOPO
  Velocity  type      : RADIO
  Telescope           : WSRT
  Observer            : LOFAR Cosmic Rays KSP
  Date observation    : 2006/03/21/19:11:42
  
  Axis Coord Type      Name       Proj Shape Tile Coord value at pixel    Coord incr Units
  ---------------------------------------------------------------------------------------- 
  1    1     Direction Longitude   STG   120  120       0.000    61.00 -7.200000e+03 arcsec
  2    1     Direction Latitude    STG   120  120      90.000    61.00  7.200000e+03 arcsec
  3    2     Linear    Distance            1    1           0     1.00  1.000000e+00 
  4    3     Linear    Time                1    1           0     1.00  1.000000e+00 
  5    4     Spectral  Frequency           1    1           0     1.00  1.000000e+00 Hz
  \endverbatim

*/
int test_PagedImage ()
{
  cout << "\n[test_PagedImage]\n" << endl;

  Int nofFailedTests (0);
  Skymapper skymapper;
  String pathTestImage (Aipsrc::aipsRoot());

  pathTestImage += "/data/demo/Images/test_image";

  /*!
    [1] The simplest test of them all: just try to open up one of the images 
    provided as part of the AIPS++ data repository
   */

  cout << " [1] Opening AIPS++ test image from disk ... " << endl;
  // 1.a direct creation
  try {
    // Open the image (as readonly for the moment).
    PagedImage<Float> myimage (pathTestImage);
    // show image characteristics
    show_PagedImage (myimage);
  } catch (AipsError x) {
    cerr << "[tSkymapper::test_PagedImage] " << x.getMesg() << endl;
    nofFailedTests++;
  }
  // 1.b creation using pointer
  try {
    cout << " - creating pointer" << endl;
    PagedImage<Float> *myimage;
    cout << " - creating the new object" << endl;
    myimage = new PagedImage<Float> (pathTestImage);
    // show image characteristics
    show_PagedImage (*myimage);
  } catch (AipsError x) {
    cerr << "[tSkymapper::test_PagedImage] " << x.getMesg() << endl;
    nofFailedTests++;
  }

  /*!
    [2] Create a new paged AIPS++ image from scratch. Here we used the default
    shape and coordinate system as provided by the Skymapper class.
  */
  
  cout << " [2] Creating new AIPS++ image as PagedImage ... " << endl;
  try {
    TiledShape shape (skymapper.imageShape());
    //
    PagedImage<Float> image (shape,
			     skymapper.coordinateSystem(),
			     String("tPagedImage01.img"));
    // show image characteristics
    show_PagedImage (image);    
  } catch (AipsError x) {
    cerr << "[tSkymapper::test_PagedImage] " << x.getMesg() << endl;
    nofFailedTests++;
  }

  /*!
    [3] Create a new paged AIPS++ image from scratch. The empty pixel array of the
    image is subsequently filled via <tt>image.putSlice (pixels,start,stride)</tt>,
    stepping through the third image axis (the distance axis for the standard 
    image tools generated by the Skymapper).
  */
  
  cout << " [3] Insert pixel values from matrix-shaped slice ... " << endl;
  try {
    IPosition shape (5,120,120,20,10,10);
    TiledShape tile (shape);

    cout << " --> creating the image of shape " << shape <<  " ..." << endl;
    PagedImage<Float> image (tile,
			     skymapper.coordinateSystem(),
			     String("tPagedImage02.img"));
    // ... and display some basic properties
    show_PagedImage (image); 
    
    IPosition start (5,0,0,0,0,0);
    IPosition stride (5,1,1,1,1,1);
    Array<Float> pixels (IPosition(2,120,120));

    cout << " --> filling the image by slices along the distance axis ..." << endl;

    for (int radius(0); radius<shape(2); radius++) {
      start(2) = radius;
      pixels = Float(radius);
      //
      image.putSlice (pixels,start,stride);
    }

  } catch (AipsError x) {
    cerr << "[tSkymapper::test_PagedImage] " << x.getMesg() << endl;
    nofFailedTests++;
  }

  /*
    [4] While the tests above of some general nature, the following code is 
    intended as test case for the operation to be performed inside the Skymapper
    code, where we require to insert the computed pixel values into the pixel
    array of the existing image.
  */
  
  cout << " [4] Insert pixels values from spatial sub-volumes ... " << endl;
  try {
    int nofAxes (5);
    int nofLon (120);
    int nofLat (120);
    int nofDist (20);
    int nofTimesteps (10);
    int nofFrequencies (128);
    IPosition shape (nofAxes,nofLon,nofLat,nofDist,nofTimesteps,nofFrequencies);
    TiledShape tile (shape);

    cout << " --> creating the image of shape " << shape <<  " ..." << endl;
    PagedImage<float> image (tile,
			     skymapper.coordinateSystem(),
			     String("tPagedImage03.img"));
    // ... and display some basic properties
    show_PagedImage (image); 
    
    /*
      Array with the results of the beamforming; the various pointing positions
      are not decomposed into (lon,lat,dist), so mapping back onto these three 
      coordinates has to be done later.
    */
    int nofSkyPositions (nofLon*nofLat*nofDist);
    IPosition shapeBeam (2,nofSkyPositions,nofFrequencies);
    Array<float> beam (shapeBeam);
    
    /*
      Temporary array, into which we copy the data before inserting them into
      the pixel array.
    */
    Array<float> tmp (IPosition(5,nofLon,nofLat,nofDist,1,1));

    /*
      Insert the data from the beam array to the pixel array; we use the number
      of timestep as outer loop, thereby enulating the subsequent processing 
      of data blocks.
    */
    
    IPosition stride (nofAxes,1);
    
    cout << " --> start writing the data into the pixel array of the image ..."
	 << endl;

    // create meter to show progress on the operation
    int counter (0);
    casa::ProgressMeter meter (0,
			       nofTimesteps*nofFrequencies,
			       "Filling data",
			       "Processing block",
			       "","",true,1);

    for (shape(3)=0; shape(3)<nofTimesteps; shape(3)++) {
      /*
	Below this point is what needs to be done per block of incoming data
      */
      for (shape(4)=0; shape(4)<nofFrequencies; shape(4)++) {
	// IPositions
	IPosition start (nofAxes,0,0,0,shape(3),shape(4));
	// Slicer (start,end,stride,Slicer::endIsLength)
// 	Slicer slice (start,
// 		      IPosition(nofAxes,nofLon,nofLat,nofDist,1,1),
// 		      stride,
// 		      Slicer::endIsLength);
	// assign a new value to the temp array
	tmp = shape(3)*10+shape(4);
	// insert the temporary data into the pixel array
 	image.putSlice (tmp,start,stride);
	// show progress
	meter.update(counter);
	counter++;
      }
    }
    
  } catch (AipsError x) {
    cerr << "[tSkymapper::test_PagedImage] " << x.getMesg() << endl;
    nofFailedTests++;
  }

  /*
    [5] In case everything else fails, the last resort is to insert the values
    pixel by pixel. While this is the securest method in terms addressing the
    arrays, the interaction with the PagedImage is least efficient.
  */


//   cout << " [5] Insert one pixel value at a time ... " << endl;
//   try {
//     int nofAxes (5);
//     int nofChannels (128);
//     IPosition shape (nofAxes,40,40,20,10,nofChannels);
//     TiledShape tile (shape);

//     cout << " --> creating the image of shape " << shape <<  " ..." << endl;
//     PagedImage<float> image (tile,
// 			     skymapper.coordinateSystem(),
// 			     String("tPagedImage04.img"));
//     // ... and display some basic properties
//     show_PagedImage (image); 

//     int coord (0);
//     int nofPixels (shape(0)*shape(1)*shape(2));
//     Matrix<float> pixels (nofPixels,nofChannels);
//     IPosition start (5);
//     ProgressBar progress (shape(0));

//     for (start(3)=0; start(3)<shape(3); start(3)++) {
      
//       pixels = start(3);
//       coord  = 0;
      
//       cout << " -- filling in pixel values for block " << start(3) << " ..."
// 	   << endl;

//       for (start(0)=0; start(0)<shape(0); start(0)++) {        // Longitude
// 	for (start(1)=0; start(1)<shape(1); start(1)++) {      // Latitude
// 	  for (start(2)=0; start(2)<shape(2); start(2)++) {    // Radius
// 	    for (start(4)=0; start(4)<shape(4); start(4)++) {  // Frequency
// 	      image.putAt (pixels(coord,start(4)),start);
// 	    }
// 	    coord++;
// 	  }
// 	}
// 	progress.update(start(0)+1);
//       }
//     }
    
//   } catch (AipsError x) {
//     cerr << "[tSkymapper::test_PagedImage] " << x.getMesg() << endl;
//     nofFailedTests++;
//   }

  return nofFailedTests;
}

// -----------------------------------------------------------------------------

/*!
  \brief Test constructors for a new Skymapper object

  \return nofFailedTests -- The number of failed tests.
*/
Int test_Skymapper ()
{
  cout << "\n[test_Skymapper]" << endl;

  Int nofFailedTests (0);

  cout << "\n[1] Testing default constructor ..." << endl;
  try {
    Skymapper skymapper;
    //
    skymapper.summary();
  } catch (AipsError x) {
    cerr << "[tSkymapper::test_Skymapper] " << x.getMesg() << endl;
    nofFailedTests++;
  }
  
  cout << "\n[2] Testing argumented constructor ..." << endl;
  try {
    // Time-frequency domain data
    TimeFreq timeFreq (2048,80e06,2);
    // Observation info
    ObservationData obsData ("LOFAR-ITS");
    obsData.setObserver ("Lars Baehren");
    // Coordinates 
    SkymapCoordinates coords (timeFreq,
			      obsData);
    
    Skymapper skymapper (coords);
    
    skymapper.summary();
  } catch (AipsError x) {
    cerr << "[tSkymapper::test_Skymapper] " << x.getMesg() << endl;
    nofFailedTests++;
  }
  
  cout << "\n[3] Logging of internal parameters ..." << endl;
  try {
    ofstream logfile;
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

  \param lopesData -- LOPES data set to use as data input
  \param blocksize -- Number of samples per block of data.

  \return nofFailedTests -- The number of failed tests.
*/
int test_processing (string const &lopesData,
		     uint const &blocksize)
{
  cout << "\n[test_processing]\n" << endl;
  
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
    Skymapper skymapper (get_SkymapCoordinates());
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
    CR::SkymapCoordinates skymapCoordinates (get_SkymapCoordinates(blocksize));
    skymapCoordinates.setNofBlocks (nofDataBlocks);
    IPosition shape (skymapCoordinates.shape());
    // create Skymapper object
    std::cout << "-- creating Skymapper object..." << std::endl;
    string filename ("skymap03.img");
    Skymapper skymapper (skymapCoordinates);
    std::cout << "-- updating Skymapper parameters..." << std::endl;
    skymapper.setBeamformer(get_antennaPositions(nofAntennas));
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
  Int nofFailedTests (0);
  string metafile ("/data/ITS/exampledata.its/experiment.meta");
  string lopesData ("/data/LOPES/example.event");
  uint blocksize (1024);
  
//     nofFailedTests += test_CoordinateSystem ();
  nofFailedTests += test_PagedImage ();
//   nofFailedTests += test_Skymapper ();
//   nofFailedTests += test_processing (lopesData,
// 				     blocksize);

  return nofFailedTests;
}
