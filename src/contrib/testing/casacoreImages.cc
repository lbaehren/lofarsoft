/*-------------------------------------------------------------------------*
 | $Id:: tSkymapper.cc 2252 2009-01-22 16:34:17Z baehren                 $ |
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

#include <iostream>

#include <casa/aips.h>
#include <casa/Arrays/ArrayIO.h>
#include <casa/Arrays/Vector.h>
#include <casa/Arrays/Matrix.h>
#include <casa/BasicSL/String.h>
#include <casa/Exceptions/Error.h>
#include <casa/System/Aipsrc.h>
#include <casa/System/ProgressMeter.h>
#include <casa/OS/Directory.h>
#include <coordinates/Coordinates/DirectionCoordinate.h>
#include <coordinates/Coordinates/LinearCoordinate.h>
#include <coordinates/Coordinates/SpectralCoordinate.h>
#include <coordinates/Coordinates/TabularCoordinate.h>
#include <images/Images/PagedImage.h>
#ifdef HAVE_HDF5
#include <images/Images/HDF5Image.h>
#endif

using std::cerr;
using std::cout;
using std::endl;
using std::string;
using std::vector;

using namespace casa;

/*!
  \file casacoreImages.cc
  
  \ingroup contrib
  
  \brief A number of tests and exercises for working with casacore images
  
  \author Lars B&auml;hren
*/

//_______________________________________________________________________________
//                                                                        summary

void summary (casa::CoordinateSystem &csys)
{
  cout << "-- World axis names = " << csys.worldAxisNames() << endl;
  cout << "-- World axis units = " << csys.worldAxisUnits() << endl;
  cout << "-- Reference pixel  = " << csys.referencePixel() << endl;
  cout << "-- Reference value  = " << csys.referenceValue() << endl;
  cout << "-- Increment        = " << csys.increment()      << endl;
}

//_______________________________________________________________________________
//                                                                        summary

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
1  -- has pixel mask : 0
  \endverbatim
*/
template <class T>
void summary (const casa::ImageInterface<T>& myimage)
{
  casa::IPosition shape (myimage.shape());
  int nofAxes (shape.nelements());
  double nofPixes (1.0);
  bool stripPath (true);

  for (int n(0); n<nofAxes; n++) {
    nofPixes *= shape(n);
  }

  cout << " -- Name             : " << myimage.name(stripPath) << endl;
  cout << " -- Image shape      : " << shape                   << endl;
  cout << " -- Number of pixels : " << nofPixes                << endl;
  cout << " -- is persistent    : " << myimage.isPersistent()  << endl;
  cout << " -- is paged         : " << myimage.isPaged()       << endl;
  cout << " -- is writable      : " << myimage.isWritable()    << endl;
  cout << " -- has pixel mask   : " << myimage.hasPixelMask()  << endl;
}

template void summary (const casa::ImageInterface<casa::Float>& myimage);
template void summary (const casa::ImageInterface<casa::Double>& myimage);
template void summary (const casa::ImageInterface<casa::Complex>& myimage);
template void summary (const casa::ImageInterface<casa::DComplex>& myimage);

//_______________________________________________________________________________
//                                                               coordinateSystem

/*!
  \brief Create coordinate system object to be attached to an image

  \return cs -- Coordinate system object
*/
casa::CoordinateSystem coordinateSystem (std::string const telescope="LOFAR",
					 std::string const observer="Lars Baehren",
					 uint blocksize=1024,
					 uint sampleFrequency=200e06)
{
  CoordinateSystem cs;
  
  /* Observation info */
  {
    ObsInfo obsInfo;
    obsInfo.setTelescope(telescope);
    obsInfo.setObserver(observer);
    
    cs.setObsInfo(obsInfo);
  }
  
  /* Direction coordinate (Long,Lat) */
  {
    Vector<Quantum<double> > refValue (2);
    Vector<Quantum<double> > increment (2);
    Vector<double> refPixel (2);
    Matrix<Double> xform(2,2);

    refValue(0)      = Quantum<double> (0,"deg");
    refValue(1)      = Quantum<double> (90,"deg");
    increment(0)     = Quantum<double> (-1,"deg");
    increment(1)     = Quantum<double> (1,"deg");
    refPixel(0)      = 0;
    refPixel(1)      = 0;
    xform            = 0.0;
    xform.diagonal() = 1.0;

    DirectionCoordinate coord (MDirection::AZEL,
			       Projection(Projection::STG),
			       refValue(0),
			       refValue(1),
			       increment(0),
			       increment(1),
			       xform,
			       refPixel(0),
			       refPixel(1));

    cs.addCoordinate (coord);
  }

  /* Linear coordinate (Radius) */
  {
    Vector<casa::String> names (1,"Radius");
    Vector<casa::String> units (1,"m");
    Vector<casa::Double> crpix(1,0.0);
    Vector<casa::Double> crval(1,0.0);
    Vector<casa::Double> cdelt(1,1.0);
    Matrix<casa::Double> pc(1,1,0.0);

    pc.diagonal(1);

    LinearCoordinate coord (names,
			    units,
			    crval,
			    cdelt,
			    pc,
			    crpix);
    
    cs.addCoordinate (coord);
  }

  /* Spectral axis */
  {
    double refPixel (0);
    double refValue (80e6);
    double increment (1e06);
    Vector<String> names (1,"Frequency");
    
    casa::SpectralCoordinate coord (casa::MFrequency::TOPO,
                                    refValue,
                                    increment,
                                    refPixel);
    coord.setWorldAxisNames(names);

    cs.addCoordinate (coord);
  }

  /* Time axis */
  {
    Vector<casa::String> names (1,"Time");
    Vector<casa::String> units (1,"s");
    Vector<casa::Double> crpix(1,0.0);
    Vector<casa::Double> crval(1,0.0);
    Vector<casa::Double> cdelt(1,1.0);
    Matrix<casa::Double> pc(1,1,0.0);

    pc.diagonal(1);

    LinearCoordinate coord (names,
			    units,
			    crval,
			    cdelt,
			    pc,
			    crpix);
    
    cs.addCoordinate (coord);
  }

  return cs;
}

