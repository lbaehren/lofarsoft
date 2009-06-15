/*-------------------------------------------------------------------------*
 | $Id:: tNewClass.cc 1302 2008-03-11 10:04:05Z baehren                  $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2007                                                    *
 *   Lars Baehren (bahren@astron.nl)                                       *
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

#include <crtools.h>

#include <iostream>

#include <casa/aips.h>
#include <casa/Arrays/Array.h>
#include <casa/Arrays/IPosition.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/Vector.h>
#include <casa/OS/Directory.h>
#include <coordinates/Coordinates/CoordinateSystem.h>
#include <coordinates/Coordinates/DirectionCoordinate.h>
#include <coordinates/Coordinates/LinearCoordinate.h>
#include <coordinates/Coordinates/SpectralCoordinate.h>
#include <images/Images/ImageFITSConverter.h>
#include <images/Images/PagedImage.h>
#include <lattices/Lattices/TiledShape.h>

#ifdef HAVE_HDF5
#include <images/Images/HDF5Image.h>
#endif

// CR-Tools header files
#include <Coordinates/CoordinateType.h>
#include <Coordinates/SpatialCoordinate.h>
#include <Utilities/MConversions.h>
#include <Utilities/TestsCommon.h>

using std::cout;
using std::endl;

using casa::IPosition;
using casa::Matrix;
using casa::String;
using casa::Vector;

const double pi (3.14159265);

/*!
  \file tcasa_images.cc

  \ingroup CR_test

  \brief A number of tests for clases in the casacore images module

  \author Lars B&auml;hren

  Since we are using derivatives of the casa::ImageInterface as primary output
  of the CR imager, we obviously need a good understanding of how to work with
  the CASA/casacore classes involved.
*/

// ==============================================================================
//
//  Helper routines
//
// ==============================================================================

//_______________________________________________________________________________

/*!
  \brief Get list of reference codes for celestial coordinate systems

  \return refcodes -- Vector with a list of reference codes for celestial
          coordinate systems.
*/
std::vector<std::string> systemRefcodes ()
{
  std::vector<std::string> refcode;

  refcode.push_back("AZEL");      /* topocentric Azimuth and Elevation (N through E) */
  refcode.push_back("AZELSW");    /* topocentric Azimuth and Elevation (S through W) */
  refcode.push_back("AZELNE");    /* topocentric Azimuth and Elevation (N through E) */
  refcode.push_back("AZELGEO");   /* geodetic Azimuth and Elevation (N through E)    */
  refcode.push_back("AZELSWGEO"); /* geodetic Azimuth and Elevation (S through W)    */
  refcode.push_back("AZELNEGEO"); /* geodetic Azimuth and Elevation (N through E)    */
  refcode.push_back("B1950");     /* mean epoch and ecliptic at B1950.0              */
  refcode.push_back("ECLIPTIC");  /* ecliptic for J2000 equator and equinox          */
  refcode.push_back("ICRS");      /* International Celestial reference system        */
  refcode.push_back("J2000");     /* mean equator and equinox at J2000.0 (FK5)       */
  refcode.push_back("GALACTIC");  /* galactic coordinates                            */

  return refcode;
}

//_______________________________________________________________________________
//                                                            directionCoordinate

/*!
  \param nelem      -- Number of pixel elements along the coordinate axes.
  \param refcode    -- 
  \param projection -- 
*/
casa::DirectionCoordinate directionCoordinate (std::vector<uint> const &nelem,
					       std::string const &refcode="AZEL",
					       std::string const &projection="STG")
{
  casa::Vector<double> refPixel(2);
  Vector<Quantum<double> > refValue(2);
  Vector<Quantum<double> > increment(2);
  casa::Matrix<double> xform(2,2);
  // Reference pixel is center of map for directional components
  refPixel(0) = nelem[0]/2.0;
  refPixel(1) = nelem[1]/2.0;
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
  return casa::DirectionCoordinate (CR::MDirectionType(refcode),
				    CR::ProjectionType(projection),
				    refValue(0),
				    refValue(1),
				    increment(0),
				    increment(1),
				    xform,
				    refPixel(0),
				    refPixel(1));
}

//_______________________________________________________________________________
//                                                              tabularCoordinate

