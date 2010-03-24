/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
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

#include <casa/aips.h>
#include <casa/iostream.h>
#include <casa/Exceptions/Error.h>

#include <IO/LopesEvent.h>

using CR::Data;
using CR::DataReader;
using CR::LopesEvent;

/*!
  \file tLopesEvent.cc

  \ingroup CR_Data
  
  \brief A collection of test routines for LopesEvent
 
  \author Lars B&auml;hren
 
  \date 2005/08/25

  <h3>Usage</h3>

  In order to run the test program simply type:
  \verbatim
  tLopesEvent <eventfile>
  \endverbatim
  where the single argument is the path to a LOPES event file.

  As the output data are simple ASCII tables, you can use e.g. <i>gnuplot</i>
  to plot the contents:
  \code
  set grid;

  set xlabel 'Sample number'
  set ylabel 'ADC value'
  plot 'fx.data' u 1 w l, 'fx.data' u 2 w l

  set ylabel 'Voltage'
  plot 'voltage.data' u 1 w l, 'voltage.data' u 2 w l

  set logscale y
  set ylabel 'Raw FFT'
  plot 'fft.data' u ($1*$1) w l, 'fft.data' u ($3*$3) w l

  set ylabel 'Calibrated FFT'
  plot 'calfft.data' u ($1*$1) w l, 'calfft.data' u ($3*$3) w l
  \endcode

*/

// -----------------------------------------------------------------------------

void show_LopesEvent (LopesEvent& event)
{
  cout << " - Filename .......... : " << event.filename() << endl;
  cout << " - Length ............ : " << event.length() << endl;
  cout << " - Version number .... : " << event.version() << endl;
  cout << " - Time (JDR) ........ : " << event.timeJDR() << endl;
  cout << " - Time (TL) ......... : " << event.timeTL() << endl;
  cout << " - Time (LTL) ........ : " << event.timeLTL() << endl;
  cout << " - Data type ......... : " << event.dataType() << endl;
  cout << " - Event type ........ : " << event.evType() << endl;
  cout << " - Presync . [samples] : " << event.presync() << endl;

  cout << " - blocksize           : " << event.blocksize() << endl;
  cout << " - FFT length ........ : " << event.fftLength() << endl;
  cout << " - selected antennas   : " << event.selectedAntennas() << endl;
//  cout << " - selected channels   : " << event.selectedChannels() << endl;
}

// -----------------------------------------------------------------------------

/*!
  \brief A few statistical information on the exported data

  \param data -- Array with the data extracted from the file
*/
template <class T> void show_statistics (Matrix<T> &data)
{
  cout << " - Shape of data array   = " << data.shape()     << endl;
  cout << " - Number of data points = " << data.nelements() << endl;
  cout << " - Range of values       = " << min(data) << " .. " << max(data) << endl;
}

template void show_statistics (Matrix<Float> &data);
template void show_statistics (Matrix<Double> &data);
template void show_statistics (Matrix<Complex> &data);
template void show_statistics (Matrix<DComplex> &data);

// -----------------------------------------------------------------------------

// -------------------------------------------------------------- test_LopesEvent

