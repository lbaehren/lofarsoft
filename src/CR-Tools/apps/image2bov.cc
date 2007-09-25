/*-------------------------------------------------------------------------*
 | $Id                                                                   $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2007                                                    *
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

/*!
  \file image2bov.cc
  
  \ingroup CR_Applications
  
  \brief Export pixel values of an AIPS++ image as brick of values (BOV)
  
  \author Lars B&auml;hren
  
  \date 2007/09/21

  <h3>Motivation</h3>

  If we want to to something more but 2-dimensional visualization of our
  multidimensional images, we need to used tools like <i>VAPOR</i> or
  <i>VisIt</i>. The problem however is that now of the visualization tools
  are able to work with our image data directly, so we need to go through some
  conversion step first.

  <h3>Prerequisites</h3>

  In order to visualize some 2D data in spherical coordinates:
  <ol>
    <li>read file
    <li>create a Pseudocolor plot
    <li>add a Transform operator
    <li>display the Transform operator attributes window
    <li>select the Coordinate tab of that window
    <li>select the Spherical radio button of the Input coordinates region
    <li>select the Cartesian radio button of the Output coordinates region
    <li>click the window's Apply button
  </ol>

  <h3>Example</h3>

  <ul>
    <li>Image filename : 2006.01.25-22:54:05.000-AZEL-SIN.img
    <li>Maximum
    <ul>
      <li>Distance : 4000 m = Slice 13
      <li>Time : 1.8 mus = Slice 64
    </ul>
  </ul>
*/

#include <fstream>
#include <iostream>
#include <string>

#include <casa/aips.h>
#include <casa/Arrays/Array.h>
#include <casa/Arrays/IPosition.h>
#include <casa/Arrays/Slicer.h>
#include <casa/Arrays/Vector.h>
#include <casa/System/ProgressMeter.h>
#include <coordinates/Coordinates/Coordinate.h>
#include <coordinates/Coordinates/CoordinateSystem.h>
#include <coordinates/Coordinates/LinearCoordinate.h>
#include <coordinates/Coordinates/SpectralCoordinate.h>
#include <images/Images/PagedImage.h>

using std::cerr;
using std::cout;
using std::endl;
using std::string;

using casa::Array;
using casa::Coordinate;
using casa::CoordinateSystem;
using casa::IPosition;
using casa::LinearCoordinate;
using casa::PagedImage;
using casa::ProgressMeter;
using casa::Slicer;
using casa::SpectralCoordinate;
using casa::Vector;

// ==============================================================================
//
//  Global variables
//
// ==============================================================================

/*
  Mapping of the image axes:
  
  [az,el,r] = [0,1,2]  -->  [r,az,el] = [2,0,1]
*/
const int axisRadius (2);
const int axisAzimuth (0);
const int axisElevation (1);

// ==============================================================================
// 
//  Function prototypes
//
// ==============================================================================

/*!
  \brief Provide a short summary of the loaded paged image
  
  \return status -- Returns <i>true</i> if all the operations to obtain a
                    summary of the image went ok; otherwise <i>false</i> is
		    returned.
*/
bool image_summary (casa::PagedImage<casa::Float> const &image);

/*!
  \brief Get the names of the coordinate types used for the image axes

  \param image -- 

  \return types -- 
*/
casa::Vector<string> coordinate_types (casa::CoordinateSystem const &cs);

/*!
  \brief Get the world values along the time axis

  As programs like VisIt store data as file per time-step, we need to properly
  tag the generated bricks of values, to line them up later on. As the timestamps
  can be extracted from the coordinate system associated with the image, we
  extract this coordinate system first, retrieve the linear cooordinate and 
  then perform a pixel-to-world transformation.

  If the coordinate system associated with the image is constructed properly,
  one could retrieve the time axis (LinearCordinate) and obtain the values via
  the <tt>toWorld()</tt> method; however the coordinate system object might
  not contain proper information about the coordinates -- even thought the
  underlying WCS parameters are correct. Because of this the savest way to 
  proceed is to explicitely carry out the conversion based on the CRVAL and
  CDELT keywords for the coordinate axis in question.

  \param image -- Paged AIPS++ image

  \return timeValues -- Vector with the world values associated with the
                        time coordinate axis, [sec].
*/
Vector<casa::Double> time_axis_values (casa::PagedImage<casa::Float> &image);

