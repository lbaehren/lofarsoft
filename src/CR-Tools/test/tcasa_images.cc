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
#include <coordinates/Coordinates/CoordinateSystem.h>
#include <coordinates/Coordinates/DirectionCoordinate.h>
#include <coordinates/Coordinates/LinearCoordinate.h>
#include <coordinates/Coordinates/SpectralCoordinate.h>
#include <images/Images/PagedImage.h>
#include <lattices/Lattices/TiledShape.h>

#ifdef HAVE_HDF5
#include <images/Images/HDF5Image.h>
#endif

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
*/

// ==============================================================================
//
//  Helper routines
//
// ==============================================================================

/*!
  \brief Create the shape information for the created image

  \param nDistance  -- Number of steps along the distance axis
  \param nTime      -- Number of integration steps in time
  \param nFrequency -- Number of frequency channels
  
  \return shape -- The shape of the image, i.e. the number of axes and their
          respective lengths, e.g. <tt>shape=[25,25,10,50,128]</tt>
*/
IPosition image_shape (int const &nDistance=10,
		       int const &nTime=50,
		       int const &nFrequency=128)
{
  IPosition shape (5,25,25,nDistance,nTime,nFrequency);

  return shape;
}

// ------------------------------------------------------------------------------

/*!
  \brief Create observation information object
  
  \return obsInfo -- Object containing misc. information on about the observation,
          such as the name of the telescope and the observer, but also the time at
	  which the observation was done.
*/
casa::ObsInfo observation_info (std::string const &telescope="LOFAR",
				std::string const &observer="Lars Baehren")
{
  casa::ObsInfo obsInfo;
  casa::Time startTime;
  casa::Quantity epoch (startTime.modifiedJulianDay(), "d");

  obsInfo.setObsDate (epoch);
  obsInfo.setTelescope (casa::String(telescope));
  obsInfo.setObserver (casa::String(observer));

  return obsInfo;
}

// ------------------------------------------------------------------------------

casa::DirectionCoordinate direction_coordinate (double const &increment=2.0)
{
  IPosition shape;
  Matrix<double> xform(2,2);
  
  shape            = image_shape();
  xform            = 0.0;
  xform.diagonal() = 1.0;
  
  casa::DirectionCoordinate coord (casa::MDirection::J2000,
				   casa::Projection(casa::Projection::STG),
				   0.0*pi/180.0,
				   90.0*pi/180.0,
				   -increment*pi/180.0,
				   increment*pi/180.0,
				   xform,
				   0.5*shape(0),
				   0.5*shape(1));
  
  coord.setReferencePixel(Vector<double>(2,0.0));

  return coord;
}

// ------------------------------------------------------------------------------

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

casa::SpectralCoordinate spectral_coordinate () 
{
    Vector<String> names  (1,"Frequency");
    casa::SpectralCoordinate axis;
    
    axis.setWorldAxisNames(names);
 
    return axis;
}

// ------------------------------------------------------------------------------

