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

/* $Id: DataReader.cc,v 1.30 2006/10/31 18:24:08 bahren Exp $*/

/*!
  \class DataReader
*/

#include <casa/Arrays/ArrayLogical.h>
#include <casa/Arrays/ArrayMath.h>

#include <IO/DataReader.h>

namespace LOPES {  // namespace LOPES -- begin
  
  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================
  
  DataReader::DataReader ()
  {
    init (1);
  }
  
  DataReader::DataReader (uint const &blocksize)
  {
    init (blocksize);
  }
  
  DataReader::DataReader (uint const &blocksize,
			  uint const &nyquistZone,
			  double const &samplerate)
  {
    init (blocksize,
	  nyquistZone,
	  samplerate);
  }
  
  DataReader::DataReader (uint const &blocksize,
			  Vector<double> const &adc2voltage,
			  Matrix<DComplex> const &fft2calfft)
  {
    init (blocksize,
	  1,
	  80e06);
    init (blocksize,
	  adc2voltage,
	  fft2calfft);
  }
  
  template <class T> 
  DataReader::DataReader (String const &filename,
			  uint const &blocksize,
			  T const &var,
			  Vector<double> const &adc2voltage,
			  Matrix<DComplex> const &fft2calfft)
  {
    init (blocksize,
	  1,
	  80e06);
    init (blocksize,
	  adc2voltage,
	  fft2calfft);
  }
  
  DataReader::DataReader (DataReader const &other)
  {
    copy (other);
  }
  
  // ============================================================================
  //
  //  Destruction
  //
  // ============================================================================
  
  DataReader::~DataReader ()
  {
    destroy();
  }
  
  void DataReader::destroy ()
  {;}
  
  // ============================================================================
  //
  //  Operators
  //
  // ============================================================================
  
  DataReader& DataReader::operator= (DataReader const &other)
  {
    if (this != &other) {
      destroy ();
      copy (other);
    }
    return *this;
  }
  
  // ------------------------------------------------------------------------- copy
  
  void DataReader::copy (DataReader const &other)
  {
    try {
      // -- header -----------------------------------------------------
      
      header_p     = other.header_p;
      
      // -- Array dimensions -------------------------------------------
      
      blocksize_p    = other.blocksize_p;    
      fftLength_p    = other.fftLength_p;
      nyquistZone_p  = other.nyquistZone_p;
      samplingRate_p = other.samplingRate_p;
      
      // -- conversion -------------------------------------------------
      
      adc2voltage_p.resize (other.adc2voltage_p.shape());
      adc2voltage_p    = other.adc2voltage_p;
      
      fft2calfft_p.resize (other.fft2calfft_p.shape());
      fft2calfft_p     = other.fft2calfft_p;
      
      // -- Antennas ---------------------------------------------------
      
      antennas_p.resize (other.antennas_p.shape());
      antennas_p       = other.antennas_p;
      
      selectedAntennas_p.resize (other.selectedAntennas_p.shape());
      selectedAntennas_p = other.selectedAntennas_p;
      
      // -- Frequency channels -----------------------------------------
      
      selectedChannels_p.resize (other.selectedChannels_p.shape());
      selectedChannels_p = other.selectedChannels_p;
      
      selectChannels_p = other.selectChannels_p;
      
      hanningFilter_p  = other.hanningFilter_p;
      
      applyHanning_p   = other.applyHanning_p;
      
      // -- File streams -----------------------------------------------
      
      nofStreams_p = other.nofStreams_p;
      startBlock_p = other.startBlock_p;
      iterator_p   = new DataIterator[nofStreams_p];
      fileStream_p = new fstream[nofStreams_p];
      
      try {
	for (uint stream (0); stream<nofStreams_p; stream++) {
	  iterator_p[stream]   = other.iterator_p[stream];
	  // 	other.fileStream_p[stream].tie((fileStream_p[stream]));
	}
      } catch (AipsError x) {
	cerr << x.getMesg() << endl;
      }
      
    } catch (AipsError x) {
      cerr << "[DataReader::copy]" << x.getMesg() << endl;
    }
  }
  
  // ==============================================================================
  //
  //  Parameters
  //
  // ==============================================================================
  
  // ------------------------------------------------------------------------- init
  