/*!
  \brief Test constructors for a new LopesEvent object
  
  \param name -- Name of the event file to be opened.

  \return nofFailedTests -- The number of failed tests.
*/
int test_LopesEvent (string const& name,
		     uint const &blocksize)
{
  cout << "\n[test_LopesEvent]\n" << endl;

  int nofFailedTests (0);
  uint nofAntennas (8);
  uint fftLength;

  fftLength = blocksize/2+1;

  cout << "\n[1] Testing constructor: LopesEvent ()" << endl;
  try {
    LopesEvent event;
    // 
    show_LopesEvent (event);
  } catch (AipsError x) {
    cerr << x.getMesg() << endl;
    nofFailedTests++;
  }
  
  cout << "\n[2] Testing constructor: LopesEvent (const char *)" << endl;
  try {
    LopesEvent event (name.c_str());
    // 
    show_LopesEvent (event);
  } catch (AipsError x) {
    cerr << x.getMesg() << endl;
    nofFailedTests++;
  }
  
  cout << "\n[3] Testing constructor: LopesEvent (string const &, uint const &)"
       << endl;
  try {
    LopesEvent event (name,blocksize);
    // 
    show_LopesEvent (event);
  } catch (AipsError x) {
    cerr << x.getMesg() << endl;
    nofFailedTests++;
  }
  
  cout << "\n[4] Testing constructor: LopesEvent (string const &, uint const &, ...)"
       << endl;
  try {
    Vector<Double> adc2voltage (nofAntennas,0.5);
    Matrix<DComplex> fft2calfft (fftLength,nofAntennas,0.5);
    //
    LopesEvent event (name,blocksize,adc2voltage,fft2calfft);
    // 
    show_LopesEvent (event);
  } catch (AipsError x) {
    cerr << x.getMesg() << endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
}

// -------------------------------------------------------------------- test_Header

/*!
  \brief Test access to the information stored in the data file header
  
  \param name -- Name of the event file to be opened.
  
  \return nofFailedTests -- The number of failed tests.
*/
int test_Header (const char *name)
{
  int nofFailedTests (0);
  
  cout << "\n[#] Opening LOPES event file for reading of header ..." << endl;
  try {
    LopesEvent event;
    event.readHeader(name);
    // 
    show_LopesEvent (event);
    //
  } catch (AipsError x) {
    cerr << x.getMesg() << endl;
    nofFailedTests++;
  }

  return nofFailedTests;
}

// -------------------------------------------------------------------- test_Data

/*!
  \brief Test reading in the data
  
  \param name -- Name of the event file to be opened.
  
  \return nofFailedTests -- The number of failed tests.
*/
int test_Data (const char *name)
{
  cout << "\n[test_Data]\n" << endl;

  int nofFailedTests (0);
  
  cout << "[1] Opening LOPES event file for reading of data ..." << endl;
  try {
    LopesEvent event (name);
    uint nofDataSets (event.length());
    uint blocksize (event.blocksize());
    Matrix<short> fx (blocksize,nofDataSets);
    uint sample (0);
    float sum (0.0);
    float mean (0.0);
    // 
    show_LopesEvent (event);
    //
    cout << " -- reading in data ..." << endl;
    //
    cout << " ant \t pointer \t entries \t sum \t mean \t sum \t mean" << endl;
    for (uint antenna(0); antenna<nofDataSets; antenna++) {
      //
      Data *data = event.dataSet(antenna);
       //
      sum = 0.0;
      for (sample=0; sample<blocksize; sample++) {
	fx(sample,antenna) = data->data(sample);
	sum += data->data(sample);
      }
      mean = sum/blocksize;
      //
      cout << antenna                 << "\t"
	   << data                    << "\t"
	   << data->data(0)           << "\t"
 	   << data->data(blocksize-1) << "\t"
	   << sum                     << "\t"
	   << mean 
 	   << endl;
    }

  } catch (AipsError x) {
    cerr << x.getMesg() << endl;
    nofFailedTests++;
  }

  return nofFailedTests;
}


// -------------------------------------------------------------- test_DataReader

/*!
  \brief Usage of an LopesEvent object within the DataReader processing framework

  \param name      -- Name of the event file to be opened.
  \param blocksize -- Size of a read data block, [samples].

  \return nofFailedTests -- The number of failed tests.
*/
int test_DataReader (string const &name,
                     uint const &blocksize)
{
  cout << "\n[test_DataReader]\n" << endl;

  int nofFailedTests (0);
  ofstream outfile;
  IPosition shape;

  DataReader *dr;
  LopesEvent *le;

  le = new LopesEvent (name,blocksize);
  dr = le;

  show_LopesEvent (*le);

  cout << "[1] Writing ADC values ..." << endl;
  try {
    Matrix<Double> fx (dr->fx());
    shape = fx.shape();
    //
    show_statistics(fx);
    //
    outfile.open("fx.data");
    for (int sample (0); sample<shape(0); sample++) {
      for (int antenna (0); antenna<shape(1); antenna++) {
	outfile << fx(sample,antenna) << "\t";
      }
      outfile << endl;
    }
    outfile.close();
  } catch (AipsError x) {
    cerr << x.getMesg() << endl;
    nofFailedTests++;
  }

  // Set conversion from ADC values to voltages
  {
    Vector<Double> adc2voltage (shape(1));
    adc2voltage = 0.00048828125;
    
    dr->setADC2Voltage (adc2voltage);
  }

  cout << "[2] Writing voltages values ..." << endl;
  try {
    Matrix<Double> voltage (dr->voltage());
    shape = voltage.shape();
    //
    show_statistics(voltage);
    //
    outfile.open("voltage.data");
    for (int sample (0); sample<shape(0); sample++) {
      for (int antenna (0); antenna<shape(1); antenna++) {
	outfile << voltage(sample,antenna) << "\t";
      }
      outfile << endl;
    }
    outfile.close();
  } catch (AipsError x) {
    cerr << x.getMesg() << endl;
    nofFailedTests++;
  }

  cout << "[3] Writing raw FFT (Nyquist zone " << dr->nyquistZone() << ") ..."
       << endl;
  try {
    Matrix<DComplex> fft (dr->fft());
    shape = fft.shape();
    //
    show_statistics(fft);
    //
    outfile.open("fft.data");
    for (int channel (0); channel<shape(0); channel++) {
      for (int antenna (0); antenna<shape(1); antenna++) {
	outfile << real(fft(channel,antenna)) << "\t"
		<< imag(fft(channel,antenna)) << "\t";
      }
      outfile << endl;
    }
    outfile.close();
  } catch (AipsError x) {
    cerr << x.getMesg() << endl;
    nofFailedTests++;
  }

  cout << "[4] Writing calibrated FFT ..." << endl;
  try {
    Matrix<DComplex> calfft (dr->calfft());
    shape = calfft.shape();
    //
    show_statistics(calfft);
    //
    outfile.open("calfft.data");
    for (int channel (0); channel<shape(0); channel++) {
      for (int antenna (0); antenna<shape(1); antenna++) {
	outfile << real(calfft(channel,antenna)) << "\t"
		<< imag(calfft(channel,antenna)) << "\t";
      }
      outfile << endl;
    }
    outfile.close();
  } catch (AipsError x) {
    cerr << x.getMesg() << endl;
    nofFailedTests++;
  }

  return nofFailedTests;
}

//_______________________________________________________________________________
//                                                                           main

int main (int argc,
	  char *argv[])
{
  int nofFailedTests (0);
  string filename;
  uint blocksize (2048);

  if (argc < 2) {
    cerr << "[tLopesEvent] Missing name of event file!" << endl;
    return -1;
  } else {
    filename = argv[1];
  }

  // Test for the constructor(s)
  nofFailedTests += test_LopesEvent (filename, blocksize);
  
  // Test reading of header
  nofFailedTests += test_Header (filename.c_str());
  
  // Test reading of data
  nofFailedTests += test_Data (filename.c_str());
  
  nofFailedTests += test_DataReader (filename, blocksize);
  
  return nofFailedTests;
}