/*
  \brief Get a tabular coordinate

  \param unit -- World axis unit of the coordinate.
  \param name -- Name of the coordinate axis.
  \param type -- Distribution of the tabulated values: "log", "group"

  \return coord -- Tabular coordinate
*/
casa::TabularCoordinate tabularCoordinate (std::string const &unit="m",
					   std::string const &name="Length",
					   std::string const &type="log") 
{
  uint nelem (10);
  Vector<double> pixel (nelem,0.0);
  Vector<double> world (nelem,0.0);

  if (type == "log") {
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
  }
  else if (type == "group") {
    pixel(0) = 0;   world(0) = 1;
    pixel(1) = 1;   world(1) = 2;
    pixel(2) = 2;   world(2) = 3;
    pixel(3) = 3;   world(3) = 10;
    pixel(4) = 4;   world(4) = 11;
    pixel(5) = 5;   world(5) = 12;
    pixel(6) = 6;   world(6) = 13;
    pixel(7) = 7;   world(7) = 101;
    pixel(8) = 8;   world(8) = 102;
    pixel(9) = 9;   world(9) = 103;
  }
  
  return casa::TabularCoordinate (pixel,world,unit,name);
}

//_______________________________________________________________________________
//                                                              radial_coordinate

/*!
  \brief Create LinearCoordinate object for radial distance

  \param referenceValue -- Reference value
  \param referencePixel -- Reference pixel
  \param increment      -- Coordinate increment

  \return coord -- Coordinate of type "Linear", describing the radial distance
          in a spherical coordinate system.
*/
casa::LinearCoordinate radial_coordinate (double const &referenceValue=0.0,
					  double const &referencePixel=0.0,
					  double const &increment=0.0)
{
  Vector<String> names (1,"Distance");
  Vector<String> units (1,"m");
  Vector<double> refVal (1,referenceValue);
  Vector<double> inc (1,increment);
  Matrix<double> pc (1,1,1.0);
  Vector<double> refPix (1,referencePixel);
  
  casa::LinearCoordinate coord (names,
				units,
				refVal,
				inc,
				pc,
				refPix);

  return coord;
}

//_______________________________________________________________________________
//                                                                     image_csys

/*!
  \brief Create the coordinate system attached to the image

  This function creates a coordinate system object 

  \param imageType          -- Type of image for which to create the coordinate
         system; since we want to cover the basic image types to be defined for
	 LOFAR we make use of the following values: "CR_SKYMAP", "RM_CUBE", "RM_MAP".
  \param incrementDirection -- Increment (resolution) along the direction axes.
  \param incrementDistance  -- Increment along the distance axis.
  \param incrementTime      -- Increment along the frequency axis.
  
  \return cs -- Coordinate system tool
*/
casa::CoordinateSystem image_csys (std::string imageType="CR_SKYMAP",
				   double incrementDirection=2.0,
				   double incrementDistance=0.0,
				   double incrementTime=1.0)
{
  casa::CoordinateSystem cs;
  
  if (imageType == "CR_SKYMAP") {
    // General observation information
    cs.setObsInfo(CR::test_ObsInfo());
    // Direction axis
    cs.addCoordinate(CR::CoordinateType::makeDirectionCoordinate());
    // [2] Distance axis
    cs.addCoordinate(radial_coordinate());
    // [3] Time axis
    cs.addCoordinate(CR::CoordinateType::makeTimeCoordinate());
    // [4] Frequency axis
    cs.addCoordinate(CR::CoordinateType::makeSpectralCoordinate());
  }
  else if (imageType == "RM_CUBE") {
    // General observation information
    cs.setObsInfo(CR::test_ObsInfo());
    // Direction axis
    cs.addCoordinate(CR::CoordinateType::makeDirectionCoordinate());
    // Faraday rotation
    cs.addCoordinate (CR::CoordinateType::makeFaradayCoordinate());
    // Polarization
    cs.addCoordinate (CR::CoordinateType::makeStokesCoordinate());
  }
  else if (imageType == "RM_MAP") {
    // General observation information
    cs.setObsInfo(CR::test_ObsInfo());
    // Direction axis
    cs.addCoordinate(CR::CoordinateType::makeDirectionCoordinate());
  }
  else {
    std::cout << "[image_csys] Unknown image type!" << std::endl;
  }
  
  return cs;
}

// ==============================================================================
//
//  Testing routines
//
// ==============================================================================

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

//_______________________________________________________________________________
//                                                               test_export2fits