casa::LinearCoordinate time_coordinate (double const &referenceValue=0.0,
					double const &referencePixel=0.0,
					double const &increment=1.0)
{
  Vector<String> names  (1,"Time");
  Vector<String> units  (1,"s");
  Vector<double> refVal (1,referenceValue);
  Vector<double> inc    (1,increment);
  Matrix<double> pc     (1,1,1.0);
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

casa::LinearCoordinate faraday_coordinate (double const &referenceValue=0.0,
					   double const &referencePixel=0.0,
					   double const &increment=0.0)
{
  Vector<String> names (1,"Faraday rotation");
  Vector<String> units (1,"rad/(m)2");
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
	 LOFAR we make use of the following values: "CR_SKY", "RM_CUBE", "RM_MAP".
  \param incrementDirection -- Increment (resolution) along the direction axes.
  \param incrementDistance  -- Increment along the distance axis.
  \param incrementTime      -- Increment along the frequency axis.
  
  \return cs -- Coordinate system tool
*/
casa::CoordinateSystem image_csys (std::string imageType="CR_SKY",
				   double incrementDirection=2.0,
				   double incrementDistance=0.0,
				   double incrementTime=1.0)
{
  casa::CoordinateSystem cs;
  
  if (imageType == "CR_SKY") {
    // General observation information
    cs.setObsInfo(observation_info());
    // Direction axis
    cs.addCoordinate(direction_coordinate(incrementDirection));
    // [2] Distance axis
    cs.addCoordinate(radial_coordinate());
    // [3] Time axis
    cs.addCoordinate(time_coordinate());
    // [4] Frequency axis
    cs.addCoordinate(spectral_coordinate());
  }
  else if (imageType == "RM_CUBE") {
    // General observation information
    cs.setObsInfo(observation_info());
    // Direction axis
    cs.addCoordinate(direction_coordinate(incrementDirection));
    // Faraday rotation
    cs.addCoordinate (faraday_coordinate());
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
    cs.setObsInfo(observation_info());
    // Direction axis
    cs.addCoordinate(direction_coordinate(incrementDirection));
  }
  else {
    std::cout << "[image_csys] Unknown image type!" << std::endl;
  }
  
  return cs;
}

// ------------------------------------------------------------------------------

/*!
  \brief Provide a summary of the image's properties

  Example output:
  \verbatim
  -- Image type ....... : HDF5Image
  -- Table name ....... : hdf5image_d.h5
  -- Image shape ...... : [25, 25, 10, 50, 128]
  -- Maximum cache size : 0
  \endverbatim
  
  \param image -- Image object derived from the ImageInterface class.
*/
template <class T>
void image_summary (casa::ImageInterface<T> &image)
{
  cout << "-- Image type ............. : " << image.imageType() << endl;
  cout << "-- Table name ............. : " << image.name()      << endl;
  cout << "-- Image shape ............ : " << image.shape()     << endl;
  cout << "-- Maximum cache size ..... : " << image.maximumCacheSize() << endl;
  cout << "-- Is the image paged? .... : " << image.isPaged()      << endl;
  cout << "-- Is the image persistent? : " << image.isPersistent() << endl;
  cout << "-- Is the image writable?   : " << image.isWritable()   << endl;
  cout << "-- Has the image a mask?    : " << image.hasPixelMask() << endl;
}

template void image_summary (casa::ImageInterface<float> &image);
template void image_summary (casa::ImageInterface<double> &image);
template void image_summary (casa::ImageInterface<casa::Complex> &image);
template void image_summary (casa::ImageInterface<casa::DComplex> &image);

// ==============================================================================
//
//  Testing routines
//
// ==============================================================================

/*
  \brief Test opening an existing image and try working with it

  \param infile -- Name of the image file stored on disk

  \return nofFailedTests -- The number of failed tests in this function
*/
int test_openImage (std::string const &infile)
{
  cout << "\n[test_openImage]\n" << endl;

  int nofFailedTests (0);

  cout << "[1] Opening existing image ..." << endl;
  try {
    
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  return nofFailedTests;
}

// ------------------------------------------------------------------------------

/*!
  \brief Test the creation of a PagedImage

  \return nofFailedTests -- The number of failed tests in this function
*/
int test_createImage ()
{
  cout << "\n[test_createImage]\n" << endl;

  int nofFailedTests (0);

  IPosition shape (image_shape());
  casa::TiledShape tshape (shape);
  casa::String filename;
    
  cout << "[1] Creating new PagedImage<T> ..." << endl;
  try {
    cout << "-> PagedImage<float>" << endl;
    casa::PagedImage<float> imageFloat (tshape,
					image_csys(),
					"testimage01f.img");
    image_summary (imageFloat);
    
    cout << "-> PagedImage<float>" << endl;
    casa::PagedImage<double> imageDouble (tshape,
					  image_csys(),
					  "testimage01d.img");
    image_summary (imageDouble);
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }
  
  cout << "[2] Creating new PagedImage<float> ..." << endl;
  try {
    filename = "testimage02.img";
    shape(0) = shape(1) = 200;
    //
    casa::PagedImage<float> image (tshape,
				   image_csys(),
				   filename);
    //
    image_summary (image);    
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  cout << "[3] Creating new PagedImage<float> ..." << endl;
  try {
    filename = "testimage03.img";
    shape = IPosition (5,512,512,100,512,128);
    //
    casa::PagedImage<float> image (tshape,
				   image_csys(),
				   filename);
    //
    image_summary (image);    
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
  IPosition shape (image_shape());
  int nofAxes = shape.nelements();
  casa::TiledShape tshape (shape);
  casa::String filename;
  
  cout << "[1] Testing default constructor ..." << endl;
  try {    
    cout << "--> HDF5Image<float>" << endl;
    casa::HDF5Image<float> imageFloat (tshape,
				       image_csys(),
				       "hdf5image_f.h5");
    image_summary (imageFloat);
    
    cout << "--> HDF5Image<double>" << endl;
    casa::HDF5Image<double> imageDouble (tshape,
					 image_csys(),
					 "hdf5image_d.h5");
    image_summary (imageDouble);
    
    cout << "--> HDF5Image<casa::Complex>" << endl;
    casa::HDF5Image<casa::Complex> imageComplex (tshape,
						 image_csys(),
						 "hdf5image_c.h5");
    image_summary (imageComplex);
    
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
  int nofFailedTests (0);
  std::string imageType;

  std::cout << "[1] Create image holding RM cube ..." << std::endl;
  try {
    IPosition shape (4,4096,4096,500,4);
    casa::TiledShape tshape (shape);

    imageType = "RM_CUBE";
    
    casa::HDF5Image<double> imageDouble (tshape,
					 image_csys(imageType),
					 "lofar_rm_cube.h5");    
  } catch (std::string message) {
    std::cerr << message << endl;
    nofFailedTests++;
  }

  std::cout << "[2] Create image holding RM map ..." << std::endl;
  try {
    IPosition shape (2,4096,4096);
    casa::TiledShape tshape (shape);

    imageType = "RM_MAP";
    
    casa::HDF5Image<double> imageDouble (tshape,
					 image_csys(imageType),
					 "lofar_rm_map.h5");    
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

  /* check the input parameters first */

  if (argc < 2) {
    cout << "[tcasa_image] No path to image provided." << endl;
  } else {
    // get the name of the image file ...
    std::string filename = argv[1];
    // .. and try to open it
    nofFailedTests += test_openImage (filename);
  }

  /* Everything from here on does not rely on the existance of an already
     pre-existing image. */

  nofFailedTests += test_createImage();

#ifdef HAVE_HDF5
  nofFailedTests += test_HDF5Image ();
  nofFailedTests += create_lofar_images();
#endif

  return nofFailedTests;
}
