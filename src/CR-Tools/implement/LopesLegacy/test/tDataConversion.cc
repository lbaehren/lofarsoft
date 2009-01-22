/***************************************************************************
 *   Copyright (C) 2005                                                    *
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

/* $Id$ */

#include <LopesLegacy/DataConversion.h>

/*!
  \file tDataConversion.cc
  
  \brief A collection of tests for DataConversion
  
  \author Lars B&aumll;hren
  
  \data 2005/05
*/

using std::cerr;
using std::cout;
using std::endl;
using casa::AipsError;
using CR::DataConversion;

// =============================================================================

/*!
  \brief Test construction of DataConversion object

  \param nofAntennas -- Number of antennas.
  \param blocksize   -- Blocksize by which data are read; this is the input 
                        size of the FFT.
  \param fftlength   -- Output length of the FFT.

  \return ok -- Status of the test routine.
*/
bool test_DataConversion (const int nofAntennas,
			  const int blocksize,
			  const int fftlength)
{
  bool ok (true);

  cout << "[1]" << endl;
  try {
    DataConversion conv (nofAntennas,blocksize);
  } catch (AipsError x) {
    cerr << "[tDataConversion] " << x.getMesg() << endl;
    ok = false;
  }

  cout << "[2]" << endl;
  try {
    DataConversion conv (nofAntennas,blocksize,fftlength);
  } catch (AipsError x) {
    cerr << "[tDataConversion] " << x.getMesg() << endl;
    ok = false;
  }

  return ok;
}

// =============================================================================


int main () {

  bool ok (true);
  int nofAntennas (60);
  int blocksize (65536);
  int fftlength (32769);

  {
    ok = test_DataConversion (nofAntennas,blocksize,fftlength);
  }

  return 0;
}
