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

/* $Id: DataCC.cc,v 1.8 2007/03/19 17:40:33 bahren Exp $ */

#include <Data/DataCC.h>

namespace CR {  //  Namespace CR -- begin
  
  // ============================================================================
  //
  //  Construction / Destruction
  //
  // ============================================================================
  
  // --------------------------------------------------------------------- DataCC
  
  DataCC::DataCC (String fileMeta)
    : ObservationMeta (fileMeta)
  {
    DataCC::initDataCC ();
  }
  
  // --------------------------------------------------------------------- DataCC
  
  DataCC::DataCC (String fileMeta,
		  String fileAntpos)
    : ObservationMeta (fileMeta,fileAntpos)
  {
    DataCC::initDataCC ();
  }
  
  // --------------------------------------------------------------------- DataCC
  
  void DataCC::initDataCC ()
  {
    // read in the contents metafile
    ObservationMeta::readMetafile();
    // Extract the frequency channels from the plugin options
    DataCC::setFrequencyChannels ();
    // Put together the path to the input datafile
    DataCC::setDatafile ();
  }
  
  // ============================================================================
  //
  //  Mapping between data cube axes in disk file and storage
  //
  // ============================================================================
  
  void DataCC::setAxisOrder (const Vector<Int>& axisOrder)
  {
    IPosition shape = axisOrder.shape();
    axisOrder_p.resize(shape);
    axisOrder_p = axisOrder;
  }
  
  // =============================================================================
  //
  //  Selection of (a) antenna elements and (b) frequency channels.
  //
  // =============================================================================
  
  void DataCC::setAntennaSelection (Vector<Bool> const &inputAntMask)
  {
    IPosition shapeMask (inputAntMask.shape());
    IPosition ccShape (ccCube_p.shape());
    
    // Initialize the internal antenna selection mask
    antennaSelection_p.resize(ccShape(0));
    antennaSelection_p = True;
    
    if (shapeMask(0) == ccShape(0)) {
      antennaSelection_p = inputAntMask;
    } else {
      //
      Vector<Int> antennas = ObservationMeta::antennas();
      //
      // If the number of antenna elements extracted from the experiment metafile
      // coincides with the number of antenna elements used for the computation of
      // the cc-cube, we use the antenna numbers as bridge between the internal 
      // antenna selection mask and the input antenna selection mask.
      //
      CR::setSubmask (antennaSelection_p,
		      inputAntMask,
		      antennas);
    } 
    
  }
  
  void DataCC::setFrequencySelection (const Vector<Bool>& inputFreqMask)
  {
    IPosition shapeMask (inputFreqMask.shape());
    IPosition ccShape (ccCube_p.shape());
    
    frequencySelection_p.resize(ccShape(2));
    frequencySelection_p = True;
    
    if (shapeMask(0) == ccShape(2)) {
      frequencySelection_p = inputFreqMask;
    } else {
      Vector<Bool> frequencySelection(ccShape(2));
      Vector<Int> frequencyChannels;
      //
      frequencySelection = True;
      frequencyChannels = DataCC::frequencyChannels();
      //
      CR::setSubmask (frequencySelection,
		      inputFreqMask,
		      frequencyChannels);
      //
      frequencySelection_p = frequencySelection;
    }
    
  }
  
  
  // ==============================================================================
  //
  //  Access to the cross-correlation data-cube
  //
  // ==============================================================================
  
  // ------------------------------------------------------------------------ shape
  
  IPosition DataCC::shape ()
  {
    return ccCube_p.shape();
  }
  
  // ----------------------------------------------------------------------- ccCube
  
  Cube<DComplex> DataCC::ccCube ()
  {
    return ccCube_p;
  }
  
  // ----------------------------------------------------------------------- ccCube
  