  void DataReader::init (uint const &blocksize) 
  {
    uint nyquistZone   (1);
    double samplingRate (80e06);
    
    init (blocksize,
	  nyquistZone,
	  samplingRate);
  }
  
  // ------------------------------------------------------------------------- init
  
  void DataReader::init (uint const &blocksize,
			 uint const &nyquistZone,
			 double const &samplerate)
  {
    setStartBlock   (1);
    setBlocksize    (blocksize);
    setNyquistZone  (nyquistZone);
    setSamplingRate (samplerate);
    
    Vector<double> adc2voltage (1,1.0);
    
    Matrix<DComplex> fft2calfft (fftLength_p,1,1.0);
    
    init (blocksize,
	  adc2voltage,
	  fft2calfft);
  }
  
  // ------------------------------------------------------------------------- init
  
  void DataReader::init (uint const &blocksize,
			 Vector<double> const &adc2voltage,
			 Matrix<DComplex> const &fft2calfft)
  {
    /*
      Do some basic checking here: the provided numbers and array must be
      conformant - esp. the number of elements in 'adc2voltage' must correspond
      to the length of the second axis of 'fft2calfft'.
    */
    
    IPosition shapeADC (adc2voltage.shape());
    IPosition shapeFFT (fft2calfft.shape());
    
    if (shapeADC(0) != shapeFFT(1)) {
      cerr << "[DataReader::init] Inconsistent number of antennas!" << endl;
      cerr << " - shape(adc2voltage) = " << adc2voltage.shape() << endl;
      cerr << " - shape(fft2calfft)  = " << fft2calfft.shape() << endl;
      cerr << " => Rejecting conversion arrays!" << endl;
      /*
	If the shapes are incorrect there is no way to determine, which of the 
	arrays we'd have to adjust, to get to a consistent setup; in order to 
	nevertheless at least create a valid DataReader object, we make a call to 
	init (uint const&)
      */
      init (blocksize);
    } else {
      /*
	The blocksize must be set at first, because this also will determine the 
	output length of the FFT, which in turn is required for further checking
      */
      setBlocksize (blocksize);
      
      /*
	Antenna selection: by default we enable all antennas
      */
      Vector<uint> antennas (shapeADC);
      Vector<uint> selectedAntennas (shapeADC);
      
      for (int n(0); n<shapeADC(0); n++) {
	antennas(n) = selectedAntennas(n) = n;
      }
      
      setAntennas (antennas,
		   selectedAntennas);
      
      // book-keeping
      nofStreams_p = adc2voltage.nelements();
      
      //   if (fftLength_p != uint(shape(0))) {
      //     cerr << "[DataReader::init] Inconsistent number of frequencies!" << endl;
      //     cerr << " - FFT output length = " << fftLength_p << endl;
      //     cerr << " - shape(fft2calfft) = " << fft2calfft.shape() << endl;
      //   }
      
      /*
	Frequency channel selection: by default we enable all frequency channels
      */
      Vector<bool> selectedChannels (fftLength_p);
      selectedChannels = true;
      
      setSelectedChannels (selectedChannels);
      selectChannels_p = false;
      
      // process provided parameters
      setADC2voltage (adc2voltage);
      setFFT2calFFT (fft2calfft);
      
      // -- Setup of the Hanning filter (disabled by default)
      
      setHanningFilter (0.0);
      
      // -- feedback -----------------------------------------------------
      
      //     cout << "[DataReader::init]" << endl;
      //     cout << " blocksize        = " << blocksize_p           << endl;
      //     cout << " fftLength        = " << fftLength()           << endl;
      //     cout << " Nyquist zone     = " << nyquistZone()         << endl;
      //     cout << " nofAntennas      = " << nofAntennas()         << endl;
      //     cout << " selectedAntennas = " << nofSelectedAntennas() << endl;
      //     cout << " selectedChannels = " << nofSelectedChannels() << endl;
    }
    
  }
  
