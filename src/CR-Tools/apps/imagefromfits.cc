/*-------------------------------------------------------------------------*
 | $Id:: templates.h 391 2007-06-13 09:25:11Z baehren                    $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2005                                                    *
 *   Sven Lafebre ()                                                       *
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

#include <casa/aips.h>
#include <casa/iostream.h>
// #include <casa/Inputs/Input.h>
// #include <casa/OS/File.h>
// #include <casa/OS/HostInfo.h>
// #include <casa/OS/Path.h>
#include <images/Images/PagedImage.h>
#include <images/Images/ImageFITSConverter.h>
#include <fits/FITS/BasicFITS.h>

#include <casa/namespace.h>

/*!
  \file imagefromfits.cc

  \brief Convert FITS image to AIPS++ image

  \ingroup CR_Applications

  \author Lars B&auml;hren

  \date 2007/08/28

  <h3>Synopsis</h3>

  This program is a modified version of the <i>fits2image</i> test program 
  distributed as of CASA/casacore.

  A FITS to image conversion may be accomplished as follows:
  \code
  PagedImage<Float> *image = 0;
  String fitsName = "exists.fits";
  String imageName = "new.image";
  String error;
  Bool ok = ImageFITSConverter::FITSToImage(image, error, imageName, fitsName);
  if (!image) .\.. error .\..
  \endcode
  
  <h3>Usage</h3>

  \verbatim
  imagefromfits <FITS file> <Image file>
  \endverbatim
 */
int main (int argc,
	  char **argv)
{
  /* Check the number of input parameters */
  if (argc < 2) {
    std::cerr << "[imagefromfits] Too few parameters" << std::endl;
    std::cerr << "" << std::endl;
    std::cerr << "USAGE: imagefromfits <FITS file> <Image file>" << std::endl;
    std::cerr << "" << std::endl;
    return -1;
  }

  /* Variables required for conversion routines */

  casa::String infile  = argv[1];
  casa::String outfile = argv[2];
  casa::ImageInterface<casa::Float>* pOutImage;
  casa::Bool ok (false);
  casa::String error;
  
  try {
    ok = ImageFITSConverter::FITSToImage (pOutImage,
					  error,
					  outfile,
					  infile);
  } catch (std::string message) {
    std::cerr << "[imagefromfits] " << message << std::endl;
    return 1;
  }
  
  /* Clean up memory */
  delete pOutImage;

  return 0;
}

