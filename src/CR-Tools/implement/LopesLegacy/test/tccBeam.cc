/***************************************************************************
 *   Copyright (C) 2005                                                    *
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

/* $Id$*/

// Standard library
#include <fstream>
// AIPS++/CASA
#include <scimath/Mathematics.h>
#include <scimath/Mathematics/FFTServer.h>
// LOPES-Tools
#include <Beamforming/ccBeam.h>

/*!
  \file tccBeam.cc

  \brief A collection of test routines for ccBeam
 
  \author Lars B&auml;hren
 
  \date 2005/12/09

  <h3>Usage</h3>

  After running the test program you should have two new files in your directory

  - ccbeam-real.data
  - ccbeam-complex.data

  Both should be identical, so check this via
  \verbatim
  diff ccbeam-real.data ccbeam-complex.data
  \endverbatim
  There might be minor differences due to rounding errors, e.g.
  \verbatim
  346c346
  < 345   9357.77
  ---
  > 345   9357.78
  918c918
  < 917   9941.47
  ---
  > 917   9941.48
  \endverbatim
  but this is nothing to worry about here.
*/

// ----------------------------------------------------------------- ccbeam2ascii

/*!
  \brief Export the cc-beam data vector to an ASCII table

  \param filename -- Name of the created data file.
  \param cc       -- Vector containing the values of the cc-beam.
*/
void ccbeam2ascii (const String& filename,
		   const Vector<Float>& cc)
{
  int nelem (cc.nelements());

  ofstream outfile;
  outfile.open(filename.c_str(), ios::out);
  
  for (int n(0); n<nelem; n++) {
    outfile << n << "\t" << cc(n) << endl;
  }

  outfile.close();
}

// ------------------------------------------------------------------ test_ccBeam

int test_ccBeam ()
{
  int nofFailedTests (0);

  try {
    ccBeam<Float,Complex> cc;
  } catch (AipsError x) {
    cerr << x.getMesg() << endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}

// -------------------------------------------------------------------- test_beam

/*!
  \brief Test constructors for a new ccBeam object

  \return nofFailedTests -- The number of failed tests.
*/
int test_beam ()
{
  int nofFailedTests (0);
  
  int nofAntennas (10);
  int blocksize (2048);
  Matrix<Float> data (blocksize,nofAntennas);
 
  indgen (data);

  // Test 1: cc-Beam from real-valued input data
  cout << "[1] cc-Beam from real-valued input data" << endl;
  try {
//     Vector<Float> cc (ccBeam<Float,Complex>::ccbeam(data));
//     //
//     ccbeam2ascii ("ccbeam-real.data",cc);
  } catch (AipsError x) {
    cerr << x.getMesg() << endl;
    nofFailedTests++;
  }

  // Test 2: cc-Beam from complex-valued input data
  cout << "[2] cc-Beam from complex-valued input data" << endl;
  try {
    // FFT the data matrix; feeding the Fourier transformed data into the ccBeam
    // should result in the same values, as the ccBeam will perform the inverse
    // FFT first internally.
    FFTServer<Float,Complex> server(IPosition(1,blocksize),
				    FFTEnums::REALTOCOMPLEX);
    int fftLength ((blocksize/2)+1);
    Vector<Complex> tmp;
    Matrix<Complex> fft (fftLength,nofAntennas);

    for (int ant(0); ant<nofAntennas; ant++) {
      server.fft (tmp,data.column(ant));
      fft.column(ant) = tmp;
    }

    // feed the complex-valued data into the ccBeam
//     Vector<Float> cc (ccBeam<Float,Complex>::ccbeam(fft,blocksize));
//     //
//     ccbeam2ascii ("ccbeam-complex.data",cc);    
  } catch (AipsError x) {
    cerr << x.getMesg() << endl;
    nofFailedTests++;
  }

  return nofFailedTests;
}

// -----------------------------------------------------------------------------

int main ()
{
  int nofFailedTests (0);

  // Test for the constructor(s)
  {
    nofFailedTests += test_ccBeam ();
  }

  return nofFailedTests;
}