  void DataReader::init (uint const &blocksize,
			 Vector<uint> const &antennas,
			 Vector<double> const &adc2voltage,
			 Matrix<DComplex> const &fft2calfft,
			 Vector<String> const &filenames,
			 DataIterator const *iterators)
  {
    bool status (true);
    uint nofFiles (filenames.nelements());
    
    /*
      [Step 1]
      Make a call to the more simple internal init method first - this will
      do the basic setup and provide a consistent set of internal parameters
    */
    init (blocksize,
	  adc2voltage,
	  fft2calfft);
    
    /*
      [Step 2]
      Take care of the antenna numbers, as they are needed later of for accessing
      the array containg conversion values and data to be returned
    */
    
    /*
      [Step 3]
      Take care of the file streams
    */
    
    // check the values for the number of files and the data block size
    if (nofFiles != nofStreams_p ||
	blocksize != blocksize_p) {
      cerr << "[DataReader::setStreams] Wrong blocksize or number of filenames!"
	   << endl;
      cerr << " - blocksize (internal)   = " << blocksize_p << endl;
      cerr << " - blocksize (streams)    = " << blocksize << endl;
      cerr << " - nof. Files             = " << nofStreams_p << endl;
      cerr << " - length of input vector = " << nofFiles << endl;
      //
      status = false;
    } else {
      // update the internal streams array
      fileStream_p = new fstream[nofStreams_p];
      iterator_p   = new DataIterator[nofStreams_p];
      // connect the streams to the files
      for (uint file(0); file<nofStreams_p; file++) {
	fileStream_p[file].open(filenames(file).c_str(), ios::in | ios::binary);
	iterator_p[file] = iterators[file];
      } 
    }
    
  }
  
  // ----------------------------------------------------------------- setBlocksize
  
  void DataReader::setBlocksize (uint const &blocksize)
  {
    blocksize_p = blocksize;
    //
    setFFTLength ();
  }
  
  // ----------------------------------------------------------------- setFFTLength
  
  void DataReader::setFFTLength ()
  {
    Vector <double> inColumn (blocksize_p,1.0);
    Vector<DComplex> outColumn;
    casa::FFTServer<double,DComplex> server(IPosition(1,blocksize_p),
					    casa::FFTEnums::REALTOCOMPLEX);
    server.fft(outColumn,inColumn);
    
    fftLength_p = outColumn.nelements();
  }
  
  // ----------------------------------------------------------------- samplingRate
  
  double DataReader::samplingRate (String const &unit)
  {
    Quantity rate (samplingRate_p,"Hz");
    
    return rate.getValue(unit);
  }
  
  // -------------------------------------------------------------- setSamplingRate
  
  void DataReader::setSamplingRate (double const &samplingRate)
  {
    samplingRate_p = samplingRate;
  }
  
  // -------------------------------------------------------------- setSamplingRate
  
  void DataReader::setSamplingRate (Quantity const &samplingRate)
  {
    setSamplingRate (samplingRate.getValue("Hz"));
  }
  
  // -------------------------------------------------------------- frequencyValues
  
  Vector<double> DataReader::frequencyValues (bool const &onlySelected)
  {
    Vector<double> frequencies;
    double freqMin (samplingRate_p/2*(nyquistZone_p-1));
    double freqResolution (samplingRate_p/(2*(fftLength_p-1)));
    uint nofChannels;
    
    if (onlySelected) {
      nofChannels = nofSelectedChannels();
      frequencies.resize (nofChannels);
      //
      for (uint channel (0); channel<nofChannels; channel++) {
	frequencies(channel) = freqMin + selectedChannels_p(channel)*freqResolution;
      }
    } else {
      nofChannels = fftLength_p;
      frequencies.resize (nofChannels);
      //
      for (uint channel (0); channel<nofChannels; channel++) {
	frequencies(channel) = freqMin + channel*freqResolution;
      }
    }
    
    return frequencies;
  }
  
  // ==============================================================================
  //
  //  Navigation through the data volume
  //
  // ==============================================================================
  
  // ---------------------------------------------------------------- setStartBlock
  
  void DataReader::setStartBlock (uint const &startBlock)
  {
    if (startBlock>0) {
      startBlock_p = startBlock;
    } else {
      std::cerr << "[DataReader::setStartBlock] Invalid number for start block."
		<< std::endl;
      startBlock_p = 1;
    }
  }
  
  // --------------------------------------------------------------------- setBlock
  
  void DataReader::setBlock (uint const &block)
  {
    for (unsigned int n(0); n<nofStreams_p; n++) {
      iterator_p[n].setBlock(block);
    }
  }
  
  // -------------------------------------------------------------------- setStride
  
