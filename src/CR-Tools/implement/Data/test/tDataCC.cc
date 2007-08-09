/***************************************************************************
 *   Copyright (C) 2004,2005 by Lars B"ahren                               *
 *   bahren@astron.nl                                                      *
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

/* $Id: tDataCC.cc,v 1.3 2007/03/20 14:45:31 bahren Exp $ */

#include <casa/aips.h>
#include <casa/Arrays.h>
#include <casa/BasicSL/Complex.h>

// Custom header files
#include <Data/DataCC.h>

/*!
  \file tDataCC.cc
  
  \ingroup Data
  
  \brief A collection of tests for DataCC.

  \author Lars B&auml;hren
  
  <h3>Prerequisite</h3>

  <ul type="square">
    <li>[LOPES-Tools] DataCC -- Storage for a set of cross-correlated data.
  </ul>

  <h3>Synopsis</h3>

  This file contains a number of test routines for the DataCC class. Beside that
  this program also can be used to extract some initial diagnostics on a
  cross-crorrelation dataset (e.g. by extraction of the antenna signal
  autocorrelation spectra).

  <h3>Usage</h3>

  \verbatim
  cd $LOPESCODEDIR/c-progs
  ./tDataCC <metafile> <antposfile>
  \endverbatim
  where <i>&lt;metafile&gt;</i> is the fully specified path to the experiment
  metafile of an ITS experiment and <i>&lt;antposfile&gt;</i> is the fully
  specified path to the antenna position file, holding the 3-dimensional
  positions of the array elements.
  
*/

// =============================================================================

/*!
  \brief Compute the values of the frequency axis.
  
  \param ccData -- A DataCC object.

  \return freq -- A vector with the frequency values, [Hz].
*/
Vector<Double> frequencies (DataCC& ccData)
{
  IPosition ccShape (ccData.shape());
  Double points_FFT = ccData.fftsize();
  Double TIM_clock = 80.0e+6;
  Double step = TIM_clock/points_FFT;
  Vector<Double> freq(ccShape(2));

  for (int k=0; k<ccShape(2); k++) {
    freq(k) = k*step;
  }

  return freq;        
}

// =============================================================================

/*!
  \brief Display the properties of the frequency axis of the data cube. 

  \param ccData - A DataCC object.
 */
void showFrequencies (DataCC& ccData)
{
  IPosition ccShape;
  Int fftsize;
  Vector<Int> frequencyChannels;
  Vector<Double> frequencyValues;
  Vector<Bool> frequencyMask;

  // Get some of the original settings for later reference
  ccShape = ccData.shape();
  fftsize = ccData.fftsize();
  //
  cout << "\n[tDataCC::showFrequencies]" << endl;
  cout << " - Shape of the data cube : " << ccShape << endl;
  cout << " - Input size of the FFT  : " << fftsize << endl;

  // Get arrays with frequency information
  frequencyChannels = ccData.frequencyChannels();
  frequencyValues = ccData.frequencyValues();
  frequencyMask = ccData.frequencyMask();

  cout << "\n[tDataCC::showFrequencies]" << endl;
  //
  cout << " - Frequency channels, array shape : "
       << frequencyChannels.shape() << endl;
  cout << " - Frequency channels, index range : "
       << frequencyChannels(0) << " .. "
       << frequencyChannels(frequencyChannels.nelements()-1) << endl;
  //
  cout << " - Frequency values, array shape . : "
       << frequencyValues.shape() << endl;
  cout << " - Frequency values, [Hz] ........ : "
       << frequencyValues(0) << " .. "
       << frequencyValues(frequencyValues.nelements()-1) << endl;
  //
  cout << " - Frequency mask, array shape ... : "
       << frequencyMask.shape() << endl;
}

// =============================================================================

/*!
  \brief Export the autocorrelation spectra of all antennae
  
  Compute and export the autocorrelation spectra for all antennae in the
  dataset; two output files are generated:
  - <i>tDataCC-allAC.dat</i> - auto-correlation spectrum for each antenna;
  columns 1 and 2 contain frequency channel and frequency value [Hz]
  respectively.
  - <i>tDataCC-meanAC.dat</i> - mean auto-correlation spectrum; columns 1
  and 2 contain frequency channel and frequency value [Hz] respectively.
  
  \param ccData - A DataCC object.
 */