/*!
  \brief Test translation of WCS information during export to FITS

  \return nofFailedTests -- The number of failed tests encountered within this
          function.
*/
int test_export2fits ()
{
  cout << "\n[tcasacoremages::test_export2fits]\n" << endl;

  int nofFailedTests (0);

  bool status (true);
  casa::String error;
  std::string outfile;
  std::vector<std::string> refcode = systemRefcodes();
  std::vector<std::string> projection = CR::ProjectionNames();
  casa::CoordinateSystem csys;
  casa::IPosition shape (3,50,50,10);
  casa::TiledShape tile (shape);

  for (uint ref(0); ref<refcode.size(); ref++) {
    for (uint proj(0); proj<projection.size(); proj++) {
      // report tested combination
      cout << "-- Testing " << refcode[ref] << " / " << projection[proj] << endl;
      // base name of the output file
      std::string filename = "image-" + refcode[ref] + "-" + projection[proj];
      /// create spatial coordinate 
      CR::SpatialCoordinate coord (CR::CoordinateType::DirectionRadius,
				   refcode[ref],
				   projection[proj]);
      coord.toCoordinateSystem (csys,false);
      csys.setObsInfo(CR::test_ObsInfo());
      // create PagedImage ...
      outfile = filename + ".img";
      casa::PagedImage<float> image_paged (tile,
					   csys,
					   outfile);
      // .. and export to FITS
      outfile = "!" + filename + ".fits";
      status = casa::ImageFITSConverter::ImageToFITS(error,
						     image_paged,
						     outfile);
      // create HDF5Image
#ifdef HAVE_HDF5
      outfile = filename + ".h5";
      casa::HDF5Image<float> image_hdf5 (tile,
					 csys,
					 outfile);
#endif
    }
  }

  return nofFailedTests;
}

// ------------------------------------------------------------------------------

