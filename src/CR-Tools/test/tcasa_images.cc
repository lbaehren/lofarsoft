/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
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

/*!
  \file tcasa_images.cc

  \ingroup CR_test

  \brief A number of tests for clases in the casacore images module

  \author Lars B&auml;hren
*/

// ------------------------------------------------------------------------------

/*!
  \brief Create the shape information for the created image

  \param nDistance  -- Number of steps along the distance axis
  \param nTime      -- Number of integration steps in time
  \param nFrequency -- Number of frequency channels
  
  \return shape -- The shape of the image, i.e. the number of axes and their
          respective lengths, e.g. <tt>shape=[25,25,10,50,128]</tt>
*/
IPosition image_shape (int nDistance=10,
		       int nTime=50,
		       int nFrequency=128)
{
  IPosition shape (5,25,25,nDistance,nTime,nFrequency);

  return shape;
}

// ------------------------------------------------------------------------------

/*!
  \brief Create the coordinate system attached to the image

  This function creates a coordinate system object 

  \param incrementDirection -- Increment (resolution) along the direction axes.
  \param incrementDistance  -- Increment along the distance axis.
  \param incrementTime      -- Increment along the frequency axis.
  
  \return cs -- Coordinate system tool
*/
casa::CoordinateSystem image_csys (double incrementDirection=2.0,
				   double incrementDistance=0.0,
				   double incrementTime=1.0)
{
  const double pi (3.14159265);
  casa::CoordinateSystem cs;

  // General observation information

  casa::ObsInfo obsInfo;
  casa::Time startTime;
  casa::Quantity epoch (startTime.modifiedJulianDay(), "d");
  //
  obsInfo.setObsDate (epoch);
  obsInfo.setTelescope ("LOFAR");
  obsInfo.setObserver ("Lars Baehren");
  //
  cs.setObsInfo(obsInfo);
  
  // [1] Direction axis
  {
    IPosition shape;
    Matrix<double> xform(2,2);
    
    shape            = image_shape();
    xform            = 0.0;
    xform.diagonal() = 1.0;
    
    casa::DirectionCoordinate coord (casa::MDirection::AZEL,
				     casa::Projection(casa::Projection::STG),
				     0.0*pi/180.0,
				     90.0*pi/180.0,
				     -incrementDirection*pi/180.0,
				     incrementDirection*pi/180.0,
				     xform,
				     0.5*shape(0),
				     0.5*shape(1));
    
    coord.setReferencePixel(Vector<double>(2,0.0));
    
    cs.addCoordinate(coord);
  }
  
  // [2] Distance axis
  {
    Vector<String> names (1,"Distance");
    Vector<String> units (1,"m");
    Vector<double> refVal (1,-1.0);
    Vector<double> inc (1,incrementDistance);
    Matrix<double> pc (1,1,1.0);
    Vector<double> refPix (1,0.0);
    
    casa::LinearCoordinate coord (names,
				  units,
				  refVal,
				  inc,
				  pc,
				  refPix);
    
    cs.addCoordinate(coord);
  }

  // [3] Time axis
  {
    Vector<String> names  (1,"Time");
    Vector<String> units  (1,"s");
    Vector<double> refVal (1,0.0);
    Vector<double> inc    (1,incrementTime);
    Matrix<double> pc     (1,1,1.0);
    Vector<double> refPix (1,0.0);
    
    casa::LinearCoordinate axis (names,
				 units,
				 refVal,
				 inc,
				 pc,
				 refPix);
    
    cs.addCoordinate(axis);
  }

  // [4] Frequency axis
  {
    Vector<String> names  (1,"Frequency");
    casa::SpectralCoordinate axis;
    
    axis.setWorldAxisNames(names);
    
    cs.addCoordinate(axis);
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

// ------------------------------------------------------------------------------

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
#endif

  return nofFailedTests;
}
