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
#include <images/Images/PagedImage.h>
// #include <images/Images/ImageFITSConverter.h>
#include <lattices/Lattices/TiledShape.h>

#ifdef HAVE_HDF5
#include <images/Images/HDF5Image.h>
#endif

// CR-Tools header files
#include <Coordinates/CoordinateType.h>
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

// ------------------------------------------------------------------------------

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

// ------------------------------------------------------------------------------

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
    {
      Vector<casa::Int> iquv(4);
      iquv(0) = casa::Stokes::I;
      iquv(1) = casa::Stokes::Q;
      iquv(2) = casa::Stokes::U;
      iquv(3) = casa::Stokes::V;
      casa::StokesCoordinate coord(iquv);
      cs.addCoordinate (coord);
    }
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
    CR::image_summary (imageFloat);
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
    CR::image_summary (imageDouble);
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
    CR::image_summary (imageFloat);
    
    cout << "--> HDF5Image<double>" << endl;
    casa::HDF5Image<double> imageDouble (tshape,
					 image_csys(),
					 "hdf5image_d.h5");
    CR::image_summary (imageDouble);
    
    cout << "--> HDF5Image<casa::Complex>" << endl;
    casa::HDF5Image<casa::Complex> imageComplex (tshape,
						 image_csys(),
						 "hdf5image_c.h5");
    CR::image_summary (imageComplex);
    
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
  // Create image holding RM cube

  std::cout << "[1] Create image holding RM cube ..." << std::endl;
  try {
    IPosition shape (4,4096,4096,500,4);
    casa::TiledShape tshape (shape);

    imageType = "RM_CUBE";

#ifdef HAVE_HDF5    
    casa::HDF5Image<double> image (tshape,
				   image_csys(imageType),
				   "lofar_rm_cube.h5");
#else
    casa::PagedImage<double> image (tshape,
				    image_csys(imageType),
				    "lofar_rm_cube.img");
#endif
    CR::image_summary(image);
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  //__________________________________________________________________
  // Create image holding RM map
  
  std::cout << "[2] Create image holding RM map ..." << std::endl;
  try {
    IPosition shape (2,4096,4096);
    casa::TiledShape tshape (shape);

    imageType = "RM_MAP";

#ifdef HAVE_HDF5    
    casa::HDF5Image<double> image (tshape,
				   image_csys(imageType),
				   "lofar_rm_map.h5");    
#else
    casa::PagedImage<double> image (tshape,
				    image_csys(imageType),
				    "lofar_rm_map.img");    
#endif
    CR::image_summary(image);
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  //__________________________________________________________________
  // Create image holding CR sky map

  std::cout << "[3] Create image holding CR sky map ..." << std::endl;
  try {
    IPosition shape (5,256,256,100,10,513);
    casa::TiledShape tshape (shape);

    imageType = "CR_SKYMAP";

#ifdef HAVE_HDF5    
    casa::HDF5Image<double> image (tshape,
				   image_csys(imageType),
				   "lofar_cr_skymap.h5");    
#else
    casa::PagedImage<double> image (tshape,
				    image_csys(imageType),
				    "lofar_cr_skymap.img");    
#endif
    CR::image_summary(image);
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