  Cube<DComplex> DataCC::ccCube (const Vector<Bool>& antennaMask,
				 const Vector<Bool>& frequencyMask)
  {
    Int nantOUT(0);
    Int nfreqOUT(0);
    Int ni(0);
    Int nj(0);
    Int nk(0);
    Cube<DComplex> ccCube;
    
    // -----------------------------------------------------------------
    // Check the shape of masking arrays.
    
    DataCC::setAntennaSelection (antennaMask);
    DataCC::setFrequencySelection (frequencyMask);
    
    IPosition ccShape (ccCube_p.shape());
    IPosition freqShape (frequencySelection_p.shape());
    IPosition antShape (antennaSelection_p.shape());
    
    cout << "[DataCC::ccCube]" << endl;
    cout << " - Shape of the cc-data cube  : " << ccShape << endl;
    cout << " - Shape of frequency mask .. : " << freqShape << endl;
    cout << " - Shape of antenna mask .... : " << antShape << endl;
    
    // -----------------------------------------------------------------
    // Determine the dimensions of the returned cc-matrix.
    
    nfreqOUT = 0;
    for (int n=0; n<ccShape(2); ++n) {
      nfreqOUT += int(frequencySelection_p(n));
    }
    
    nantOUT = 0;
    for (int n=0; n<ccShape(1); ++n) {
      nantOUT += int(antennaSelection_p(n));
    }
    
    IPosition ccOUT (3,nantOUT,nantOUT,nfreqOUT);
    ccCube.resize(ccOUT);
    
    cout << " - Adjusting shape : " << ccShape << " -> " << ccOUT << endl;
    
    /*
      Set contents of the masking cube based on the provided frequency flagging
      vector.
    */
    cout << " - Extracting selected cc-cube elements ... " << flush;
    ni = 0;
    int i (0);
    int j (0);
    int k (0);
    for (i=0; i<ccShape(0); ++i) {
      if (antennaSelection_p(i)) {
	nj = 0;
	for (j=0; j<ccShape(1); ++j) {
	  if (antennaSelection_p(j)) {
	    nk = 0;
	    for (k=0; k<ccShape(2); ++k) {
	      if (frequencySelection_p(k)) {
		ccCube(ni,nj,nk) = ccCube_p(i,j,k);
		++nk;
	      }
	    } // end loop : k
	    ++nj;
	  }
	} // end loop : j
	++ni;
      }
    } // end loop : i
    cout << "Done." << endl;
    
    return ccCube;
    
  }
  