  void DataReader::setStride (uint const &stride)
  {
    for (unsigned int n(0); n<nofStreams_p; n++) {
      iterator_p[n].setStride(stride);
    }
  }
  
  // --------------------------------------------------------------------- setShift
  
  void DataReader::setShift (uint const &shift)
  {
    for (unsigned int n(0); n<nofStreams_p; n++) {
      iterator_p[n].setShift(shift);
    }
  }
  
  // -------------------------------------------------------------------- nextBlock
  
  void DataReader::nextBlock ()
  {
    for (unsigned int n(0); n<nofStreams_p; n++) {
      iterator_p[n].nextBlock();
    }
  }
  
  // ==============================================================================
  //
  //  Methods
  //
  // ==============================================================================
  
  // --------------------------------------------------------------------------- fx
  
  Matrix<double> DataReader::fx ()
  {
    Matrix<double> out (blocksize_p,nofSelectedAntennas());
    
    out = 0.0;
    
    return out;
  }
  
  // ---------------------------------------------------------------------- voltage
  
  Matrix<double> DataReader::voltage ()
  {
    Matrix<double> in (fx());
    Matrix<double> out (in);
    
    /*
      Keep in mind here, that there might be a set of antennas selected
    */
    try {
      for (uint antenna(0); antenna<nofSelectedAntennas(); antenna++) {
	out.column(antenna) = in.column(antenna)*adc2voltage_p(selectedAntennas_p(antenna));
      }
    } catch (AipsError x) {
      cerr << "[DataReader::voltage]" << x.getMesg() << endl;
    }
    
    return out;
  }
  
  // -------------------------------------------------------------------------- fft
  
  Matrix<DComplex> DataReader::fft ()
  {
    Matrix<double> in (voltage());
    Matrix<DComplex> out (fftLength_p,
			nofSelectedAntennas());
    
    Vector <double> inColumn;
    Vector<DComplex> outColumn;
    casa::FFTServer<double,DComplex> server(IPosition(1,blocksize_p),
					    casa::FFTEnums::REALTOCOMPLEX);
    IPosition shape (out.shape());
    
    for (int antenna(0); antenna<shape(1); antenna++) {
      inColumn = in.column(antenna);
      // apply Hanning filter (optional)
      if (applyHanning_p == true) {
	hanningFilter_p.filter(inColumn);
      }
      // FFT the data block for the current antenna ...
      server.fft(outColumn,inColumn);
      // .. and copy the result, depending on the Nyquist zone setting
      switch (nyquistZone_p) {
      case 1:
	out.column(antenna) = outColumn;
	break;
      case 2:
	for (int channel (0); channel<shape(0); channel++) {
	  out(channel,antenna) = conj(outColumn(shape(0)-channel-1));
	}
	break;
      }
    }
    
    // handle frequency channel selection
    if (!selectChannels_p) {
      return out;
    } else {
      return selectChannels (out);
    }
  }
  
  // ----------------------------------------------------------------------- calfft
  
