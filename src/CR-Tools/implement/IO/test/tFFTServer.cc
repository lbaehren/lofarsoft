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

/* $Id: tFFTServer.cc,v 1.4 2006/10/31 20:46:45 bahren Exp $*/

#include <stdlib.h>

// AIPS++/CASA
#include <casa/aips.h>
#include <casa/Arrays.h>
#include <casa/Arrays/Array.h>
#include <casa/Arrays/ArrayIO.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/Arrays/Cube.h>
#include <casa/Arrays/IPosition.h>
#include <casa/Arrays/Vector.h>
#include <casa/BasicSL/Complex.h>
#include <fits/FITS/BasicFITS.h>
#include <scimath/Mathematics.h>
#include <scimath/Mathematics/FFTServer.h>

#include <Data/ITS_Capture.h>
#include <IO/DataReader.h>

#include <casa/namespace.h>

/*!
  \file tFFTServer.cc
  
  \brief A number of tests for the usage of the FFT Server.
  
  \author Lars B&auml;hren
  
  \date 2005/12/06
*/

// -------------------------------------------------------------- infile_location

std::string get_filenameData ()
{
  bool ok (true);
  std::string filename ("");
  std::vector<std::string> filenames;

  filenames.push_back("/data/ITS/2005.08/lightning_20050831_1/experiment.meta");
  filenames.push_back("/data/ITS/2005.11/jupiter_20051130_01/experiment.meta");
  filenames.push_back("/data/ITS/2005.11/jupiter_20051130_02/experiment.meta");

  for (uint n(0); filenames.size(); n++) {
    //
    std::cout << " --> checking file " << filenames[n] << " ..." << std::endl;
    // try to open the file
    FILE *fd = fopen(filenames[n].c_str(),"r");
    if (fd == NULL) {
      ok = false;
      std::cerr << " ---> Failed to open file!" << std::endl;
    } else {
      filename = filenames[n];
      // report success
      std::cout << " ---> Success opening file." << std::endl;
      // return the result
      return filename;
    }
  }
  
  return filename;
}

// ------------------------------------------------------------------ test_invers

/*!
  \brief Test usage of the invers FFT to reproduce the input data.

  \param filenames -- Set of files from which the data are read.
  \param blocksize -- Size of an individual block of data (in samples).

  \return nofFailedTests -- The number of failed tests.
*/
int test_invers (String const &filename,
		 const Int& blocksize)
{
  int nofFailedTests (0);
  
  DataReader *dr;
  ITS_Capture *capture = new ITS_Capture (filename, blocksize);
  dr = capture;
  
  uInt nofFiles (dr->nofSelectedAntennas());
  Matrix<Double> voltage (dr->voltage());
  Matrix<DComplex> fft (dr->fft());
  Matrix<Double> voltageReconstruction (blocksize,nofFiles);
  
  FFTServer<Double,DComplex> server(IPosition(1,blocksize),
				    FFTEnums::REALTOCOMPLEX);
  
  cout << "[1] Start from the FFT'ed data ..." << endl;
  try {
    Vector<Double> tmp;
    
    for (uInt n(0); n<nofFiles; n++) {
      // FFT the data block for the current antenna ...
      server.fft(tmp,fft.column(n));
      // .. and copy the result
      voltageReconstruction.column(n) = tmp;
    }
    
    cout << " - sum(input) = " << sum(voltage) << endl;
    cout << " - sum(recon) = " << sum(voltageReconstruction) << endl;
  } catch (AipsError x) {
    cerr << x.getMesg() << endl;
    nofFailedTests++;
  }
  
  cout << "[2] Start from the initial raw data ..." << endl;
  try {
    Matrix<DComplex> tmp;
    
    server.fft(tmp,voltage);
    
    cout << " - sum(fft)          : " << sum(fft) << "  " << fft.shape() << endl;
    cout << " - sum(FFT(voltage)) : " << sum(tmp) << "  " << tmp.shape() << endl;
  } catch (AipsError x) {
    cerr << x.getMesg() << endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}

// ----------------------------------------------------------------- main routine

int main ()
{
  int nofFailedTests (0);
  
  Int blocksize (8192);
  String filename = get_filenameData ();
  
  if (filename != "") {
    nofFailedTests += test_invers(filename,blocksize);
  } else {
    nofFailedTests++;
  }

  return nofFailedTests;
}