/*!
  \brief Export the AIPS++ image pixel value to a brick of values

  Keep in mind that we need to perform some proper lining up of the image
  coordinates; as the original AIPS++ image uses [az,el,radius,time,freq]
  we need to rearrange the first three axes to obtain a proper reproduction
  of the spatial brightness distribution.

  \param image -- Paged AIPS++ image

  \return status -- Status of the operation; returns <i>false</i> if an error
                    was encountered.
*/
bool export_to_brick_of_values (casa::PagedImage<casa::Float> &image);

/*!
  \brief Write the header file describing the exported data

  \param datafile   -- Name of the dta file, to which the pixel values are 
                       written.
  \param timestep   -- Counter for the time step
  \param timeValues -- World coordinate values along the time axis of the image
  \param shape      -- Shape of the image pixel array

  \return status --
*/
bool write_header_file (char *datafile,
			int const &timestep,
			Vector<casa::Double> const &timeValues,
			IPosition const &shape);

// ==============================================================================
//
//  Implementation
//
// ==============================================================================

bool image_summary (casa::PagedImage<casa::Float> const &image)
{
  bool status (true);

  CoordinateSystem cs = image.coordinates();

  try {
    // characteristics of the image
    cout << " -- Image type            : " << image.imageType()    << endl;
    cout << " -- Is image paged?       : " << image.isPaged()      << endl;
    cout << " -- Is image persistent?  : " << image.isPersistent() << endl;
    cout << " -- Does image have mask? : " << image.hasPixelMask() << endl;
    cout << " -- Current table name    : " << image.name(true)     << endl;
    cout << " -- Shape of pixel array  : " << image.shape()        << endl;
    // characteristics of the coordinate system
    cout << " -- nof. coordinates      : " << cs.nCoordinates()    << endl;
    cout << " -- Coordinate types      : " << coordinate_types(cs) << endl;
    cout << " -- World axis names      : " << cs.worldAxisNames()  << endl;
    cout << " -- World axis units      : " << cs.worldAxisUnits()  << endl;
    cout << " -- Reference value       : " << cs.referenceValue()  << endl;
    cout << " -- World axis increment  : " << cs.increment()       << endl;
  } catch (std::string message) {
    cerr << "[image2bov] " << message << endl;
    status = false;
  }
  
  return status;
}

// ------------------------------------------------------------- coordinate_types

casa::Vector<string> coordinate_types (casa::CoordinateSystem const &cs)
{
  unsigned int nofCoordinates = cs.nCoordinates();
  Vector<string> names (nofCoordinates);

  for (unsigned int n(0); n<nofCoordinates; n++) {
    names(n) = Coordinate::typeToString(cs.type(n));
  }
  
  return names;
}

// ------------------------------------------------------------- time_axis_values

Vector<casa::Double> time_axis_values (casa::PagedImage<casa::Float> &image)
{
  unsigned int axisID (3);
  IPosition shape (image.shape());
  Vector<casa::Double> timeValues (shape(axisID),0.);
  CoordinateSystem cs = image.coordinates();

  // Extract refrence value and coordinate increment
  Vector<casa::Double> refValue = cs.referenceValue();
  Vector<casa::Double> incr = cs.increment();
  
  for (unsigned int n(0); n<shape(axisID); n++) {
    timeValues(n) = refValue(axisID) + n*incr(axisID);
  }

  return timeValues;
}

// ------------------------------------------------------------ write_header_file

