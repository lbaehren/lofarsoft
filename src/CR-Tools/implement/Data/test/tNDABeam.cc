/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2006                                                    *
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

#include <casa/aips.h>
#include <casa/Exceptions/Error.h>

#include <Data/NDABeam.h>

/*!
  \file tNDABeam.cc

  \ingroup CR_Data

  \brief A collection of test routines for NDABeam
 
  \author Lars B&auml;hren
 
  \date 2006/08/04

  <h3>Usage</h3>

  The name of the metafile, on which the test program is run, can be provided
  as command-line parameter, e.g.
  \verbatim
  tNDABeam /data/NDA/jupX3/experiment.meta
  \endverbatim

  <h3>Output</h3>

*/

// -----------------------------------------------------------------------------

void show_parameters (NDABeam &data)
{
  cout << " - Metafile               = " << data.metafile()            << endl;
  cout << " - blocksize              = " << data.blocksize()           << endl;
  cout << " - FFT length             = " << data.fftLength()           << endl;
  cout << " - nof. streams           = " << data.nofStreams()          << endl;
  cout << " - shape(adc2voltage)     = " << data.ADC2Voltage().shape() << endl;
  cout << " - shape(fft2calfft)      = " << data.fft2calfft().shape()  << endl;
  cout << " - Datafiles (names only) = " << data.datafiles(false)      << endl;
  cout << " - Datafiles (full path)  = " << data.datafiles(true)       << endl;
  cout << " - Antenna numbers        = " << data.antennas(true)        << endl;
  cout << " - Selected antennas      = " << data.selectedAntennas()    << endl;
}


// -----------------------------------------------------------------------------

/*!
  \brief Test constructors for a new NDABeam object

  \return nofFailedTests -- The number of failed tests.
*/
int test_NDABeam (const String& metafile)
{
  std::cout << "\n[test_NDABeam]\n" << std::endl;

  int nofFailedTests (0);
  uint blocksize (128);
  
  std::cout << "[1] Testing default constructor ..." << std::endl;
  {
    NDABeam data;
    //
    show_parameters (data);
  }
  
  std::cout << "[2] Testing argumented constructor ..." << std::endl;
  {
    NDABeam data (metafile);
    //
    show_parameters (data);
  }
  
  std::cout << "[3] Testing argumented constructor ..." << std::endl;
  {
    NDABeam data (metafile,
		  blocksize);
    //
    show_parameters (data);
  }
  
  return nofFailedTests;
}

// -----------------------------------------------------------------------------

int test_readData (const String& metafile)
{
  std::cout << "\n[test_readData]\n" << std::endl;
  
  int nofFailedTests (0);
  uint blocksize (1024);
  
  NDABeam data (metafile,
		blocksize);

  cout << "adc2voltage = " << data.ADC2Voltage() << endl;
  
  cout << "[1] Exporting raw ADC time series ... " << flush;
  {
    Matrix<Double> fx (data.fx());
    IPosition shape(fx.shape());

    ofstream logfile;
    logfile.open("fx.data");
    
    for (int sample(0); sample<shape(0); sample++) {
      for (int antenna(0); antenna<shape(1); antenna++) {
	logfile << fx (sample,antenna) << "  ";
      }
      logfile << endl;
    }

    logfile.close();
  }
  cout << "done" << endl;

  cout << "[2] Exporting voltage time series ... " << flush;
  {
    Matrix<Double> voltage (data.voltage());
    IPosition shape(voltage.shape());

    ofstream logfile;
    logfile.open("voltage.data");
    
    for (int sample(0); sample<shape(0); sample++) {
      for (int antenna(0); antenna<shape(1); antenna++) {
	logfile << voltage (sample,antenna) << "  ";
      }
      logfile << endl;
    }

    logfile.close();
  }
  cout << "done" << endl;

  cout << "[3] Exporting raw FFT spectra ... " << flush;
  {
    Matrix<DComplex> fft (data.fft());
    IPosition shape(fft.shape());

    ofstream logfile;
    logfile.open("fft.data");
    
    for (int sample(0); sample<shape(0); sample++) {
      for (int antenna(0); antenna<shape(1); antenna++) {
	logfile << abs(fft (sample,antenna)) << "  ";
      }
      logfile << endl;
    }

    logfile.close();
  }
  cout << "done" << endl;
  
  cout << "[4] Exporting calibrated FFT spectra ... " << flush;
  {
    Matrix<DComplex> calfft (data.calfft());
    IPosition shape(calfft.shape());

    ofstream logfile;
    logfile.open("calfft.data");
    
    for (int sample(0); sample<shape(0); sample++) {
      for (int antenna(0); antenna<shape(1); antenna++) {
	logfile << abs(calfft (sample,antenna)) << "  ";
      }
      logfile << endl;
    }

    logfile.close();
  }
  cout << "done" << endl;

  return nofFailedTests;
}

// -----------------------------------------------------------------------------

int main (int argc,
	  char *argv[])
{
  int nofFailedTests (0);
  string metafile;
  
  if (argc < 2) {
    cerr << "[tNDABeam] No filename on input - using default." << endl;
    return -1;
  } else {
    metafile = argv[1];
  }

  // Test for the constructor(s)
  {
    nofFailedTests += test_NDABeam (metafile);
  }

  // Test reading in the data from disk
  {
    nofFailedTests += test_readData (metafile);
  }
  
  return nofFailedTests;
}