//_______________________________________________________________________________
//                                                                   test_ObsInfo

/*!
  \brief Test working with a casa::ObsInfo object
  
  This class is used to record miscellaneous information about an observation.
  At present it contains the following:

  <ol>
    <li>Telescope name
    <li>Observer name
    <li>Observation date
    <li>Pointing centre (as distinct from the phase center or tangent point)
  </ol>

  \return nofFailedTests -- The number of failed tests encountered within this
          function.
*/
int test_ObsInfo ()
{
  cout << "\n[test_ObsInfo]" << endl;

  int nofFailedTests (0);

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
int test_PagedImage (vector<string> &images,
		     bool const &test_putAt)
{
  cout << "\n[test_PagedImage]\n" << endl;

  int nofFailedTests (0);
  casa::String pathTestImage (casa::Aipsrc::aipsRoot());

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
    summary (myimage);
  } catch (AipsError x) {
    cerr << "[casacoreImages::test_PagedImage] " << x.getMesg() << endl;
    nofFailedTests++;
  }
  // 1.b creation using pointer
  try {
    cout << " - creating pointer" << endl;
    PagedImage<Float> *myimage;
    cout << " - creating the new object" << endl;
    myimage = new PagedImage<Float> (pathTestImage);
    // show image characteristics
    summary (*myimage);
  } catch (AipsError x) {
    cerr << "[casacoreImages::test_PagedImage] " << x.getMesg() << endl;
    nofFailedTests++;
  }

  /*!
    [2] Create a new paged AIPS++ image from scratch. The empty pixel array of the
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
			     coordinateSystem(),
			     String("tPagedImage02.img"));
    // ... and display some basic properties
    summary (image); 
    
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
    cerr << "[casacoreImages::test_PagedImage] " << x.getMesg() << endl;
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
			     coordinateSystem(),
			     String("tPagedImage03.img"));
    // ... and display some basic properties
    summary (image); 
    
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
    cerr << "[casacoreImages::test_PagedImage] " << x.getMesg() << endl;
    nofFailedTests++;
  }

  /*
    [5] In case everything else fails, the last resort is to insert the values
    pixel by pixel. While this is the securest method in terms addressing the
    arrays, the interaction with the PagedImage is least efficient. For that
    matter this test needs to be activated explicitely.
  */

  if (test_putAt) {
    cout << " [5] Insert one pixel value at a time ... " << endl;
    try {
      int nofAxes (5);
      int nofChannels (128);
      IPosition shape (nofAxes,40,40,20,10,nofChannels);
      TiledShape tile (shape);
      
      cout << " --> creating the image of shape " << shape <<  " ..." << endl;
      PagedImage<float> image (tile,
			       coordinateSystem(),
			       String("tPagedImage04.img"));
      // ... and display some basic properties
      summary (image); 
      
      int coord (0);
      int nofPixels (shape(0)*shape(1)*shape(2));
      Matrix<float> pixels (nofPixels,nofChannels);
      IPosition start (5);
      
      for (start(3)=0; start(3)<shape(3); start(3)++) {
	
	pixels = start(3);
	coord  = 0;
	
	cout << " -- filling in pixel values for block " << start(3) << " ..."
	     << endl;
	
	for (start(0)=0; start(0)<shape(0); start(0)++) {        // Longitude
	  for (start(1)=0; start(1)<shape(1); start(1)++) {      // Latitude
	    for (start(2)=0; start(2)<shape(2); start(2)++) {    // Radius
	      for (start(4)=0; start(4)<shape(4); start(4)++) {  // Frequency
		image.putAt (pixels(coord,start(4)),start);
	      }
	      coord++;
	    }
	  }
	}
      }

    } catch (AipsError x) {
      cerr << "[casacoreImages::test_PagedImage] " << x.getMesg() << endl;
      nofFailedTests++;
    }
  }
  
  return nofFailedTests;
}

//_______________________________________________________________________________
//                                                               test_lofarImages

