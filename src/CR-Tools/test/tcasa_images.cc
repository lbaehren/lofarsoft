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
#include <Coordinates/MConversions.h>
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

/*!
  \brief Get a tabular coordinate

  \param unit -- World axis unit of the coordinate.
  \param name -- Name of the coordinate axis.
  \param type -- Distribution of the tabulated values: "log", "group"
  - log : [0,1,2,5,10,20,50,100,200,500]
  - group : [0,1,2,3,10,,11,12,13,100,101,102,103]

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
    pixel(0) = 0;   world(0) = 0;
    pixel(1) = 1;   world(1) = 1;
    pixel(2) = 2;   world(2) = 2;
    pixel(3) = 3;   world(3) = 3;
    pixel(4) = 4;   world(4) = 10;
    pixel(5) = 5;   world(5) = 11;
    pixel(6) = 6;   world(6) = 12;
    pixel(7) = 7;   world(7) = 13;
    pixel(8) = 8;   world(8) = 100;
    pixel(9) = 9;   world(9) = 101;
  }
  
  return casa::TabularCoordinate (pixel,world,unit,name);
}

// ==============================================================================
//
//  Testing routines
//
// ==============================================================================

//_______________________________________________________________________________
//                                                               test_export2fits

/*!
  \brief Test translation of WCS information during export to FITS

  \return nofFailedTests -- The number of failed tests encountered within this
          function.
*/
int test_export2fits ()
{
  cout << "\n[tcasa_images::test_export2fits]\n" << endl;

  int nofFailedTests (0);

  bool status (true);
  casa::String error;
  std::string outfile;
  std::vector<std::string> refcode = CR::MDirectionNames();
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
  cout << "\n[tcasa_images::test_PagedImage]\n" << endl;

  int nofFailedTests (0);

  IPosition shape (4,1024,1024,512,4);
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

  cout << "[1] Creating new PagedImage<float> ..." << endl;
  try {
    /* create image file */
    casa::PagedImage<float> imageFloat (tshape,
					cs,
					"testimage_float.img");
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
					  cs,
					  "testimage_double.img");
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
  cout << "\n[tcasa_images::test_HDF5Image]\n" << endl;

  int nofFailedTests (0);
  IPosition shape (4,1024,1024,512,4);
  int nofAxes = shape.nelements();
  casa::TiledShape tshape (shape);
  casa::String filename;
  casa::CoordinateSystem cs;

  // General observation information
  cs.setObsInfo(CR::test_ObsInfo());
  // Direction coordinate
  cs.addCoordinate(CR::CoordinateType::makeDirectionCoordinate());
  // Spectral coordinate
  cs.addCoordinate(CR::CoordinateType::makeSpectralCoordinate());
  // Stokes coordinate
  cs.addCoordinate(CR::CoordinateType::makeStokesCoordinate());
  
  cout << "[1] Testing default constructor ..." << endl;
  try {    
    cout << "--> HDF5Image<float>" << endl;
    casa::HDF5Image<float> imageFloat (tshape,
				       cs,
				       "hdf5image_float.h5");
    CR::summary (imageFloat);
    
    cout << "--> HDF5Image<double>" << endl;
    casa::HDF5Image<double> imageDouble (tshape,
					 cs,
					 "hdf5image_double.h5");
    CR::summary (imageDouble);
    
    cout << "--> HDF5Image<casa::Complex>" << endl;
    casa::HDF5Image<casa::Complex> imageComplex (tshape,
						 cs,
						 "hdf5image_complex.h5");
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
					 cs,
					 "hdf5image_double.h5");

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
    casa::HDF5Image<double> image ("hdf5image_double.h5");
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
    casa::HDF5Image<double> image ("hdf5image_double.h5");
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

//_______________________________________________________________________________
//                                                                   lofar_images

/*
  \brief Create an image matching the expected characteristics of a LOFAR image

  \return nofFailedTests -- The number of failed tests/operations encountered 
          within this function
*/
int lofar_images ()
{
  cout << "\n[tcasa_images::lofar_images]\n" << endl;

  int nofFailedTests (0);

  //__________________________________________________________________
  // Create image holding standard sky image

   cout << "[1] Create image holding standard sky image ..." << endl; 
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

#ifdef HAVE_HDF5
    casa::HDF5Image<double> image (tshape,
				   cs,
				   "lofar_sky.h5");
#else
    casa::PagedImage<double> image (tshape,
				    cs,
				    "lofar_sky.img");
#endif
    CR::summary(image);
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  //__________________________________________________________________
  // Create image holding RM cube

  cout << "[2] Create image holding RM cube ..." << endl;
  try {
    IPosition shape (4,4096,4096,1024,4);
    casa::TiledShape tshape (shape);

    casa::CoordinateSystem cs;
    // General observation information
    cs.setObsInfo(CR::test_ObsInfo());
    // Direction axis
    cs.addCoordinate(CR::CoordinateType::makeDirectionCoordinate());
    // Faraday rotation
    cs.addCoordinate (CR::CoordinateType::makeFaradayCoordinate());
    // Polarization
    cs.addCoordinate (CR::CoordinateType::makeStokesCoordinate());

#ifdef HAVE_HDF5
    casa::HDF5Image<double> image (tshape,
				   cs,
				   "lofar_rmc.h5");
#else
    casa::PagedImage<double> image (tshape,
				    cs,
				    "lofar_rmc.img");
#endif
    CR::summary(image);
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  //__________________________________________________________________
  // Create image holding RM map
  
  cout << "[3] Create image holding RM map ..." << endl;
  try {
    IPosition shape (3,4096,4096,4);
    casa::TiledShape tshape (shape);

    casa::CoordinateSystem cs;
    // General observation information
    cs.setObsInfo(CR::test_ObsInfo());
    // Direction axis
    cs.addCoordinate(CR::CoordinateType::makeDirectionCoordinate());
    // Polarization
    cs.addCoordinate (CR::CoordinateType::makeStokesCoordinate());

#ifdef HAVE_HDF5    
    casa::HDF5Image<double> image (tshape,
				   cs,
				   "lofar_rmm.h5");    
#else
    casa::PagedImage<double> image (tshape,
				    cs,
				    "lofar_rmm.img");    
#endif
    CR::summary(image);
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  //__________________________________________________________________
  // Create image holding CR sky map

  cout << "[4] Create image holding CR sky map ..." << endl;
  try {
    IPosition shape (5,256,256,100,513,10);
    casa::TiledShape tshape (shape);
    
    casa::CoordinateSystem cs;
    // General observation information
    cs.setObsInfo(CR::test_ObsInfo());
    // Direction axis
    cs.addCoordinate(CR::CoordinateType::makeDirectionCoordinate());
    // Distance axis
    cs.addCoordinate(tabularCoordinate());
    // Frequency axis
    cs.addCoordinate(CR::CoordinateType::makeSpectralCoordinate());
    // Time axis
    cs.addCoordinate(CR::CoordinateType::makeTimeCoordinate());
    
#ifdef HAVE_HDF5    
    casa::HDF5Image<double> image (tshape,
				   cs,
				   "lofar_crc.h5");    
#else
    casa::PagedImage<double> image (tshape,
				    cs,
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

  /* Parse command-line options */

  for (int arg(1); arg<argc; arg++) {
    if (std::string(argv[arg]) == "--paged") {
      nofFailedTests += test_PagedImage ();
    }
#ifdef HAVE_HDF5
    else if (std::string(argv[arg]) == "--hdf5") {
      nofFailedTests += test_HDF5Image ();
    }
#endif
    else if (std::string(argv[arg]) == "--lofar") {
      nofFailedTests += lofar_images();
    }
    else {
      std::cerr << "[tcasa_images] Unrecognized command line option!" << endl;
      std::cerr << "\tValid options are:" << endl;
      std::cerr << "\t--paged"            << endl;
      std::cerr << "\t--hdf5"             << endl;
      std::cerr << "\t--lofar"            << endl;
    }
  }
  
  return nofFailedTests;
}