  Matrix<DComplex> DataReader::calfft ()
{
  int antenna(0);
  int channel(0);
  int nofSelectedAntennas (DataReader::nofSelectedAntennas());
  int nofSelectedChannels (DataReader::nofSelectedChannels());
  Matrix<DComplex> in (fft());
  Matrix<DComplex> out (nofSelectedChannels,nofSelectedAntennas);

  out = 0.0;

//   cout << "[DataReader::calfft]" << endl;
//   cout << " - selected antennas = " << selectedAntennas_p   << endl;
//   cout << " - selected channels = " << selectedChannels_p   << endl;
//   cout << " - shape(fft)        = " << in.shape()           << endl;
//   cout << " - shape(fft2calfft) = " << fft2calfft_p.shape() << endl;
//   cout << " - shape(calfft)     = " << out.shape()          << endl;

  /*
    Conversion from raw to calibrated FFT: as we may have selected on both
    antenna numbers and frequency channels, we cannot simply do
      out = in*fft2calfft_p;
  */
  try {
    for (antenna=0; antenna<nofSelectedAntennas; antenna++) {
      for (channel=0; channel<nofSelectedChannels; channel++) {
	out (channel,antenna) = in(channel,antenna)
	  *fft2calfft_p(selectedChannels_p(channel),selectedAntennas_p(antenna));
      }
    }
  } catch (AipsError x) {
    cerr << "[DataReader::calfft]" << x.getMesg() << endl;
  }

  return out;
}

// -------------------------------------------------------------------- ccSpectra

Cube<DComplex> DataReader::ccSpectra (bool const &fromCalFFT)
{
  int antenna1 (0);
  int antenna2 (0);
  int channel  (0);
  Matrix<DComplex> in (DataReader::nofSelectedChannels(),
		      DataReader::nofSelectedAntennas());

  if (fromCalFFT == true) {
    in = calfft();
  } else {
    in = fft();
  }

  IPosition shape (in.shape());
  Cube<DComplex> out (shape(0),shape(1),shape(1),0.0);

  for (antenna2=0; antenna2<shape(1); antenna2++) {
    for (antenna1=0; antenna1<shape(1); antenna1++) {
      for (channel=0; channel<shape(0); channel++) {
	out (channel,antenna1,antenna2) = in(channel,antenna1)*in(channel,antenna2);
      }
    }
  }

  return out;
}

// ----------------------------------------------------------------- visibilities

Matrix<DComplex> DataReader::visibilities (bool const &fromCalFFT)
{
  int antenna1 (0);
  int antenna2 (0);
  int channel  (0);
  int baseline (0);
  Matrix<DComplex> in (DataReader::nofSelectedChannels(),
		      DataReader::nofSelectedAntennas());

  if (fromCalFFT == true) {
    in = calfft();
  } else {
    in = fft();
  }

  IPosition shape (in.shape());
  Matrix<DComplex> vis (shape(0),nofBaselines(false));

  for (antenna1=0; antenna1<shape(1); antenna1++) {
    for (antenna2=antenna1+1; antenna2<shape(1); antenna2++) {
      for (channel=0; channel<shape(0); channel++) {
	vis(channel,baseline) = in(channel,antenna1)*conj(in(channel,antenna2));
      }
      baseline++;
    }
  }
  
  return vis;
}

// ==============================================================================
//
//  Conversion & Selection
//
// ==============================================================================

// --------------------------------------------------------------- setADC2voltage

void DataReader::setADC2voltage (Vector<double> const &adc2voltage)
{
  uint nelem (adc2voltage.nelements());

  if (nelem == nofSelectedAntennas()) {
    Vector<double> tmp (nofAntennas());
    tmp = 1.0;
    // re-arrange the provided values to match the correct array shape
    for (uint antenna(0); antenna<nelem; antenna++) {
      tmp(selectedAntennas_p(antenna)) = adc2voltage (antenna);
    }
    // store the values 
    adc2voltage_p.resize(tmp.shape());
    adc2voltage_p = tmp;
  }
  else if (nelem == nofAntennas()) {
    adc2voltage_p.resize(nelem);
    adc2voltage_p = adc2voltage;
  }
  else {
    cerr << "[DataReader::setADC2voltage] Mismatching array shapes" << endl;
    cerr << " shape(adc2voltage)  = " << nelem                 << endl;
    cerr << " # antennas          = " << nofAntennas()         << endl;
    cerr << " # selected antennas = " << nofSelectedAntennas() << endl;
  }

}

// ---------------------------------------------------------------- setFFT2calFFT

void DataReader::setFFT2calFFT (Matrix<DComplex> const &fft2calfft)
{
  bool status (true);
  IPosition shape (fft2calfft.shape());
  uint nofChannels (fftLength_p);
  uint nofAntennas (nofAntennas());
  uint nofSelectedAntennas (nofSelectedAntennas());
  uint nofSelectedChannels (nofSelectedChannels());

  /*
    We can use a single general loop for accepting the input data; the only
    difference in the actual access to the matrix elements will be caused by the
    shape of the input matrix, thus if we check for this first we can use generic
    code later on.
   */

  if (uint(shape(0)) == nofSelectedChannels &&
      uint(shape(1)) == nofSelectedAntennas) {
    //
    nofChannels = nofSelectedChannels;
    nofAntennas = nofSelectedAntennas;
  }
  else if (uint(shape(0)) == nofSelectedChannels &&
	   uint(shape(1)) == nofAntennas) {
    //
    nofChannels = nofSelectedChannels;
  }
  else if (uint(shape(0)) == fftLength_p &&
	   uint(shape(1)) == nofSelectedAntennas) {
    //
    nofChannels = fftLength_p;
    nofAntennas = nofSelectedAntennas;
  }
  else if (uint(shape(0)) == fftLength_p &&
	   uint(shape(1)) == nofAntennas) {
    //
    nofChannels = fftLength_p;
  }
  else {
    cerr << "[DataReader::setFFT2calFFT] Mismatching array shapes" << endl;
    cerr << " shape(fft2calfft)       = " << shape                 << endl;
    cerr << " FFT output length       = " << fftLength_p           << endl;
    cerr << " # of selected channels  = " << nofSelectedChannels   << endl;
    cerr << " # of antennas           = " << nofAntennas           << endl;
    cerr << " # of selected antennas  = " << nofSelectedAntennas   << endl;
    //
    status = false;
  }

  // store the new values - if everything went fine so far
  if (status) {
    uint antenna(0);
    uint channel(0);
    // adjust the size of the internal array
    fft2calfft_p.resize (fftLength_p,nofAntennas);
    fft2calfft_p = 1.0;
    //
    for (antenna=0; antenna<nofAntennas; antenna++) {
      for (channel=0; channel<nofChannels; channel++) {
	fft2calfft_p (selectedChannels_p(channel),selectedAntennas_p(antenna))
	  = fft2calfft(channel,antenna);
      }
    }
  }
}

// ------------------------------------------------------------- setHanningFilter

void DataReader::setHanningFilter (double const &alpha)
{
  if (alpha == 0.0) {
    applyHanning_p = false;
  } else {
    HanningFilter<double> tmp (blocksize_p,alpha);
    hanningFilter_p = tmp;
    applyHanning_p = true;
  }
}

void DataReader::setHanningFilter (const double &alpha,
				   const uint &beta)
{
  if (alpha == 0.0) {
    applyHanning_p = false;
  } else {
    HanningFilter<double> tmp (blocksize_p,
			      alpha,
			       beta);
    hanningFilter_p = tmp;
    applyHanning_p = true;
  }
}
  
