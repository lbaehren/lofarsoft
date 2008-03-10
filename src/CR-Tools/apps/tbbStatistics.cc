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
  long naxis            = shape.nelements();
  long nelements        = spectrum.nelements();
  long naxes[2]         = { shape(1), shape(0)};
  float pixels[shape(0)][shape(1)];

  return true;
}

// -----------------------------------------------------------------------------

/*!
  \brief Create dynamic spectrum from channel data

  \param filename  -- Name of the dataset from which to read in the data
  \param blocksize -- Size of a single block of data, [samples].
  \param nofBlocks -- Number of blocks to process
  
  \return nofFails --
*/
int dynamic_spectrum (std::string const &filename,
		      uint const &blocksize=1024,
		      uint const &nofBlocks=10)
{
  int nofFails (0);

  // open file into TBB dataset
  CR::LOFAR_TBB dataset (filename);
  dataset.summary();

  return nofFails;
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
  nofFails += dynamic_spectrum (filename);

  return nofFails;
}
