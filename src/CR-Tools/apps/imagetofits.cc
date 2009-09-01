/*-------------------------------------------------------------------------*
 | $Id:: templates.h 391 2007-06-13 09:25:11Z baehren                    $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2009                                                    *
 *   Sef Welles                                                            *
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
#include <Utilities/TestsCommon.h>

#include <images/Images/ImageFITSConverter.h>
#include <images/Images/HDF5Image.h>
#include <images/Images/PagedImage.h>
#include <images/Images/RebinImage.h>
//#include <casa/Arrays/Slicer.h>
using namespace casa;
using namespace std;

int main(int argc, char *argv[])
{
  string infile;
  string datatype;
  
  /*
    Check if filename of the dataset and the format are provided on the command line.
  */
  if (argc < 2) {
    cerr << "Usage: imagetofits <input.event> <datatype>\n"
	 << "Supported datatypes are 'hdf5' and 'paged'."
	 << endl;
    return 1;
  } else {
    infile = argv[1];
    if (argc > 2) {
      datatype = argv[2];
    };
  }



  
  String fitsName = "new.fits";
  String error;
  Bool ok;
  
  // Open the image and convert it to fits:
  if (datatype == "hdf5") {
    HDF5Image<Float> imageIn (infile);
    
    // Regridding of input file:
    IPosition factors (imageIn.shape());
    factors    = 1;
    factors(3) = 513;
    RebinImage<Float> rb(imageIn, factors);
    IPosition shapeOut = rb.shape();
    TiledShape tShapeOut(shapeOut);

    /* Summary of image properties */
    cout << "-- Summary of the original image:" << std::endl;
    CR::summary (imageIn);
    cout << "-- Summary of the image after rebinning:" << std::endl;
    CR::summary (rb);
    
/* 	const LELCoordinates* pLatCoord = &(rb.lelCoordinates());
    const LELImageCoord* pImCoord =
    dynamic_cast<const LELImageCoord*>(pLatCoord);
    CoordinateSystem coords = pImCoord->coordinates();
 */
 /*
	CoordinateSystem coords = rb.coordinates();
   	TempImage<Float> imageOut(tShapeOut, coords); //srb.coordinates());
   	LatticeUtilities::copyDataAndMask(os, imageOut, rb);
   	ImageUtilities::copyMiscellaneous (imageOut, imageIn);
  */
  cout<<"Hier gaat het nog goed...1"<<endl;		
  	ok = casa::ImageFITSConverter::ImageToFITS(error, rb, fitsName);

  } else if (datatype == "paged") {
    PagedImage<Float> imageIn (infile);
	ok = casa::ImageFITSConverter::ImageToFITS(error, imageIn, fitsName);

  } else {
    cerr << "Usage: imagetofits <input.event> <datatype>\n"
	 << "Supported datatypes are 'hdf5' and 'paged'."
	 << endl;
  }
  

  
  return 0;
}
