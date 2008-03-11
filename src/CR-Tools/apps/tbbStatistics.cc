/*-------------------------------------------------------------------------*
 | $Id:: tNewClass.cc 1159 2007-12-21 15:40:14Z baehren                  $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2008                                                    *
 *   Lars B"ahren (lbaehren@gmail.com)                                     *
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

#include <Data/LOFAR_TBB.h>

using std::cerr;
using std::cout;
using std::endl;

/*!
  \file tbbStatistics.cc

  \ingroup CR_Applications

  \brief Create a number of statistical number for TBB time-series data
 
  \author Lars B&auml;hren
 
  \date 2008/03/10
*/


// -----------------------------------------------------------------------------

/*!
  \brief Export the dynamic spectrum to a FITS file for later display

  \param spectrum -- [freq,time] Array with the generated dynamic spectrum.
  \param outfile  -- Name of the output FITS file to which the dynamic spectrum
         is written.

  \return status -- Status of the operation; returns <tt>false</tt> in case an 
          error was encountered.
*/
bool export2fits (casa::Matrix<double> const &spectrum,
		  std::string const &outfile="dynamicspectrum.fits")
{
  casa::IPosition shape = spectrum.shape();
  int status            = 0;
  long fpixel           = 1;
  long naxis            = shape.nelements();
  long nelements        = spectrum.nelements();
  long naxes[2]         = { shape(1), shape(0)};
  float pixels[shape(0)][shape(1)];
  std::string filename;
  fitsfile *fptr;

  /* Provide some basic feedback before starting export of data */
  cout << "[tbbStatistics::export2fits]"               << endl;
  cout << "-- Output file       = " << outfile          << endl;
  cout << "-- Dynamic spectrum  = " << spectrum.shape() << endl;
  cout << "-- nof. image pixels = " << nelements        << endl;

  /* Adjust the filename such that existing data are overwritten */
  filename = "!" + outfile;
  
  cout << "--> creating new FITS file ..." << endl;
  fits_create_file (&fptr, filename.c_str(), &status);
  
  cout << "--> creating primary image array ..." << endl;
  fits_create_img (fptr, FLOAT_IMG, naxis, naxes, &status);
  
  cout << "--> setting up array with image pixel data ..." << endl;
  try {
    // additional local variables
    uint timestep = 0;
    uint channel  = 0;
    // copy values
    for (timestep=0; timestep<shape(1); timestep++) {
      for (channel=0; channel<shape(0); channel++) {
	pixels[channel][timestep] = spectrum (channel,timestep);
      }
    }
  } catch (std::string message) {
    std::cerr << "[tbbStatistics::export2fits] Error copying pixel values!"
	      << endl;
  }
  
  cout << "--> writing pixel values to file ..." << endl;
  try {
    fits_write_img(fptr, TFLOAT, fpixel, nelements, pixels[0], &status);
  } catch (std::string message) {
    std::cerr << "[tbbStatistics::export2fits] Error writing pixel values!"
	      << endl;
  }
  
  return true;
}

// -----------------------------------------------------------------------------

/*!
  \brief Create dynamic spectrum from channel data

  \param filename  -- Name of the dataset from which to read in the data
  \param blocksize -- Size of a single block of data, [samples].
  \param nofBlocks -- Number of blocks to process
  
  \return status -- Status of the operation; returns <tt>false</tt> in case an 
          error was encountered.
*/
int dynamic_spectrum (std::string const &filename,
		      uint const &blocksize=1024,
		      uint const &nofBlocks=20)
{
  bool status  = true;

  // open file into TBB dataset
  CR::LOFAR_TBB dataset (filename,blocksize);
  dataset.summary();

  uint block   = 0;
  uint channel = 0;
  uint nofDipoles  = dataset.nofDipoleDatasets();
  uint nofChannels = dataset.fftLength();
  casa::Matrix<casa::DComplex> spectra (nofChannels,
					nofDipoles);
  casa::Matrix<double> dynamicSpectrum (nofChannels,
					nofBlocks);

  /* go through the data to generate a dynamic spectrum */

  try {
    for (block=0; block<nofBlocks; block++) {
      // read one block of data
      spectra = dataset.fft();
      // process the retrieved data and add them to the dynamic spectrum
      for (channel=0; channel<nofChannels; channel++) {
	dynamicSpectrum (channel,block) = real(sum(abs(spectra.row(channel))));
      }
      // increment pointer to start position
      dataset.nextBlock();
    }
  } catch (std::string message) {
    std::cerr << "[dynamic_spectrum] " << message << endl;
    status = false;
  }

  /* export of the dynamic spectrum to a FITS file */
  if (status) {
    return export2fits (dynamicSpectrum);
  }

  return status;
}

// -----------------------------------------------------------------------------

int main (int argc,
	  char *argv[])
{
  int nofFails (0);

  if (argc < 2) {
    cerr << "[tbbStatistics] Too few parameters!" << endl;
    cerr << "" << endl;
    cerr << "  tbbStatistics <filename>" << endl;
    cerr << "" << endl;
    return -1;
  }

  std::string filename (argv[1]);

  // Create dynamic spectrum
  if (!dynamic_spectrum (filename)) {
    nofFails++;
  }

  return nofFails;
}