  // ============================================================================
  //
  //  Auto-/Cross-correlation spectra
  //
  // ============================================================================
  
// --------------------------------------------------------------------- spectrum

Vector<Double> DataCC::spectrum (const Int ant)
{
  return DataCC::spectrum (ant,ant);
}

// --------------------------------------------------------------------- spectrum

Vector<Double> DataCC::spectrum (const Int ant1,
				 const Int ant2) 
{
  Vector<DComplex> ccSpectrum (DataCC::ccSpectrum(ant1,ant2));
  Int nofChannels (ccSpectrum.nelements());
  Vector<Double> spectrum (nofChannels);

  for (int chan=0; chan<nofChannels; ++chan) {
//     spectrum(chan) = abs(ccSpectrum(chan)*conj(ccSpectrum(chan)));
    spectrum(chan) = fabs(ccSpectrum(chan));
  }
  
  return spectrum;
}

// --------------------------------------------------------------------- spectrum

void DataCC::spectrum (const Int ant1,
		       const Int ant2,
		       std::ostream& s)
{
  s << DataCC::spectrum (ant1,ant2) << endl;
}

Vector<DComplex> DataCC::ccSpectrum (const Int ant1,
				    const Int ant2)
{
  IPosition shape (ccCube_p.shape());
  Vector<DComplex> spectrum (shape(2));
  IPosition blc (shape);
  IPosition trc (shape);

  blc(0) = trc(0) = ant1;
  blc(1) = trc(1) = ant2;

  blc(2) = 0;
  trc(2) = shape(2)-1;

  try {
    IPosition shapeSpectrum (1,shape(2));
    spectrum = ccCube_p(blc,trc).reform(shapeSpectrum);
  } catch (AipsError x) {
    cerr << "[DataCC::ccSpectrum] " << x.getMesg() << endl;
  }
  
  return spectrum;
}

// =============================================================================
//
//  Data I/O from/to disk
//
// =============================================================================

// ------------------------------------------------------------------ setDatafile

void DataCC::setDatafile () 
{
  String datafileDir = ObservationMeta::directory();
  String datafileName ("i0001.dat");
  
  Matrix<String> datafiles = ObservationMeta::datafiles();
  if (datafiles(0,0) != "") {
    datafileName = CR::fileFromPath (datafiles(0,0));
  }

  datafile_p = datafileDir + "/" + datafileName;
}

void DataCC::readDataCC ()
{
  ifstream datafile;
  IPosition shape;
  int nofElements,element,nchan,nant;
  double* inputMatrix;
  
  cout << "[DataCC::readDataCC]" << endl;

  //--------------------------------------------------------
  // determine the number of antennae

  shape = ObservationMeta::antennas().shape();
  nant = shape(0);

  //--------------------------------------------------------
  // Determine the number of frequeny channels

  shape = availableChannels_p.shape();
  nchan = shape(0);

  //--------------------------------------------------------
  // get the path to the file containing the cc datacube

  nofElements = nant*nant*nchan;
  inputMatrix = new double[nofElements];
  
  //--------------------------------------------------------
  // Read the data from disk into buffer
  
  cout << " - Data file : " << datafile_p << endl;
  cout << " - Reading data from disk into buffer ... " << flush;

  datafile.open(datafile_p.c_str(), ios::in | ios::binary);
  
  datafile.seekg(0);
  datafile.read ((char*)inputMatrix, sizeof (double) * nofElements);
  datafile.close();
  
  cout << "Done." << endl;
  
  //--------------------------------------------------------
  // sort the data into the internal data cube
  
  ccCube_p.resize(nant,nant,nchan);
  ccCube_p = DComplex(0.0,0.0);

  cout << " - Filling upper part of ccCube ... " << flush;
  
  element = 0;
  for (int k=0; k<nchan; ++k) {
    for (int i=0; i<nant; ++i) {
      for (int j=0; j<nant; ++j) {
        if (i<j) { // imaginary part
          ccCube_p(i,j,k) += DComplex(0.0,inputMatrix[element]);
        } 
        else if (i>j) { // real part
          ccCube_p(j,i,k) += DComplex(inputMatrix[element],0.0);
        }
        else { // diagonal
          ccCube_p(i,i,k) = inputMatrix[element];
        }
        ++element;
      }
    }
  }

  cout << "Done." << endl;

  // Free allocated memory
  
  delete[] inputMatrix;

  // DComplex conjugate part

  cout << " - Filling complex conjugate part ... " << flush;
  
  for (int i=0; i<nant; ++i) {
    for (int j=i+1; j<nant; ++j) {
      for (int k=0; k<nchan; ++k) {
	ccCube_p(j,i,k) = conj(ccCube_p(i,j,k));
      }
    }   
  }
  
  cout << "Done." << endl;

}

// =============================================================================
//
//  Processing of the plugin parameters
//
// =============================================================================

Vector<Double> DataCC::frequencyValues () 
{
  Int nofChannels = availableChannels_p.nelements();
  Vector<Double> frequencies(nofChannels);
  Double TIM_clock = 80.0e+6;
  Double step = TIM_clock/fftsize_p;
  
  for (int k=0; k<nofChannels; k++) {
    frequencies(k) = availableChannels_p(k)*step;
  }
  
  return frequencies;
}

Vector<Double> DataCC::frequencyValues (const Bool onlySelected)
{
  Vector<Double> freqAll;
  Vector<Double> freqSel;

  freqAll = DataCC::frequencyValues ();

  if (!onlySelected) {
    freqSel = freqAll;
  } else {
    int nfreqSel (frequencySelection_p.nelements());
    int nfreqOUT (0);
    //
    for (int n=0; n<nfreqSel; ++n) {
      nfreqOUT += int(frequencySelection_p(n));
    }
    freqSel.resize(nfreqOUT);
    // sort the frequency values into the returned array
    nfreqOUT = 0;
    for (int n=0; n<nfreqSel; ++n) {
      if (frequencySelection_p(n)) {
	freqSel(nfreqOUT) = freqAll(n);
	nfreqOUT +=1;
      }
    }
    
  }

  return freqSel;
}

Vector<Bool> DataCC::frequencyMask ()
{
  Int fftlen = fftsize_p/2;
  Vector<Bool> mask(fftlen);
  mask = False;

  // set up the mask
  for (uInt k=0; k<availableChannels_p.nelements(); k++) {
    mask(availableChannels_p(k)) = True;
  }
  
  // return the result
  return mask;
}

void DataCC::setFrequencyChannels ()
{
  String plugin1 = ObservationMeta::plugin1();
  String plugin2 = ObservationMeta::plugin2();
  Vector<String> sub1 = CR::getSubstrings (plugin1," ");
  Vector<String> sub2 = CR::getSubstrings (plugin2," ");
  IPosition ip;
  Matrix<Int> frequencyChannels;
  Int nofChannels (0);
  Int nofIntervals (0);

  // -------------------------------------------------------
  // Parse the options of the first plugin

  ip = sub1.shape();
  for (int i=0; i<ip(0); i++) {
    // Number of points (channels) in the fft.
    if (sub1(i) == "-c") {
      fftsize_p = atoi(sub1(i+1).c_str());
    }
    // Enabled channels
    if (sub1(i) == "-e") {
      // decompose multiple intervals
      nofIntervals = CR::nofSubstrings (sub1(i+1),",");
      Vector<String> intervals = CR::getSubstrings (sub1(i+1),",");
      frequencyChannels.resize(nofIntervals,2);
      frequencyChannels = -1;
      // decompose the range
      for (int j=0; j<nofIntervals; j++) {
	Vector<String> tmp = CR::getSubstrings (intervals(j),":");
	IPosition shape = tmp.shape();
	for (int k=0; k<shape(0); k++) {
	  try {
	    frequencyChannels(j,k) = atoi(tmp(k).c_str());
	  } catch (AipsError x) {
	    cerr << "[DataCC::setFrequencyChannels] " << x.getMesg() << endl;
	  }
	}
      }
    }
  }

  // -------------------------------------------------------
  // Parse the options of the second plugin

  ip = sub2.shape();
  for (int i=0; i<ip(0); i++) {
    if (sub2(i) == "-c") {
      nofChannels = atoi(sub2(i+1).c_str());
    }
  }

  // Determine the total number of frequency channels
  // -------------------------------------------------------

  availableChannels_p.resize(nofChannels);

  Int chan = 0;
  for (int i=0; i<nofIntervals; i++) {
    if (frequencyChannels(i,1) > frequencyChannels(i,0)) {
      for (int k=frequencyChannels(i,0); k<=frequencyChannels(i,1); k++) {
	availableChannels_p(chan) = k;
	++chan;
      }
    } else {
      availableChannels_p(chan) = frequencyChannels(i,0);
      ++chan;
    }
  }
  
}


// =============================================================================
//
//  Convert the cross-correlation data into visibilities
//
// =============================================================================

// ----------------------------------------------------------------- visibilities

Matrix<DComplex> DataCC::visibilities () 
{
  IPosition ccShape (ccCube_p.shape());
  Vector<Bool> antennaMask(ccShape(0), True);
  Vector<Bool> frequencyMask(ccShape(2), True);
  // get the visibilities
  return DataCC::ccCube2Visibilities (antennaMask,frequencyMask);
}

// ----------------------------------------------------------------- visibilities

Matrix<DComplex> DataCC::visibilities (const Vector<Bool>& antennaMask,
				      const Vector<Bool>& frequencyMask)
{
  // get the visibilities
  return DataCC::ccCube2Visibilities(antennaMask,frequencyMask);
}

// ----------------------------------------------------------------- visibilities

Matrix<DComplex> DataCC::visibilities (const Int nofChannels) 
{
  // get the visibilities
  Matrix<DComplex> vis = DataCC::visibilities ();

  // Frequency averaging

  // return the result
  return vis;  
}

// ----------------------------------------------------------------- visibilities

Matrix<DComplex> DataCC::visibilities (const Vector<Double>& band, 
				       const Int nofChannels)
{
  IPosition ccShape (ccCube_p.shape());
  Vector<Double> frequencies (DataCC::frequencyValues());
  // Set up the masking arrays
  Vector<Bool> antennaMask(ccShape(0), True);
  Vector<Bool> frequencyMask = CR::maskFromRange (band,
						     frequencies);
  //
  Matrix<DComplex> vis = DataCC::ccCube2Visibilities (antennaMask,
						      frequencyMask);
  // averaging of the frequency channels
  
  // return the visibilities
  return vis;
}

Matrix<DComplex> DataCC::ccCube2Visibilities (const Vector<Bool>& antennaMask,
					      const Vector<Bool>& frequencyMask)
{
  cout << "[DataCC::ccCube2Visibilities]" << endl;
  
  // get the cross-correlated data, applying selection
  Cube<DComplex> ccData = DataCC::ccCube (antennaMask,frequencyMask);
  cout << " - cc-cube extracted." << endl;

  // Set up the matrix storing the visibility data
  IPosition shape = ccData.shape();
  uInt nofBaselines = (shape(0)*(shape(0)-1))/2;
  Matrix<DComplex> vis (nofBaselines,shape(2));

  //
  uInt nBaseline(0);

  //
  baselines_p.resize (nofBaselines,2);

  // Copy data from ccCube to visibilities matrix
  for (Int i=0; i<shape(0); i++) {
    for (Int j=i+1; j<shape(0); j++) {
      baselines_p(nBaseline,0) = i;
      baselines_p(nBaseline,1) = j;
      for (Int k=0; k<shape(2); k++) {
 	vis(nBaseline,k) = ccData(i,j,k);
      }
      //
      nBaseline++;
    }
  }

  // return the result
  return vis;
}

Matrix<Int> DataCC::baselines () {
  return baselines_p;
}

}  //  Namespace CR -- end