  // --------------------------------------------------------------- selectChannels
  
  Matrix<DComplex> DataReader::selectChannels (Matrix<DComplex> const &in)
  {
    IPosition shape (in.shape());
    uint nofChannels (DataReader::nofSelectedChannels());
    Matrix<DComplex> out (nofChannels,
			  nofSelectedAntennas());
    
    /*
      We also should check here wether or not the number of antennas agrees.
    */
    
    if (uint(shape(0)) == nofChannels &&
	uint(shape(1)) == nofSelectedAntennas()) {
      out = in;
      return out;
    }
    
    try {
      for (uint channel(0); channel<nofChannels; channel++) {
	out.row(channel) = in.row(selectedChannels_p(channel));
      }
    } catch (AipsError x) {
      cerr << "[DataReader::selectChannels]" << x.getMesg() << endl;
    }
    
    return out;
  }
  
  // ==============================================================================
  //
  //  Antennas and antenna selection
  //
  // ==============================================================================
  
  // ------------------------------------------------------------------ setAntennas
  
  bool DataReader::setAntennas (Vector<uint> const &antennas)
  {
    bool status (true);
    
    /*
      We might need additional checking againt the number of elements in
      adc2voltage, in case the latter is defined.
    */
    
    antennas_p.resize (antennas.shape());
    antennas_p = antennas;
    
    return status;
  }
  
  // ------------------------------------------------------------------ setAntennas
  
  bool DataReader::setAntennas (Vector<uint> const &antennas,
				Vector<uint> const &selectedAntennas)
  {
    bool status (true);
    uint nofAntennas (antennas.nelements());
    uint nofSelectedAntennas (selectedAntennas.nelements());
    
    if (nofSelectedAntennas > nofAntennas) {
      cerr << "[DataReader::setAntennas]"                        << endl;
      cerr << " nof antennas          = " << nofAntennas         << endl;
      cerr << " nof selected antennas = " << nofSelectedAntennas << endl;
      status = false;
    } else {
      setAntennas (antennas);
      setSelectedAntennas (selectedAntennas);
    }
    
    return status;
  }
  
  // ------------------------------------------------------------------ setAntennas
  
