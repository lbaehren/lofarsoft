/***************************************************************************
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
#include <casa/iostream.h>
#include <casa/fstream.h>
#include <casa/string.h>
#include <casa/Arrays.h>
#include <casa/Exceptions/Error.h>

#include <ApplicationSupport/DataReaderTools.h>
#include <Data/ITSCapture.h>
#include <Data/LopesEvent.h>

using namespace std;
using namespace CR;

/*!
  \file tDataReaderTools.cc

  \ingroup ApplicationSupport

  \brief A collection of test routines for DataReaderTools
 
  \author Lars B&auml;hren
 
  \date 2006/06/20
*/

GlishRecord makeRecord (string const &which="ITSCapture");

/*!
  \brief Create a Glish record containing all required information for testing

  \code
  // Create some data to fill into the record

  // Create GlishRecord and fill it with the data
  GlishRecord rec;
  rec.add("datatype",datatype);
  rec.add("basefile",basefile);
  rec.add("blocksize",blocksize);
  rec.add("antennas",antennas);
  rec.add("selectedAntennas",selectedAntennas);
  rec.add("adc2voltage",adc2voltage);
  rec.add("fft2calfft",fft2calfft);

  return rec;
  \endcode
*/
GlishRecord makeRecord (string const &which)
{
  std::cout << "[makeRecord] Creating record ... " << std::endl;

  int blocksize (8192);
  String datatype ("UNDEFINED");
  String basefile ("UNDEFINED");
  int fftLength (0);
  int nofAntennas (0);
  Vector<uint> tmp_antennas;

  try {
    if (which == "ITSCapture") {
      datatype = "ITSCapture";
//       basefile = "/data/ITS/exampledata.its/experiment.meta";
      basefile = "/data/ITS/2004.11/2004.11.02-11:30:14.04-24h/experiment.meta";
//       basefile = "/data/ITS/2004.08/2004.08.24-14:28:29.00-Thunderstorm/experiment.meta";
      //
      cout << " - creating ITSCapture object ..." << endl;
      ITSCapture data (basefile,blocksize);
      cout << " - extracting FFT lenght ..." << endl;
      fftLength   = data.fftLength();
      cout << " - extracting nof. antennas ..." << endl;
      nofAntennas = data.nofAntennas(); 
      tmp_antennas = data.antennas();
    }
    else if (which == "LopesEvent") {
      datatype = "LopesEvent";
//       basefile = "/data/LOPES/example.event";
      basefile = "/data/LOPES/2003/2003.10.28-megaburst/2003.10.28.10:58:16.304.event";
      //
      cout << " - creating LopesEvent object ..." << endl;
      LopesEvent data (basefile,blocksize);
      cout << " - extracting FFT lenght ..." << endl;
      fftLength   = data.fftLength();
      cout << " - extracting nof. antennas ..." << endl;
      nofAntennas = data.nofAntennas();    
      tmp_antennas = data.antennas();
    }
    else {
      cerr << "WARNING: undefined data type " << which << endl;
    }
  } catch (AipsError x) {
    cerr << x.getMesg() << endl;
  }
  
  // conversion factors and selection arrays
  Vector<Double> adc2voltage (nofAntennas,1.0);
  Matrix<DComplex> fft2calfft (fftLength,nofAntennas,1.0);
  Vector<int> antennas (nofAntennas);
  Vector<bool> antennaSelection (nofAntennas);
  Vector<int> selectedAntennas (nofAntennas);
  Vector<int> selectedChannels (fftLength);
  
  for (int antenna(0); antenna<nofAntennas; antenna++) {
    antennas(antenna) = tmp_antennas(antenna);
    selectedAntennas(antenna) = antenna;
    antennaSelection(antenna) = true;
  }

  for (int channel (0); channel<fftLength; channel++) {
    selectedChannels(channel) = channel;
  }

  // some feedback

  cout << " -- input file        = " << basefile         << endl;
  cout << " -- blocksize         = " << blocksize        << endl;
  cout << " -- FFT length        = " << fftLength        << endl;
  cout << " -- nof. antennas     = " << nofAntennas      << endl;
  cout << " -- antennas          = " << antennas         << endl;
  cout << " -- antenna selection = " << antennaSelection << endl;
  
  // Assemble the GlishRecord
  GlishRecord rec;
  rec.add("datatype",datatype);
  rec.add("basefile",basefile);
  rec.add("blocksize",blocksize);
  rec.add("antennas",antennas);
  rec.add("antennaSelection",antennaSelection);
  rec.add("selectedAntennas",selectedAntennas);
  rec.add("selectedChannels",selectedChannels);
  rec.add("adc2voltage",adc2voltage);
  rec.add("fft2calfft",fft2calfft);
  
  std::cout << " - done." << std::endl;

  return rec;
}

// -----------------------------------------------------------------------------

