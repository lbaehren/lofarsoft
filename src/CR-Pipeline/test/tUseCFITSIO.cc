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

#include <iostream>
#include <string>
#include <cmath>

#include <fitsio.h>

/*!
  \file tUseCFITSIO.cc

  \ingroup CR-Pipeline

  \brief A simple test for external build against libcfitsio

  \author Lars B&auml;hren

  \date 2007/01/29

  Check if we can properly build against the CFITSIO library, as required for
  reading/writing FITS files.
*/

/*!
  \brief main routine
  
  \return nofFailedTests -- The number of failed tests
*/
int main () 
{
  int nofFailedTests (0);
  int status (0);
  int nTime(10);
  int nFreq(20);
  long naxis (2);
  long naxes[2] = {nTime,nFreq};
  long nelements (nTime*nFreq);
  long fpixel (1);
  float pixels[nFreq][nTime];  // array shape: [Freq,Time]

  std::cout << " - naxis     = " << naxis << std::endl;
  std::cout << " - naxes     = " << naxes[0] << "," << naxes[1] << std::endl;
  std::cout << " - nelements = " << nelements << std::endl;
  
  // fill the pixel array
  try {
    for (int i(0); i<nTime; i++) {
      for (int j(0); j<nFreq; j++) {
	pixels[j][i] = 1.0*(i+j);
      }
    }
  } catch (std::string message) {
    std::cerr << "Error during filling of pixel array. " << message << std::endl;
    nofFailedTests++;
  }

  std::cout << "[1] Create new file with primary array image" << std::endl;
  try {
    // Create a new file
    fitsfile *fptr;
    fits_create_file (&fptr, "!testimage1.fits", &status);
    // Create the primary array image (16-bit short integer pixels)
    fits_create_img (fptr, FLOAT_IMG, naxis, naxes, &status);
    // Close the file
    fits_close_file(fptr, &status);
    status = 0;
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  std::cout << "[2] Write the array of integers to the image" << std::endl;
  try {
    // Create a new file
    fitsfile *fptr;
    fits_create_file (&fptr, "!testimage2.fits", &status);
    // Create the primary array image (16-bit short integer pixels)
    fits_create_img (fptr, FLOAT_IMG, naxis, naxes, &status);
    // Write the array of integers to the image
    fits_write_img(fptr, TFLOAT, fpixel, nelements, pixels[0], &status);
    // Close the file
    fits_close_file(fptr, &status);
    status = 0;
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }

  std::cout << "[3] Write the array of integers to the image" << std::endl;
  try {
    fitsfile *fptr;
    long exposure;
    short array[200][300];
    
    naxes[0]  = 300;
    naxes[1]  = 200;
    nelements = naxes[0] * naxes[1];
    status = 0;
    fpixel = 1;

    /* create new file */
    fits_create_file(&fptr, "!testimage3.fits", &status);
    
    /* Create the primary array image (16-bit short integer pixels */
    fits_create_img(fptr, SHORT_IMG, naxis, naxes, &status);
    
    /* Write a keyword; must pass the ADDRESS of the value */
    exposure = 1500;
    fits_update_key(fptr,TLONG,"EXPOSURE",&exposure,"Total Exposure Time", &status);
    
    /* Initialize the values in the image with a linear ramp function */
    for (nFreq = 0; nFreq < naxes[1]; nFreq++)
      for (nTime = 0; nTime < naxes[0]; nTime++)
	array[nFreq][nTime] = nTime + nFreq;
    
    /* Write the array of integers to the image */
    fits_write_img(fptr, TSHORT, fpixel, nelements, array[0], &status);
    
    // close the file
    fits_close_file(fptr, &status);
    
    // print out any error messages
    fits_report_error(stderr, status);
  } catch (std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}