  bool DataReader::setAntennas (Vector<uint> const &antennas,
				Vector<bool> const &antennaSelection)
  {
    bool status (true);
    uint nofAntennas (antennas.nelements());
    
    if (nofAntennas != antennaSelection.nelements()) {
      cerr << "[DataReader::setAntennas]"                               << endl;
      cerr << " shape(antennas)         = " << antennas.shape()         << endl;
      cerr << " shape(antennaSelection) = " << antennaSelection.shape() << endl;
      status = false;
    } else {
      setAntennas (antennas);
      setSelectedAntennas (antennaSelection);
    }
    
    return status;
  }
  
  // ---------------------------------------------------------- setSelectedAntennas
  
  bool DataReader::setSelectedAntennas (Vector<uint> const &antennaSelection,
					bool const &absolute)
  {
    bool status (true);
    
    // check if the arrays size is correct
    
    try {
      
      if (antennaSelection.nelements() > antennas_p.nelements()) {
	cerr << "[DataReader::setSelectedAntennas] Too many selected antennas!"
	     << endl;
	cerr << " shape(antennas)         = " << antennas_p.shape()       << endl;
	cerr << " shape(antennaSelection) = " << antennaSelection.shape() << endl;
	status = false;
      } else {
	selectedAntennas_p.resize(antennaSelection.shape());
	selectedAntennas_p = antennaSelection;
      }
      
    } catch (AipsError x) {
      cerr << "[DataReader::setSelectedAntennas]" << x.getMesg() << endl;
      status = false;
    }
    
    return status;
  }
  
  // ---------------------------------------------------------- setSelectedAntennas
  
  bool DataReader::setSelectedAntennas (Vector<bool> const &antennaSelection)
  {
    uint nelem (antennaSelection.nelements());
    
    /*!
      We need to check the number of elements in the selection array; of course
      the number of selected antennas should always be <= the total number of
      antennas. If this condition is not given, do not accept the input array
      and exit with "false".
    */
    if (nelem == nofAntennas()) {
      Vector<uint> selectedAntennas (ntrue(antennaSelection));
      uint counter (0);
      
      antennaSelection_p.resize(nelem);
      antennaSelection_p = antennaSelection;
      
      for (uint antenna(0); antenna<nelem; antenna++) {
	if (antennaSelection(antenna)) {
	  selectedAntennas (counter) = counter;
	  counter++;
	}
      }
      
      return setSelectedAntennas (selectedAntennas);
    } else {
      cerr << "[DataReader::setSelectedAntennas]"               << endl;
      cerr << " - Incorrect number of antenna selection flags!" << endl;
      cerr << " - nof. antennas = " << nofAntennas()            << endl;
      cerr << " - nof. flags    = " << nelem                    << endl;
      return false;
    }
  }
  
  // ----------------------------------------------------------------- nofBaselines
  
  uint DataReader::nofBaselines (bool const &allAntennas) const
  {
    uint nant (nofSelectedAntennas());
    
    if (allAntennas) {
      nant = nofAntennas ();
    }
    
    return (nant-1)*nant/2;
  }
  
  // ==============================================================================
  //
  //  Frequency channels and frequency channel selection
  //
  // ==============================================================================
  
  
  bool DataReader::setSelectedChannels (Vector<uint> const &channelSelection)
  {
    bool status (true);
    
    selectedChannels_p.resize (channelSelection.shape());
    selectedChannels_p = channelSelection;
    
    selectChannels_p = true;
    
    return status;
  }



bool DataReader::setSelectedChannels (Vector<bool> const &channelSelection)
{
  uint nelem (channelSelection.nelements());
    
    if (nelem == fftLength_p) {
      Vector<uint> selectedChannels (ntrue(channelSelection));
      uint counter (0);
      
      for (uint channel(0); channel<nelem; channel++) {
	if (channelSelection(channel)) {
	  selectedChannels (counter) = channel;
	  counter++;
	}
      }
      
      return setSelectedChannels (selectedChannels);
    } else {
      std::cerr << "[DataReader::setSelectedChannels] ";
      std::cerr << "Wrong shape of channel selection array!"
		<< endl;
      std::cerr << " - FFT length              = " << fftLength_p << std::endl;
      std::cerr << " - nelem(channelSelection) = " << nelem << std::endl;
      return false;
    }
  }
  
}  // namespace LOPES -- end