/*!
  \brief Test constructors for a new DataReaderTools object

  \return nofFailedTests -- The number of failed tests.

  To display the written data file using Gnuplot:
  \verbatim
  set grid
  set logscale y
  set xlabel 'Frequency channels number'
  set ylabel 'Spectral intensity'
  plot 'fft.data' u ($1*$1) t 'Antenna 1' w l
  replot 'fft.data' u ($2*$2) t 'Antenna 2' w l
  replot 'fft.data' u ($3*$3) t 'Antenna 3' w l
  \endverbatim
*/
int test_assignFromRecord ()
{
  std::cout << "\n[test_assignFromRecord]\n" << std::endl;

  int nofFailedTests (0);
  bool verbose (true);

  string datatype ("ITSCapture");
//   string datatype ("LopesEvent");
  GlishRecord rec = makeRecord (datatype);

//   uint blocksize (512);
  DataReader *dr;

  // assign pointer to DataReader object after parsing the input record
  dr = DataReaderTools::assignFromRecord (rec);

  // read some data

  // display the parameters stored in the DataReader object
  if (verbose) {
    cout << "Parameter values after assignment from record" << endl;
    cout << " - Type of input data    = " << datatype                  << endl;
    cout << " - Blocksize             = " << dr->blocksize()           << endl;
    cout << " - FFT length            = " << dr->fftLength()           << endl;
    cout << " - Antennas              = " << dr->antennas()            << endl;
    cout << " - Antenna selection     = " << dr->antennaSelection()    << endl;
    cout << " - Selected antennas     = " << dr->selectedAntennas()    << endl;
    cout << " - nof selected channels = " << dr->nofSelectedChannels() << endl;
    cout << endl;
  }

  // -- export of data to disk ---------------------------------------

  ofstream outfile;
  uint antenna (0);
  uint nofAntennas (dr->nofAntennas());
  uint nofChannels (dr->fftLength());

  cout << "[1] Exporting raw FFT ... " << endl;
  {
    Matrix<DComplex> fft (dr->fft());

    cout << " - nof. selected antennas = " << dr->nofSelectedAntennas() << endl;
    cout << " - nof. selected channels = " << dr->nofSelectedChannels() << endl;
    cout << " - shape(fft)             = " << fft.shape() << endl;

    outfile.open ("fft-01.data",ios::out);
    
    for (uint channel=0; channel<nofChannels; channel++) {
      outfile << channel;
      for (antenna=0; antenna<nofAntennas; antenna++) {
	outfile << "\t" << fabs(fft(channel,antenna));
      }
      outfile << endl;
    }
    
    outfile.close();
  }

  cout << "[2] Exporting raw FFT after antenna selection ... " << endl;
  {    
    Vector<bool> antennaSelection (nofAntennas);
    
    antennaSelection = true;
    antennaSelection(0) = antennaSelection(nofAntennas-1) = false;

    dr->setSelectedAntennas (antennaSelection);

    Matrix<DComplex> fft (dr->fft());
    IPosition shape (fft.shape());

    cout << " - nof. selected antennas = " << dr->nofSelectedAntennas() << endl;
    cout << " - nof. selected channels = " << dr->nofSelectedChannels() << endl;
    cout << " - shape(fft)             = " << fft.shape() << endl;

    outfile.open ("fft-02.data",ios::out);

    for (int channel=0; channel<shape(0); channel++) {
      outfile << channel;
      for (antenna=0; antenna<uint(shape(1)); antenna++) {
	outfile << "\t" << fabs(fft(channel,antenna));
      }
      outfile << endl;
    }
    
    outfile.close();
  }
  
  cout << "[3] Exporting raw FFT after channel selection ... " << endl;
  {
    uint fftLength (dr->fftLength());
    Vector<bool> selection (fftLength);
    
    selection = true;
    for (uint channel(0); channel<fftLength/2; channel++) {
      selection(channel) = false;
    }

    dr->setSelectedChannels (selection);

    Matrix<DComplex> fft (dr->fft());
    IPosition shape (fft.shape());
    Vector<uint> selectedChannels (dr->selectedChannels());

    cout << " - nof. selected antennas = " << dr->nofSelectedAntennas() << endl;
    cout << " - nof. selected channels = " << dr->nofSelectedChannels() << endl;
    cout << " - shape(fft)             = " << fft.shape() << endl;

    outfile.open ("fft-03.data",ios::out);

    for (int channel=0; channel<shape(0); channel++) {
      outfile << selectedChannels (channel);
      for (antenna=0; antenna<uint(shape(1)); antenna++) {
	outfile << "\t" << fabs(fft(channel,antenna));
      }
      outfile << endl;
    }
    
    outfile.close();
  }

  cout << "[3] Exporting visibilities ... " << endl;
  {
    Matrix<DComplex> visibilities (dr->visibilities(True));
    IPosition shape (visibilities.shape());
    Vector<uint> selectedChannels (dr->selectedChannels());

    cout << " - nof. selected antennas = " << dr->nofSelectedAntennas() << endl;
    cout << " - nof. selected channels = " << dr->nofSelectedChannels() << endl;
    cout << " - nof. baselines         = " << dr->nofBaselines() << endl;

    outfile.open ("fft-04.data",ios::out);

    for (int channel=0; channel<shape(0); channel++) {
      outfile << selectedChannels (channel);
      for (antenna=0; antenna<uint(shape(1)); antenna++) {
	outfile << "\t" << fabs(visibilities(channel,antenna));
      }
      outfile << endl;
    }
    
    outfile.close();
  }

  return nofFailedTests;
}

// -----------------------------------------------------------------------------

int main ()
{
  int nofFailedTests (0);

  // Test for the constructor(s)
  {
    nofFailedTests += test_assignFromRecord ();
  }

  return nofFailedTests;
}