bool write_header_file (char *datafile,
			int const &timestep,
			Vector<casa::Double> const &timeValues,
			IPosition const &shape)
{
  bool status (true);
  char headerfile[50];

  // set the name of the header file
  sprintf (headerfile,"file%04d.bov",timestep);
  
  // open stream for header file
  std::ofstream os (headerfile,std::ios::out);

  os << "TIME: " << timeValues(timestep) << endl;
  os << "DATA_FILE: " << datafile        << endl;
  os << "DATA_FORMAT: FLOAT"             << endl;
  os << "VARIABLE: Radio_Brightness"     << endl;
  os << "DATA_ENDIAN: LITTLE"            << endl;
  os << "DATA_SIZE: " << shape(axisRadius)
     << " " << shape(axisAzimuth)
     << " " << shape(axisElevation) << endl;
  os << "CENTERING: zonal"               << endl;
  os << "BRICK_ORIGIN: 0. 0. 0."         << endl;
  os << "BRICK_SIZE: 1. 1. 1."           << endl;

  os.close();

  return status;
}

// ---------------------------------------------------- export_to_brick_of_values

bool export_to_brick_of_values (casa::PagedImage<casa::Float> &image)
{
  bool status (true);
  IPosition shape (image.shape());
  IPosition start (shape);
  IPosition length (shape);
  IPosition stride (shape);
  char datafile[50];

  start     = 0;
  length(3) = 1;
  stride    = 1;

  // Extract the values along the time axis
  Vector<casa::Double> timeValues = time_axis_values (image);

  // set up the slice taken from the AIPS++ image

  IPosition shapeSlice (5,shape(0),shape(1),shape(2),1,1);
  IPosition slicePosition (5,0);
  Array<casa::Float> imageSlice (shapeSlice);

  // additional variables used during export
  int az (0);
  int el (0);
  int radius (0);
  float buffer[shape(1)][shape(0)][shape(2)];
  int nofPixels (shape(axisRadius)*shape(axisAzimuth)*shape(axisElevation));

  ProgressMeter meter (0,shape(3),"Title","Subtitle","","");

  for (int timestep(0); timestep<shape(3); timestep++) {
    start(3) = timestep;
    Slicer slicer (start, length, stride);
    //
    status = image.doGetSlice (imageSlice,slicer);
    // copy the values from the AIPS++ array to a standard C/C++ array
    // such that we can dump them onto disk
    for (az=0; az<shape(axisAzimuth); az++) {
      slicePosition(axisAzimuth) = az;
      for (el=0; el<shape(axisElevation); el++) {
	slicePosition(axisElevation) = el;
	for (radius=0; radius<shape(axisRadius); radius++) {
	  slicePosition(axisRadius) = radius;
	  buffer[el][az][radius] = imageSlice (slicePosition);
	}
      }
    }
    // once we have extracted the 3D volume for a timestep, we write this to
    // disk
    sprintf (datafile,"file%04d.dat",timestep);
    FILE *fp = fopen (datafile,"wb");
    fwrite((void *)buffer,sizeof(float),nofPixels, fp);
    fclose (fp);
    // create header files to describe the data previously exported
    status = write_header_file (datafile,
				timestep,
				timeValues,
				shape);
    //
    meter.update(timestep);
  }

  return status;
}

// ---------------------------------------------------------------- main function

int main (int argc,
	  char *argv[])
{
  int nofErrors (0);
  bool status (true);
  
  // parameter check -----------------------------
  
  if (argc < 2) {
    cout << "[image2bov] Too few parameters!" << endl;
    cout << "\n"                              << endl;
    cout << " USAGE:  image2bov <image file>" << endl;
    cout << "\n"                              << endl;
    return -1;
  }
  
  string infile = argv[1];

  // try to open the image file ------------------

  casa::PagedImage<casa::Float> image (infile);
  IPosition shape;

  if (image_summary (image)) {
    status = export_to_brick_of_values (image);
  }

  return nofErrors;
}
