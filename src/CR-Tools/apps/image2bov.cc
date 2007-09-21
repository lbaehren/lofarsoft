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
  <i>VisIt</i>. The problem however is that now of thhe visualization tools
  are able to work with our image data directly, so we need to go through some
  conversion step first.

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

#include <iostream>
#include <string>
#include <vector>

#include <casa/aips.h>
#include <casa/Arrays/Array.h>
#include <casa/Arrays/IPosition.h>
#include <casa/Arrays/Slicer.h>
#include <casa/System/ProgressMeter.h>
#include <images/Images/PagedImage.h>

using std::cerr;
using std::cout;
using std::endl;
using std::string;

using casa::Array;
using casa::IPosition;
using casa::PagedImage;
using casa::ProgressMeter;
using casa::Slicer;

/*!
  \brief Provide a short summary of the loaded paged image
  
  \return status -- Returns <i>true</i> if all the operations to obtain a
                    summary of the image went ok; otherwise <i>false</i> is
		    returned.
*/
bool image_summary (casa::PagedImage<casa::Float> const &image)
{
  bool status (true);

  try {
    cout << " -- Image type            : " << image.imageType()    << endl;
    cout << " -- Is image paged?       : " << image.isPaged()      << endl;
    cout << " -- Is image persistent?  : " << image.isPersistent() << endl;
    cout << " -- Does image have mask? : " << image.hasPixelMask() << endl;
    cout << " -- Current table name    : " << image.name(true)     << endl;
    cout << " -- Shape of pixel array  : " << image.shape()        << endl;
  } catch (std::string message) {
    cerr << "[image2bov] " << message << endl;
    status = false;
  }
  
  return status;
}

// ---------------------------------------------------- export_to_brick_of_values

/*!
  \brief Export the AIPS++ image pixel value to a brick of values

  \param image -- Paged AIPS++ image

  \return status --
*/
bool export_to_brick_of_values (casa::PagedImage<casa::Float> &image)
{
  bool status (true);
  IPosition shape (image.shape());
  IPosition start (shape);
  IPosition length (shape);
  IPosition stride (shape);
  char outfileName[50];

  start     = 0;
  length(3) = 1;
  stride    = 1;

  // set up the slice taken from the AIPS++ image

  IPosition shapeSlice (5,shape(0),shape(1),shape(2),1,1);
  IPosition slicePosition (5,0);
  Array<casa::Float> imageSlice (shapeSlice);

  // additional variables used during export
  int az (0);
  int el (0);
  int radius (0);
  float buffer[shape(2)][shape(1)][shape(0)];
  int nofPixels (shape(0)*shape(1)*shape(2));

  ProgressMeter meter (0,shape(3),"Title","Subtitle","","");

  for (int timestep(0); timestep<shape(3); timestep++) {
    start(3) = timestep;
    Slicer slicer (start, length, stride);
    //
    status = image.doGetSlice (imageSlice,slicer);
    // copy the values from the AIPS++ array to a standard C/C++ array
    // such that we can dump them onto disk
    for (az=0; az<shape(0); az++) {
      slicePosition(0) = az;
      for (el=0; el<shape(1); el++) {
	slicePosition(1) = el;
	for (radius=0; radius<shape(2); radius++) {
	  slicePosition(2) = radius;
	  buffer[radius][el][az] = imageSlice (slicePosition);
	}
      }
    }
    // once we have extracted the 3D volume for a timestep, we write this to
    // disk
    sprintf (outfileName,"file%04d.dat",timestep);
    FILE *fp = fopen (outfileName,"wb");
    fwrite((void *)buffer,sizeof(float),nofPixels, fp);
    fclose (fp);
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