Bool exportSpectra (DataCC& ccdata)
{
  Bool ok (True);
  IPosition ccShape = ccdata.shape();
  Matrix<Double> spectra(ccShape(1),ccShape(2));
  Vector<Double> freq = ccdata.frequencyValues();
  Double sum (0.0);

  // Extract all autocorrelation spetra and cummulate value for mean spectrum

  for (int ant=0; ant<ccShape(1); ant++) {
    Vector<Double> spectrum = ccdata.spectrum(ant);
    spectra.row(ant) = spectrum;
  }

  // Export the spectra

  cout << "\n[tDataCC::exportSpectra]" << endl;

  cout << " - Opening output streams ... " << flush;
  ofstream fileSpectra ("tDataCC-allAC.dat",ios::out);
  ofstream fileMean ("tDataCC-meanAC.dat",ios::out);
  cout << "ok" << endl;

  cout << " - Exporting autocorrelation spectra ... " << flush;
  for (int chan=0; chan<ccShape(2); chan++) {
    sum=0.0;
    // all AC spectra
    fileSpectra << chan << "\t" << freq(chan);
    for (int ant=0; ant<ccShape(1); ant++) {
      fileSpectra << "\t" << spectra(ant,chan);
      sum += spectra(ant,chan);
    }
    fileSpectra << endl;
    // mean AC spectrum
    fileMean << chan << "\t" << freq(chan) << "\t" << sum/ccShape(1) << endl;
  }
  cout << "ok" << endl;

  cout << " - Closing output streams ... " << flush;
  fileSpectra.close();
  fileMean.close();
  cout << "ok" << endl;

  return ok;
}

// =============================================================================

/*!
  \brief Extract the complex cross-correlation spectrum for a antenna combination.

  \param ccData - A DataCC object.

  \return ok - Status; returns False if there was an error.
 */
Bool extractComplexSpectrum (DataCC& ccdata)
{
  Bool ok (True);
  Vector<DComplex> ccSpectrum;
  Int ant1 (1);
  Int ant2 (4);
  
  try {
    ccSpectrum = ccdata.ccSpectrum(ant1,ant2);
  } catch (AipsError x) {
    cerr << "[tDataCC::extractComplexSpectrum] " << x.getMesg() << endl;
    ok = False;
  }
  
  if (ok) {
    Int nelem (ccSpectrum.nelements());
    ofstream outfile ("tDataCC-ccSpectrum.data",ios::out);
    //
    outfile << " # Antenna combination : " << ant1+1 << "," << ant2+1 << endl;
    //
    for (int n=0; n<nelem; n++) {
      outfile << n
	      << "\t" << ccSpectrum(n)
	      << "\t" << real(ccSpectrum(n))
	      << "\t" << imag(ccSpectrum(n))
	      << "\t" << abs(ccSpectrum(n))
	      << endl;
    }
    //
    outfile.close();
  }

  return ok;
}

// =============================================================================

/*!
  \brief Export the cc'ed data as set of visibilities.

  \param ccData - A DataCC object.

  \return ok - Status; returns False if there was an error.
*/
Bool exportVisibilities (DataCC& ccdata)
{
  Bool ok (True);
  IPosition ccShape (ccdata.shape());

  try {
    Matrix<DComplex> vis = ccdata.visibilities();
    Matrix<Int> baselines = ccdata.baselines();
    //
    cout << "[tDataCC::exportVisibilities]" << endl;
    cout << " - cc datacube  : " << ccShape << endl;
    cout << " - Visibilities : " << vis.shape() << endl;
    cout << " - Baselines .. : " << baselines.shape() << endl;
  } catch (AipsError x) {
    cerr << "[extractComplexSpectrum] " << x.getMesg() << endl;
    ok = False;
  }

  return ok;
}

// =============================================================================

int main (int argc, char *argv[])
{
  Bool ok (True);
  String metafile    = argv[1];
  String antennafile = argv[2];

  // -----------------------------------------------------------------
  // Create a cc-datacube object and fill it with the data stored on
  // disk

  DataCC cc (metafile,antennafile);
  //
  try {
    cc.readDataCC();
  } catch (AipsError x) {
    cerr << "[tDataCC] " << x.getMesg() << endl;
    ok = False;
  }

//   // report the setup for the frequency channels
//   showFrequencies(cc);

  // Export the autocorrelation spectra
  ok = exportSpectra (cc);

  // Export (some) complex cross-correlation spectra.
  ok = extractComplexSpectrum (cc);

  // Conversion to visibilities 
  ok = exportVisibilities (cc);

  return 1;

}