/*!
  \brief Test creating examples of LOFAR images

  Besides creating examples of potential types of LOFAR images -- standard survey
  image, Rotation-Measure synthesis image, Cosmic Rays near-field image, etc. --
  this function combines the various types of coordinate objects -- Direction,
  Linear, Frequency, Tabular -- into a coordinate system object which then in
  turn is attached to an image file.

  \return nofFailedTests -- The number of failed tests encountered within this
          function.
*/
int test_lofarImages ()
{
  int nofFailedTests (0);

  casa::CoordinateSystem csys;
  casa::DirectionCoordinate directionCoord;
  casa::TabularCoordinate radiusCoord;
  casa::SpectralCoordinate freqCoord;
  casa::LinearCoordinate timeCoord;
  casa::IPosition nelem (5);

  nelem(0) = 120;  /* Longitude */
  nelem(1) = 120;  /* Latitude  */
  nelem(2) = 10;   /* Radius    */
  nelem(3) = 513;  /* Frequency */
  nelem(4) = 100;  /* Time      */
  
  cout << "[1] Creating direction axes coordinate ..." << endl;
  try {
    casa::Vector<double> refPixel(2);
    Vector<Quantum<double> > refValue(2);
    Vector<Quantum<double> > increment(2);
    casa::Matrix<double> xform(2,2);
    // Reference pixel is center of map for directional components
    refPixel(0) = nelem(0)/2;
    refPixel(1) = nelem(1)/2;
    // Reference value: local zenith
    refValue(0) = Quantum<double>(0,"deg");
    refValue(1) = Quantum<double>(90,"deg");
    // Coordinate increment
    increment(0) = Quantum<double>(-1,"deg");
    increment(1) = Quantum<double>(1,"deg");
    // Linear transform
    xform            = 0.0;
    xform.diagonal() = 1.0;
    // DirectionCoordinate
    directionCoord = casa::DirectionCoordinate (casa::MDirection::AZEL,
						casa::Projection::STG,
						refValue(0),
						refValue(1),
						increment(0),
						increment(1),
						xform,
						refPixel(0),
						refPixel(1));
  }
  catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

  cout << "[2] Creating (tabulated) distance axis coordinate ..." << endl;
  try {
    Vector<double> pixel (nelem(2));
    Vector<double> world (nelem(2));
    casa::String unit ("m");
    casa::String name ("Length");

    pixel(0) = 0;   world(0) = 0;
    pixel(1) = 1;   world(1) = 1;
    pixel(2) = 2;   world(2) = 2;
    pixel(3) = 3;   world(3) = 5;
    pixel(4) = 4;   world(4) = 10;
    pixel(5) = 5;   world(5) = 20;
    pixel(6) = 6;   world(6) = 50;
    pixel(7) = 7;   world(7) = 100;
    pixel(8) = 8;   world(8) = 200;
    pixel(9) = 9;   world(9) = 500;

    radiusCoord = casa::TabularCoordinate (pixel,world,unit,name);
  }
  catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

  cout << "[3] Creating (linear) time axis coordinate ..." << endl;
  {
    Vector<casa::String> names (1,"Time");
    Vector<casa::String> units (1,"s");
    Vector<casa::Double> crpix(1,0.0);
    Vector<casa::Double> crval(1,0.0);
    Vector<casa::Double> cdelt(1,1.0);
    Matrix<casa::Double> pc(1,1,0.0);

    pc.diagonal() = 1.0;

    timeCoord = casa::LinearCoordinate  (names,
					 units,
					 crval,
					 cdelt,
					 pc,
					 crpix);
  }

  cout << "[4] Creating spectral axis coordinate ..." << endl;
  {
    double refPixel (0);
    double refValue (80e6);
    double increment (1e06);
    Vector<String> names (1,"Frequency");
    
    freqCoord = casa::SpectralCoordinate (casa::MFrequency::TOPO,
					  refValue,
					  increment,
					  refPixel);
    freqCoord.setWorldAxisNames(names);
  }

  cout << "[5] Adding coordinate objects to coordinate system ..." << endl;
  try {
    /* Add coodinate objects to coordinate system */
    csys.addCoordinate(directionCoord);
    csys.addCoordinate(radiusCoord);
    csys.addCoordinate(timeCoord);
    csys.addCoordinate(freqCoord);
    /* Provide a summary of the assembled coordinate system */
    summary (csys);
  }
  catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

#ifdef HAVE_HDF5
  cout << "[6] Creating image on disk ..." << endl;
  try {
    TiledShape tile (nelem);
    HDF5Image<double> image (tile,
			     csys,
			     String("lofarImage-cr.h5"));
    summary(image);
  }
  catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
#endif

  return nofFailedTests;
}

//_______________________________________________________________________________
//                                                                         main()

int main (int argc,
	  char *argv[])
{
  int nofFailedTests = 0;
  bool putAt         = false;
  vector<string> images;

  /* Test working with casacore paged images */
//   nofFailedTests += test_PagedImage (images,
// 				     putAt);
  /* Test generating examples of LOFAR images */
  nofFailedTests += test_lofarImages ();
  
  /* Clean up directory */
  for (unsigned int n(0); n<images.size(); n++) {
    casa::Directory dir (images[n].c_str());
    dir.removeRecursive(); 
  }
  
  return nofFailedTests;
}