/*!
  \brief Test the creation of a casa::PagedImage on disk

  \return nofFailedTests -- The number of failed tests encountered within this
          function
*/
int test_PagedImage ()
{
  cout << "\n[test_PagedImage]\n" << endl;

  int nofFailedTests (0);

  IPosition shape (5,25,25,10,50,128);
  casa::TiledShape tshape (shape);
    
  cout << "[1] Creating new PagedImage<float> ..." << endl;
  try {
    /* create image file */
    casa::PagedImage<float> imageFloat (tshape,
					image_csys(),
					"testimage_f.img");
    /* feedback */
    CR::summary (imageFloat);
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  cout << "[2] Creating new PagedImage<double> ..." << endl;
  try {
    /* create image file */
    casa::PagedImage<double> imageDouble (tshape,
					  image_csys(),
					  "testimage_d.img");
    /* feedback */
    CR::summary (imageDouble);
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
    
  return nofFailedTests;
}

// ------------------------------------------------------------------------------

#ifdef HAVE_HDF5

/*!
  \brief Test the creation of a HDF5Image

  With <tt>ImageInterface</tt> operating as base class for all types of images,
  from an application programmer's point of view a <tt>HDF5Image</tt> works just
  as a <tt>PagedImage</tt>.

  \return nofFailedTests -- The number of failed tests in this function
*/
int test_HDF5Image ()
{
  cout << "\n[test_HDF5Image]\n" << endl;

  int nofFailedTests (0);
  IPosition shape (5,25,25,10,50,128);
  int nofAxes = shape.nelements();
  casa::TiledShape tshape (shape);
  casa::String filename;
  
  cout << "[1] Testing default constructor ..." << endl;
  try {    
    cout << "--> HDF5Image<float>" << endl;
    casa::HDF5Image<float> imageFloat (tshape,
				       image_csys(),
				       "hdf5image_f.h5");
    CR::summary (imageFloat);
    
    cout << "--> HDF5Image<double>" << endl;
    casa::HDF5Image<double> imageDouble (tshape,
					 image_csys(),
					 "hdf5image_d.h5");
    CR::summary (imageDouble);
    
    cout << "--> HDF5Image<casa::Complex>" << endl;
    casa::HDF5Image<casa::Complex> imageComplex (tshape,
						 image_csys(),
						 "hdf5image_c.h5");
    CR::summary (imageComplex);
    
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  cout << "[2] Write pixels values one by one ..." << endl;
  try {
    int nAxis   = nofAxes-1;
    IPosition pos (nofAxes,0);
    casa::HDF5Image<double> imageDouble (tshape,
					 image_csys(),
					 "hdf5image.h5");

    cout << "-- start writing pixel values ... " << std::flush;
    for (int n(0); n<shape(nAxis); n++) {
      pos(nAxis) = n;
      imageDouble.putAt (double(n),pos);
    }
    cout << "DONE." << endl;

  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  cout << "[3] Writing pixel array of same rank ..." << endl;
  try {
    // open the previously created image
    cout << "-- opening previously created image ..." << endl;
    casa::HDF5Image<double> image ("hdf5image.h5");
    // set up the array to be inserted into the image buffer
    IPosition shapeArray (shape);
    shapeArray(nofAxes-1) = 1;
    casa::Array<double> arr (shapeArray);
    // set up variable to handle writing to the buffer
    IPosition start (nofAxes,0);
    IPosition stride (nofAxes,1);
    // access the image
    cout << "-- start writing data to image ... " << std::flush;
    for (int n(0); n<shape(4); n++) {
      start(4) = n;
      arr      = 1.0*n;
      image.doPutSlice (arr,start,stride);
    }
    cout << "DONE." << endl;
    // Feedback
    cout << "-- Image file  = " << image.name()   << endl;
    cout << "-- Image shape = " << image.shape()  << endl;
    cout << "-- Array shape = " << arr.shape()    << endl;
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  cout << "[4] Writing pixel array of lower rank ..." << endl;
  try {
    // open the previously created image
    cout << "-- opening previously created image ..." << endl;
    casa::HDF5Image<double> image ("hdf5image.h5");
    // set up the array to be inserted into the image buffer
    IPosition shapeArray (nofAxes-1);
    shapeArray(0) = shape(0);
    shapeArray(1) = shape(1);
    shapeArray(2) = shape(2);
    shapeArray(3) = shape(3);
    casa::Array<double> arr (shapeArray);
    // set up variable to handle writing to the buffer
    IPosition start (nofAxes,0);
    IPosition stride (nofAxes,1);
    // access the image
    cout << "-- start writing data to image ... " << std::flush;
    for (int n(0); n<shape(4); n++) {
      start(4) = n;
      arr      = 10.0*n;
      image.doPutSlice (arr,start,stride);
    }
    cout << "DONE." << endl;
    // Feedback
    cout << "-- Image file  = " << image.name()   << endl;
    cout << "-- Image shape = " << image.shape()  << endl;
    cout << "-- Array shape = " << arr.shape()    << endl;
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}

#endif

// ------------------------------------------------------------------------------

/*
  \brief Create an image matching the expected characteristics of a LOFAR sky image

  \return nofFailedTests -- The number of failed tests/operations encountered 
          within this function
*/
int create_lofar_images ()
{
  std::cout << "\n[create_lofar_images]\n" << std::endl;

  int nofFailedTests (0);
  std::string imageType;
  casa::String error;

  //__________________________________________________________________
  // Create image holding standard sky image

   std::cout << "[1] Create image holding standard sky image ..." << std::endl; 
  try {
    IPosition shape (4,4096,4096,1024,4);
    casa::TiledShape tshape (shape);
    
    casa::CoordinateSystem cs;
    // General observation information
    cs.setObsInfo(CR::test_ObsInfo());
    // Direction coordinate
    cs.addCoordinate(CR::CoordinateType::makeDirectionCoordinate());
    // Spectral coordinate
    cs.addCoordinate(CR::CoordinateType::makeSpectralCoordinate());
    // Stokes coordinate
    cs.addCoordinate(CR::CoordinateType::makeStokesCoordinate());

  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  //__________________________________________________________________
  // Create image holding RM cube

  std::cout << "[2] Create image holding RM cube ..." << std::endl;
  try {
    IPosition shape (4,4096,4096,500,4);
    casa::TiledShape tshape (shape);

    imageType = "RM_CUBE";

#ifdef HAVE_HDF5    
    casa::HDF5Image<double> image (tshape,
				   image_csys(imageType),
				   "lofar_rmc.h5");
#else
    casa::PagedImage<double> image (tshape,
				    image_csys(imageType),
				    "lofar_rmc.img");
#endif
    CR::summary(image);
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  //__________________________________________________________________
  // Create image holding RM map
  
  std::cout << "[3] Create image holding RM map ..." << std::endl;
  try {
    IPosition shape (2,4096,4096);
    casa::TiledShape tshape (shape);

    imageType = "RM_MAP";

#ifdef HAVE_HDF5    
    casa::HDF5Image<double> image (tshape,
				   image_csys(imageType),
				   "lofar_rmm.h5");    
#else
    casa::PagedImage<double> image (tshape,
				    image_csys(imageType),
				    "lofar_rmm.img");    
#endif
    CR::summary(image);
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  //__________________________________________________________________
  // Create image holding CR sky map

  std::cout << "[4] Create image holding CR sky map ..." << std::endl;
  try {
    IPosition shape (5,256,256,100,10,513);
    casa::TiledShape tshape (shape);

    imageType = "CR_SKYMAP";

#ifdef HAVE_HDF5    
    casa::HDF5Image<double> image (tshape,
				   image_csys(imageType),
				   "lofar_crc.h5");    
#else
    casa::PagedImage<double> image (tshape,
				    image_csys(imageType),
				    "lofar_crc.img");    
#endif
    CR::summary(image);
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }


  return nofFailedTests;
}

// ------------------------------------------------------------------------------

int main (int argc,
	  char *argv[])
{
  int nofFailedTests (0);

  /* Tests for constructing an image object */
  nofFailedTests += test_PagedImage ();
  
#ifdef HAVE_HDF5
  nofFailedTests += test_HDF5Image ();
#endif

  /* Create a set of example standard LOFAR images */
  nofFailedTests += create_lofar_images();

  return nofFailedTests;
}
