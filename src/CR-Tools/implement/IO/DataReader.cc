/*-------------------------------------------------------------------------*
 | $Id$ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2007                                                    *
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

#include <IO/DataReader.h>
#include <casa/Arrays/ArrayLogical.h>
#include <casa/Arrays/ArrayMath.h>

using std::cerr;
using std::cout;
using std::endl;

namespace CR {  //  Namespace CR -- begin
  
  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================
  
  //_____________________________________________________________________________
  //                                                                   DataReader
  
  /*!
    This is the default constructor for a new DataReader object; this will not
    enable the actual reading of data, but otherwise represents a fully
    operational object.
    
    \verbatim
    - blocksize   = 1
    - FFT length  = 1
    - nof. files  = 1
    - adc2voltage = [1]
    - fft2calfft  = [(1,0)]
    \endverbatim
  */
  DataReader::DataReader ()
    : TimeFreq(),
      streamsConnected_p(false)
  {
    init (TimeFreq::blocksize());
  }
  
  //_____________________________________________________________________________
  //                                                                   DataReader
  
  /*!
    \param blocksize   -- Size of a block of data, [samples]
  */
  DataReader::DataReader (uint const &blocksize)
    : TimeFreq(blocksize),
      streamsConnected_p(false)
  {
    init (TimeFreq::blocksize());
  }
  
  //_____________________________________________________________________________
  //                                                                   DataReader
  
  /*!
    \param blocksize       -- Blocksize, [samples]
    \param sampleFrequency -- Sample frequency in the ADC, [Hz]
    \param nyquistZone     -- Nyquist zone,  [1]
  */
  DataReader::DataReader (uint const &blocksize,
			  uint const &nyquistZone,
			  Double const &samplerate)
    : TimeFreq(blocksize,
	       samplerate,
	       nyquistZone),
      streamsConnected_p(false)
  {
    init (blocksize,
	  nyquistZone,
	  samplerate);
  }
  
  //_____________________________________________________________________________
  //                                                                   DataReader
  
  /*!
    \param blocksize   -- Size of a block of data, [samples]
    \param adc2voltage -- Multiplication factors for conversion from ADC values
                          to voltages
    \param fft2calfft  -- Multiplication factors for conversion from raw to
                          calibrated FFT
  */
  DataReader::DataReader (uint const &blocksize,
			  Vector<Double> const &adc2voltage,
			  Matrix<DComplex> const &fft2calfft)
    : TimeFreq(blocksize),
      streamsConnected_p(false)
  {
    init (TimeFreq::blocksize(),
	  TimeFreq::nyquistZone(),
	  TimeFreq::sampleFrequency());
    
    init (blocksize,
	  adc2voltage,
	  fft2calfft);
  }
  
  //_____________________________________________________________________________
  //                                                                   DataReader
  
  /*!
    \param blocksize   -- Size of a block of data, [samples]
    \param adc2voltage -- Multiplication factors for conversion from ADC values
           to voltages
    \param fft2calfft  -- Multiplication factors for conversion from raw to
           calibrated FFT
  */
  DataReader::DataReader (uint const &blocksize,
			  Matrix<Double> const &adc2voltage,
			  Matrix<DComplex> const &fft2calfft)
    : TimeFreq(blocksize),
      streamsConnected_p(false)
  {
    init (TimeFreq::blocksize(),
	  TimeFreq::nyquistZone(),
	  TimeFreq::sampleFrequency());
    
    init (blocksize,
	  adc2voltage,
	  fft2calfft);
  }
  
  //_____________________________________________________________________________
  //                                                                   DataReader
  
  /*!
    \todo Implementation incomplete - not all input parameter used

    \param filename    -- Name of the file from which to read in the data
    \param blocksize   -- Size of a block of data, [samples]
    \param var         -- Variable type as which the data are stored in the file
    \param adc2voltage -- Multiplication factors for conversion from ADC values
                          to voltages
    \param fft2calfft  -- Multiplication factors for conversion from raw to
                          calibrated FFT
  */
  template <class T> 
  DataReader::DataReader (String const &filename,
			  uint const &blocksize,
			  T const &var,
			  Vector<Double> const &adc2voltage,
			  Matrix<DComplex> const &fft2calfft)
    : TimeFreq(blocksize),
      streamsConnected_p(false)
  {
    init (TimeFreq::blocksize(),
	  TimeFreq::nyquistZone(),
	  TimeFreq::sampleFrequency());
    
    init (TimeFreq::blocksize(),
	  adc2voltage,
	  fft2calfft);
  }
  
  //_____________________________________________________________________________
  //                                                                   DataReader
  
  DataReader::DataReader (DataReader const &other)
    : TimeFreq()
  {
    copy (other);
  }
  
  // ============================================================================
  //
  //  Destruction
  //
  // ============================================================================
  
  //_____________________________________________________________________________
  //                                                                  ~DataReader
  
  DataReader::~DataReader ()
  {
    destroy();
  }
  
  //_____________________________________________________________________________
  //                                                                      destroy
  
  void DataReader::destroy ()
  {
    /*
     * We need to be a bit careful at this point; since it does not make any
     * sense trying to delete array for which never any memory has been
     * allocated, we first should check for indications, that indeed this might
     * have been the case.
     */
    if (streamsConnected_p) {
      if (fileStream_p != NULL) {
	// close all the data streams
	for (uint stream (0); stream<nofStreams_p; stream++) {
	  fileStream_p[stream].close();
	}
	// release the fileStream array
	delete [] fileStream_p;
      };  
      // release previously allocated memory
      delete [] iterator_p;
    }
  }
  
  // ============================================================================
  //
  //  Operators
  //
  // ============================================================================

  //_____________________________________________________________________________
  //                                                                    operator=
  
  DataReader& DataReader::operator= (DataReader const &other)
  {
    if (this != &other) {
      destroy ();
      copy (other);
    }
    return *this;
  }
  
  //_____________________________________________________________________________
  //                                                                         copy
  
  void DataReader::copy (DataReader const &other)
  {  
    blocksize_p       = other.blocksize_p;
    sampleFrequency_p = other.sampleFrequency_p;
    nyquistZone_p     = other.nyquistZone_p;
    
    try {
      // -- header -----------------------------------------
      header_p     = other.header_p;
      // -- Array dimensions -------------------------------
      fftLength_p    = other.fftLength_p;
      // -- conversion -------------------------------------
      haveADC2Voltage_p = other.haveADC2Voltage_p;
      haveFFT2CalFFT_p  = other.haveFFT2CalFFT_p;
      
      if (haveADC2Voltage_p) {
	ADC2Voltage_p.resize (other.ADC2Voltage_p.shape());
	ADC2Voltage_p    = other.ADC2Voltage_p;
      }

      if (haveFFT2CalFFT_p) {
	FFT2CalFFT_p.resize (other.FFT2CalFFT_p.shape());
	FFT2CalFFT_p     = other.FFT2CalFFT_p;
      }

      // -- Antennas ---------------------------------------
      antennas_p.resize (other.antennas_p.shape());
      antennas_p       = other.antennas_p;
      
      selectedAntennas_p.resize (other.selectedAntennas_p.shape());
      selectedAntennas_p = other.selectedAntennas_p;
      
      // -- Frequency channels -----------------------------
      selectedChannels_p.resize (other.selectedChannels_p.shape());
      selectedChannels_p = other.selectedChannels_p;
      
      selectChannels_p = other.selectChannels_p;
      hanningFilter_p  = other.hanningFilter_p;
      applyHanning_p   = other.applyHanning_p;
      
      // -- File streams -----------------------------------
      
      nofStreams_p = other.nofStreams_p;
      startBlock_p = other.startBlock_p;
      iterator_p   = new DataIterator[nofStreams_p];
      if (other.fileStream_p != NULL) {
	fileStream_p = new fstream[nofStreams_p];
	for (uint stream (0); stream<nofStreams_p; stream++) {
	  //other.fileStream_p[stream].tie((fileStream_p[stream]));
	};
	streamsConnected_p = true;
      } else {
	fileStream_p = NULL;
	streamsConnected_p = false;
      };
      
      try {
	for (uint stream (0); stream<nofStreams_p; stream++) {
	  iterator_p[stream]   = other.iterator_p[stream];
	}
      } catch (AipsError x) {
	cerr << x.getMesg() << endl;
      }
      
    } catch (AipsError x) {
      cerr << "[DataReader::copy]" << x.getMesg() << endl;
    }
  }
  
  // ============================================================================
  //
  //  Parameters
  //
  // ============================================================================

  //_____________________________________________________________________________
  //                                                                         init
  
  /*!
    \param blocksize -- Blocksize, [samples]
  */
  void DataReader::init (uint const &blocksize) 
  {
    uint nyquistZone       = nyquistZone_p;
    double sampleFrequency = sampleFrequency_p;
    
    init (blocksize,
	  nyquistZone,
	  sampleFrequency);
  }
  
  //_____________________________________________________________________________
  //                                                                         init
  
  /*!
    \param blocksize       -- Blocksize, [samples]
    \param nyquistZone     -- Nyquist zone
    \param sampleFrequency -- Sample frequency in the ADC step, [Hz]
  */
  void DataReader::init (uint const &blocksize,
			 uint const &nyquistZone,
			 Double const &sampleFrequency)
  {
    // Parameters stored in CR::TimeFreq
    TimeFreq::setBlocksize (blocksize);
    TimeFreq::setNyquistZone (nyquistZone);
    TimeFreq::setSampleFrequency (sampleFrequency);
    // Parameters stored in CR::DataReader
    fileStream_p      = NULL;
    startBlock_p      = 1;
    haveADC2Voltage_p = false;
    haveFFT2CalFFT_p  = false;
  }
  
  //_____________________________________________________________________________
  //                                                                         init
  
  /*!
    \param blocksize   -- Size of a block of data, [samples]
    \param adc2voltage -- Multiplication factors for conversion from ADC values
                          to voltages
    \param fft2calfft  -- Multiplication factors for conversion from raw to
                          calibrated FFT
  */
  void DataReader::init (uint const &blocksize,
			 Vector<Double> const &adc2voltage,
			 Matrix<DComplex> const &fft2calfft)
  {
    // convert vector of ADC2Voltage values to matrix ...
    uint sample (0);
    int antenna (0);
    int nofAntennas (adc2voltage.nelements());
    Matrix<double> adc2voltageMatrix (blocksize,nofAntennas);

    for (antenna=0; antenna<nofAntennas; antenna++) {
      for (sample=0; sample<blocksize; sample++) {
	adc2voltageMatrix (sample,antenna) = adc2voltage (antenna);
      }
    }
    
    // ... which then is forwarded to the next init function
    init (blocksize,
	  adc2voltageMatrix,
	  fft2calfft);
  }
  
  //_____________________________________________________________________________
  //                                                                         init
  
  void DataReader::init (uint const &blocksize,
			 Matrix<Double> const &adc2voltage,
			 Matrix<DComplex> const &fft2calfft)
  {
    /*
      Do some basic checking here: the provided numbers and array must be
      conformant - esp. the number of elements in 'adc2voltage' must correspond
      to the length of the second axis of 'fft2calfft'.
    */
    
    bool status (true);
    IPosition shapeADC (adc2voltage.shape());  // [blocksize,nofAntennas]
    IPosition shapeFFT (fft2calfft.shape());   // [fftLength,nofAntennas]
    
    /* Check of the number of antennas is consistent */
    if (shapeADC(1) != shapeFFT(1)) {
      cerr << "[DataReader::init] Inconsistent number of antennas!" << endl;
      cerr << " - shape(adc2voltage) = " << adc2voltage.shape() << endl;
      cerr << " - shape(fft2calfft)  = " << fft2calfft.shape() << endl;
      cerr << " => Rejecting conversion arrays!" << endl;
      status = false;
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
      try {
	TimeFreq::setBlocksize (blocksize);
      } catch (std::string message) {
	cerr << "[DataReader::init]" << message << endl;
	status = false;
      }
      
      /*
	Antenna selection: by default we enable all antennas
      */
      Vector<uint> antennas (shapeADC(1));
      Vector<uint> selectedAntennas (shapeADC(1));
      
      try {
	for (int n(0); n<shapeADC(1); n++) {
	  antennas(n) = selectedAntennas(n) = n;
	}
	
	setAntennas (antennas,
		     selectedAntennas);
      } catch (std::string message) {
	cerr << "[DataReader::init]" << message << endl;
	status = false;
      }
      
      /* book-keeping: memorize the number of data streams */
      nofStreams_p = shapeADC(1);
      
      /*
	Frequency channel selection: by default we enable all frequency channels
      */
      try {
	Vector<Bool> selectedChannels (fftLength_p);
	selectedChannels = True;
	
	setSelectedChannels (selectedChannels);
	selectChannels_p = False;
	
	// process provided parameters
	setADC2Voltage (adc2voltage);
	setFFT2calFFT (fft2calfft);
      } catch (std::string message) {
	cerr << "[DataReader::init]" << message << endl;
	status = false;
      }
      
      // -- Setup of the Hanning filter (disabled by default)
      
      setHanningFilter (0.0);
      
    }    
  }
  
  //_____________________________________________________________________________
  //                                                                         init
  
  void DataReader::init (uint const &blocksize,
			 Vector<uint> const &antennas,
			 Vector<Double> const &adc2voltage,
			 Matrix<DComplex> const &fft2calfft,
			 Vector<String> const &filenames,
			 DataIterator const *iterators)
  {
    Bool status (True);
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
      cerr << "[DataReader::init] Wrong blocksize or number of filenames!"
	   << endl;
      cerr << " - blocksize (internal)   = " << blocksize_p << endl;
      cerr << " - blocksize (streams)    = " << blocksize << endl;
      cerr << " - nof. Files             = " << nofStreams_p << endl;
      cerr << " - length of input vector = " << nofFiles << endl;
      //
      status = False;
    } else {
      // update the internal streams array
      fileStream_p = new fstream[nofStreams_p];
      iterator_p   = new DataIterator[nofStreams_p];
      // connect the streams to the files
      for (uint file(0); file<nofStreams_p; file++) {
	fileStream_p[file].open(filenames(file).c_str(), ios::in | ios::binary);
	iterator_p[file] = iterators[file];
	iterator_p[file].setBlocksize(blocksize);
      } 
      streamsConnected_p = true;
    }
    
  }

  //_____________________________________________________________________________
  //                                                                 setBlocksize

  /*!
    If the blocksize is changed the conversion arrays become invalid. The only
    real counter-measure is to inded mark the arrays as no longer valid, such
    that new values need to be provided for them as well, before they can be
    applied once more.

    \param blocksize -- Blocksize, [samples]
  */
  void DataReader::setBlocksize (uint const &blocksize)
  {
    /* Store the new value for the blocksize */
    TimeFreq::setBlocksize (blocksize);

    /* 
     * Book-keeping: if the blocksize is changed the conversion arrays become
     * invalid. The only real counter-measure is to inded mark the arrays as
     * no longer valid, such that new values need to be provided for them as well,
     * before they can be applied once more.
     */
    haveADC2Voltage_p = false;
    haveFFT2CalFFT_p  = false;
    
    ADC2Voltage_p.resize(1,1);
    FFT2CalFFT_p.resize(1,1);
    
    /* Update the blocksize parameter for the set of DataIterators. */
    for (unsigned int n(0); n<nofStreams_p; n++) {
      iterator_p[n].setBlocksize(blocksize);
    }
    
#ifdef DEBUGGING_MESSAGES
    cout << "[DataReader::setBlocksize(uint)]"                  << endl;
    cout << "-- blocksize          = " << blocksize_p           << endl;
    cout << "-- Have ADC2Voltage   = " << haveADC2Voltage_p     << endl;
    cout << "-- Have FFT2CalFFT    = " << haveFFT2CalFFT_p      << endl;
    cout << "-- shape(adc2voltage) = " << ADC2Voltage_p.shape() << endl;
    cout << "-- shape(fft2calfft)  = " << FFT2CalFFT_p.shape()  << endl;
#endif
  }
  
  //_____________________________________________________________________________
  //                                                                 setBlocksize

  /*!
    \param blocksize -- Blocksize, [samples]
    \param adc2voltage -- [sample,antenna] Weights to convert raw ADC samples to
                          voltages
  */
  void DataReader::setBlocksize (uint const &blocksize,
				 Matrix<double> const &adc2voltage)
  {
    DataReader::setBlocksize (blocksize);
    DataReader::setADC2Voltage (adc2voltage);
    
    /*
      Check and (if required) adjust the shape of the fft2calfft conversion
      factors array
    */
    try {
      IPosition shape = FFT2CalFFT_p.shape();
      if (uint(shape(0)) != fftLength_p) {
	// report what we are about to do
#ifdef DEBUGGING_MESSAGES
	cerr << "[DataReader::setBlocksize] WARNING!"
	     << " Need to re-adjust fft2calfft array and selected channels array to keep internals consistent."
	     << " The previously assigned values will be lost!"
	     << endl;
#endif
	// adjust the first axis of the array
	shape(0) = fftLength_p;
	// FIX: the selected # of channels is invalid when the number of FFT channels has changed.
	// Reset to all channels selected to prevent a crash down in setFFT2calFFT.
	// Note: the handling of # selected channels < # total channels is currently incorrect and needs attention...
	// See also setFFT2calFFT.

	// correct the channel selection array ...
	Vector<bool> frequencySelection (shape(0),true);
	setSelectedChannels(frequencySelection);
	// End fix.
	Matrix<DComplex> fft2calfftNew (shape,1.0);
	setFFT2calFFT (fft2calfftNew);
      }
    } catch (std::string message) {
      cerr << "[DataReader::setBlocksize] " << message << endl;
    }
  }

  //_____________________________________________________________________________
  //                                                                 setBlocksize

  /*!
    \brief Set the blocksize, \f$ N_{\rm Blocksize} \f$
    
    \param blocksize   -- Blocksize, [samples]
    \param adc2voltage -- [sample,antenna] Weights to convert raw ADC samples to
                          voltages
    \param fft2calfft  -- [channel,antenna]
  */
  void DataReader::setBlocksize (uint const &blocksize,
				 Matrix<double> const &adc2voltage,
				 Matrix<DComplex> const &fft2calfft)
  {
    DataReader::setBlocksize (blocksize);
    DataReader::setADC2Voltage (adc2voltage);
    DataReader::setFFT2calFFT (fft2calfft);
  }
  
  //_____________________________________________________________________________
  //                                                              frequencyValues

  Vector<Double> DataReader::frequencyValues (Bool const &onlySelected)
  {
    uint nofChannels (0);
    uint channel (0);
    Vector<Double> frequencies;
    
    // Get the values of all frequency channels
    casa::Vector<double> freq (TimeFreq::frequencyValues());
    
    if (onlySelected) {
      nofChannels = nofSelectedChannels();
      frequencies.resize (nofChannels);
      //
      for (channel=0; channel<nofChannels; channel++) {
	frequencies(channel) = freq(selectedChannels_p(channel));
      }
    } else {
      nofChannels = freq.size();
      frequencies.resize (nofChannels);
      //
      for (channel=0; channel<nofChannels; channel++) {
	frequencies(channel) = freq(channel);
      }
    }
    
    return frequencies;
  }

  //_____________________________________________________________________________
  //                                                              setHeaderRecord

  bool DataReader::setHeaderRecord ()
  {
    header_p.define("Observatory","UNDEFINED");
    header_p.define("Filesize",0);

    return true;
  }

  //_____________________________________________________________________________
  //                                                              setHeaderRecord

  bool DataReader::setHeaderRecord (Record const &header)
  {
    bool status (true);
    try {
      header_p.merge(header,RecordInterface::OverwriteDuplicates);
    } catch (AipsError x) {
      cerr << "[DataReader::setHeader]" << x.getMesg() << endl;
      status = false;
    };

    return status;
  }
  
  // ============================================================================
  //
  //  Navigation through the data volume
  //
  // ============================================================================

  //_____________________________________________________________________________
  //                                                                setStartBlock
  
  void DataReader::setStartBlock (uint const &startBlock)
  {
    if (startBlock>0) {
      startBlock_p = startBlock;
    } else {
      cerr << "[DataReader::setStartBlock] Invalid number for start block."
		<< endl;
      startBlock_p = 1;
    }
  }
  
  //_____________________________________________________________________________
  //                                                                     setBlock
  
  
  void DataReader::setBlock (uint const &block)
  {
    for (unsigned int n(0); n<nofStreams_p; n++) {
      iterator_p[n].setBlock(block);
    }
  }
  
  //_____________________________________________________________________________
  //                                                                    setStride

  void DataReader::setStride (uint const &stride)
  {
    for (unsigned int n(0); n<nofStreams_p; n++) {
      iterator_p[n].setStride(stride);
    }
  }
  
  //_____________________________________________________________________________
  //                                                                        shift

  std::vector<int> DataReader::shift ()
  {
    std::vector<int> s (nofStreams_p);

    for (unsigned int n(0); n<nofStreams_p; n++) {
      s[n] = iterator_p[n].shift();
    }

    return s;
  }
  
  //_____________________________________________________________________________
  //                                                                     setShift

  void DataReader::setShift (int const &shift)
  {
    for (unsigned int n(0); n<nofStreams_p; n++) {
      iterator_p[n].setShift(shift);
    }
  }
  
  //_____________________________________________________________________________
  //                                                                     setShift

  void DataReader::setShift (int const &shift,
			     unsigned int const &index)
  {
    if (index<nofStreams_p) {
      iterator_p[index].setShift(shift);
    } else {
      std::cerr << "[DataReader::setShift] Invalid index for stream!" << endl;
    }
  }
  
  //_____________________________________________________________________________
  //                                                                     setShift

  void DataReader::setShift (std::vector<int> const &shift)
  {
    if (shift.size() == nofStreams_p) {
      for (unsigned int n(0); n<nofStreams_p; n++) {
	iterator_p[n].setShift(shift[n]);
      } 
    } else {
      std::cerr << "[DataReader::setShift]"
		<< " Length of vector does not match nof. streams!" << endl;
    }
  }
  
  //_____________________________________________________________________________
  //                                                                    nextBlock

  void DataReader::nextBlock ()
  {
    for (unsigned int n(0); n<nofStreams_p; n++) {
      iterator_p[n].nextBlock();
    }
  }
  
  //_____________________________________________________________________________
  //                                                                    positions

  Vector<unsigned int> DataReader::positions ()
  {
    Vector<unsigned int> currentPositions (nofStreams_p);
    
    for (unsigned int n(0); n<nofStreams_p; n++) {
      currentPositions(n) = iterator_p[n].position();
    }
    
    return currentPositions;
  }
  
  // ==============================================================================
  //
  //  Methods
  //
  // ==============================================================================
  
  //_______________________________________________________________________________
  //                                                                             fx
  
  /*!
    \return fx -- [sample,antenna] Raw ADC time series, [Counts]
  */
  Matrix<Double> DataReader::fx ()
  {
    Matrix<Double> data (blocksize_p,nofSelectedAntennas());
    
    DataReader::fx (data);

    return data;
  }
  
  //_______________________________________________________________________________
  //                                                                             fx
  
  /*!
    \retval data -- [sample,antenna] Raw ADC time series, [Counts]
  */
  void DataReader::fx (Matrix<Double> &data)
  {
    casa::IPosition shape (2,blocksize_p,nofSelectedAntennas());

    if (data.shape() != shape) {
      data.resize(shape);
    }

    data = 0.0;
  }

  //_______________________________________________________________________________
  //                                                                        voltage
  
  /*!
    \return voltage -- [sample,antenna] Voltage time series, [Volt]
  */
  Matrix<Double> DataReader::voltage ()
  {
    Matrix<Double> data (blocksize_p,nofSelectedAntennas());
    
    DataReader::voltage (data);

    return data;
  }
  
  //_______________________________________________________________________________
  //                                                                        voltage
  
  /*!
    \retval voltage -- [sample,antenna] Voltage time series, [Volt]
  */
  void DataReader::voltage (Matrix<Double> &data)
  {
    casa::IPosition shape (2,blocksize_p,nofSelectedAntennas());
    casa::Matrix<Double> in (fx());
    uint sample (0);
    uint antenna(0);
    
    /* Check the shape of the array returning the data */
    if (in.shape() == shape) {
      if (data.shape() != shape) {
	data.resize(shape);
      } 
    } else {
      std::cerr << "[DataReader::voltage] Mismatching array shapes!" << std::endl;
      std::cerr << "-- shape(fx)      = " << in.shape() << std::endl;
      std::cerr << "-- shape(voltage) = " << shape      << std::endl;
    }
    
    /* Keep in mind here, that there might be a set of antennas selected */
    if (haveADC2Voltage_p) {
      for (antenna=0; antenna<nofSelectedAntennas(); antenna++) {
	for (sample=0; sample<blocksize_p; sample++) {
	  data(sample,antenna) = in(sample,antenna)*ADC2Voltage_p(sample,selectedAntennas_p(antenna));
	}
      }
    } else {
      for (antenna=0; antenna<nofSelectedAntennas(); antenna++) {
	for (sample=0; sample<blocksize_p; sample++) {
	  data(sample,antenna) = in(sample,antenna);
	}
      }
    }
  }
  
  //_______________________________________________________________________________
  //                                                                            fft
  
  Matrix<DComplex> DataReader::fft ()
  {
    casa::IPosition shape (2,
			   fftLength_p,
			   nofSelectedAntennas());
    Matrix<DComplex> data (shape);

    DataReader::fft (data);

    return data;
  }
  
  //_______________________________________________________________________________
  //                                                                            fft
  
  /*!
    \return fft -- [channel,antenna] Raw FFT of the voltage time series

    The function also takes care of aligning the frequency channels, depending
    on the Nyquist zone during the A/D conversion; if necessary frequency
    channels will be rearranged to return them in ascending order.

    The individual steps performed on the data are:
    <ol>
      <li>[optional] Apply Hanning filter to the time-domain data.
      <li>Fourier transform to frequency-domain.
      <li>Frequency channel alignment depending on the Nyquist zone during the
      A/D conversion; if necessary frequency channels will be rearranged to
      return them in ascending order.
      <li>[optional] Frequency channel selection.
    </ol>

    <b>Note:</b> Use the <tt>invfft()</tt> function for the inverse FFT.
  */
  void DataReader::fft (Matrix<DComplex> &data)
  {
    IPosition shape (2);
    uint nofChannels;
    uint nofAntennas;
    Matrix<Double> in (voltage());
    /* Variables for the FFT */
    FFTServer<Double,DComplex> server(IPosition(1,blocksize_p),
				      FFTEnums::REALTOCOMPLEX);
    Vector <Double> inColumn;
    Vector<DComplex> outColumn;

    //____________________________________________
    // Book-keeping of array sizes

    if (selectChannels_p) {
      nofAntennas = nofSelectedAntennas();
      nofChannels = DataReader::nofSelectedChannels();
    } else {
      nofAntennas = nofSelectedAntennas();
      nofChannels = fftLength_p;
    }
    shape = casa::IPosition(2,nofChannels,nofAntennas);

    //____________________________________________
    // Check of array size
    
    if (data.shape() != shape) {
      data.resize(shape);
    }

    //____________________________________________
    // Conversion of data to frequency domain

    uint antenna(0);
    uint channel(0);

    for (antenna=0; antenna<nofAntennas; antenna++) {
      inColumn = in.column(antenna);
      // apply Hanning filter (optional)
      if (applyHanning_p == True) {
	hanningFilter_p.filter(inColumn);
      }
      // FFT the data block for the current antenna ...
      server.fft(outColumn,inColumn);
      // .. and copy the result, depending on the Nyquist zone setting
      switch (nyquistZone_p) {
      case 1:
      case 3:
      case 5:
	{
	  if (selectChannels_p) {
	    for (channel=0; channel<nofChannels; channel++) {
	      data(channel,antenna) = outColumn(selectedChannels_p(channel));
	    }
	  } else {
	    data.column(antenna) = outColumn;
	  }
	}
	break;
      case 2:
      case 4:
      case 6:
	{
	  if (selectChannels_p) {
	    for (channel=0; channel<fftLength_p; channel++) {
	      data(channel,antenna) = conj(outColumn(fftLength_p-channel-1));
	    }
	  } else {
	    for (channel=0; channel<fftLength_p; channel++) {
	      data(channel,antenna) = conj(outColumn(fftLength_p-selectedChannels_p(channel)-1));
	    }
	  }
	}
	break;
      }
    }
    
  }
  
  //_______________________________________________________________________________
  //                                                                         calfft

  /*!
    \retval calfft -- [channel,antenna] Calibrated FFT, i.e. spectra after
            correction for the antenna gain-curves.
  */
  Matrix<DComplex> DataReader::calfft ()
  {
    Matrix<DComplex> data (DataReader::nofSelectedChannels(),
			   DataReader::nofSelectedAntennas());

    DataReader::calfft (data);
    
    return data;
  }
  
  //_______________________________________________________________________________
  //                                                                         calfft
  
  /*!
    \return calfft -- [channel,antenna] Calibrated FFT, i.e. spectra after
            correction for the antenna gain-curves.
  */
  void DataReader::calfft (Matrix<DComplex> &data)
  {
    int antenna(0);
    int channel(0);
    int nofSelectedAntennas (DataReader::nofSelectedAntennas());
    int nofSelectedChannels (DataReader::nofSelectedChannels());
    casa::IPosition shape (2,nofSelectedChannels,nofSelectedAntennas);
    Matrix<DComplex> in (fft());

    /* Check the shape of the array returning the data */
    if (data.shape() != shape) {
      data.resize(shape);
    }
    
    /*
      Conversion from raw to calibrated FFT: as we may have selected on both
      antenna numbers and frequency channels, we cannot simply do
      out = in*FFT2CalFFT_p;
    */
    if (haveFFT2CalFFT_p) {
      for (antenna=0; antenna<nofSelectedAntennas; antenna++) {
	for (channel=0; channel<nofSelectedChannels; channel++) {
	  data (channel,antenna) = in(channel,antenna)
	    *FFT2CalFFT_p(selectedChannels_p(channel),selectedAntennas_p(antenna));
	}
      }
    }
    else {
      for (antenna=0; antenna<nofSelectedAntennas; antenna++) {
	for (channel=0; channel<nofSelectedChannels; channel++) {
	  data (channel,antenna) = in(channel,antenna);
	}
      }
    }

  }
  
  //_____________________________________________________________________________
  //                                                                       invfft
  
  Matrix<Double> DataReader::invfft (Matrix<DComplex> fftdata)
  {
    uint nants = nofSelectedAntennas();
    if (fftdata.ncolumn() != nants) {
      cerr << "[DataReader::invfft] Bad input: ncolumn != nofSelectedAntennas" << endl;
      return Matrix<Double>();
    };
    if (fftdata.nrow() != fftLength_p) {
      cerr << "[DataReader::invfft] Bad input: nrow != fftLength" << endl;
      return Matrix<Double>();
    };
    Matrix<Double> out(blocksize_p,
		       nofSelectedAntennas(),0.);
    try {      
      Vector <Double> outColumn(blocksize_p);
      Vector<DComplex> inColumn(fftLength_p);
      FFTServer<Double,DComplex> server(IPosition(1,blocksize_p),
					FFTEnums::REALTOCOMPLEX);
      for (uint antenna(0); antenna<nants; antenna++) {
	// .. and copy the result, depending on the Nyquist zone setting
	switch (nyquistZone_p) {
	case 1:
	case 3:
	  inColumn = fftdata.column(antenna);
	  break;
	case 2:
	  for (uint channel (0); channel<fftLength_p; channel++) {
	    inColumn(channel) = conj(fftdata((fftLength_p-channel-1),antenna));
	  }
	  break;
	}
	// inv-FFT the data block for the current antenna 
	server.fft(outColumn,inColumn);
	out.column(antenna) = outColumn;
      }
    } catch (AipsError x) {
      cerr << "[DataReader::invfft]" << x.getMesg() << endl;
    }
    
    return out;
  }

  //_____________________________________________________________________________
  //                                                                       invfft
  
  Vector<Double> DataReader::invfft (Vector<DComplex> fftdata)
  {
    Vector<Double> out(blocksize_p,0.);		
    if (fftdata.nelements() != fftLength_p) {
      cerr << "[DataReader::invfft] Bad input: nrow != fftLength" << endl;
      return out;
    };
    try {      
      Vector<DComplex> inColumn(fftLength_p);
      FFTServer<Double,DComplex> server(IPosition(1,blocksize_p),
					FFTEnums::REALTOCOMPLEX);
      switch (nyquistZone_p) {
      case 1:
      case 3:
	inColumn = fftdata;
	break;
      case 2:
#ifdef DEBUGGING_MESSAGES
	cout << "DataReader::invfft Nyquist Zone == 2, inverting FFT." << endl;
#endif
	for (uint channel (0); channel<fftLength_p; channel++) {
	  inColumn(channel) = conj(fftdata(fftLength_p-channel-1));
	}
	break;
      }
      // inv-FFT the data block for the current antenna 
      server.fft(out,inColumn);
    } catch (AipsError x) {
      cerr << "[DataReader::invfft]" << x.getMesg() << endl;
    }    
    return out;
  }

  //_____________________________________________________________________________
  //                                                                    ccSpectra
  
  /*!
    \param fromCalFFT -- Cross-correlation spectra from the calibrated FFT data? If
                         set to <i>False</i>, the cross-correlation is carried out on
			 the raw FFT.

    \return ccSpectra -- Data cube with the cross-correlation spectra,
                         [nfreq,nant,nant]
  */
  Cube<DComplex> DataReader::ccSpectra (Bool const &fromCalFFT)
  {
    casa::IPosition shape (3,
			   DataReader::nofSelectedChannels(),
			   DataReader::nofSelectedAntennas(),
			   DataReader::nofSelectedAntennas());
    Cube<DComplex> data (shape);

    DataReader::ccSpectra (data,fromCalFFT);

    return data;
  }

  //_____________________________________________________________________________
  //                                                                    ccSpectra
  
  /*!
    \retval ccSpectra -- Data cube with the cross-correlation spectra,
            [nfreq,nant,nant]
    \param fromCalFFT -- Cross-correlation spectra from the calibrated FFT data?
           If set to <i>False</i>, the cross-correlation is carried out on the
	   raw FFT.
  */
  void DataReader::ccSpectra (Cube<DComplex> &data,
			      Bool const &fromCalFFT)
  {
    int ant1 (0);
    int antenna2 (0);
    int channel  (0);
    casa::IPosition shape (3,
			   DataReader::nofSelectedChannels(),
			   DataReader::nofSelectedAntennas(),
			   DataReader::nofSelectedAntennas());
    Matrix<DComplex> in (shape(0),shape(1));
    
    /* Check the shape of the array returning the data */
    if (data.shape() != shape) {
      data.resize(shape);
    }

    /* Retrieve the frequency domain data for the individual antennas */
    if (fromCalFFT == True) {
      in = calfft();
    } else {
      in = fft();
    }

    /* Compute cross-correlation spectrum */
    for (antenna2=0; antenna2<shape(1); antenna2++) {
      for (ant1=0; ant1<shape(1); ant1++) {
	for (channel=0; channel<shape(0); channel++) {
	  data (channel,ant1,antenna2) = in(channel,ant1)*in(channel,antenna2);
	}
      }
    }
  }
  
  //_____________________________________________________________________________
  //                                                                 visibilities
  
  Matrix<DComplex> DataReader::visibilities (Bool const &fromCalFFT)
  {
    int antenna1 (0);
    int antenna2 (0);
    int channel  (0);
    int baseline (0);
    Matrix<DComplex> in (DataReader::nofSelectedChannels(),
			 DataReader::nofSelectedAntennas());
    
    if (fromCalFFT == True) {
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
  
  //_____________________________________________________________________________
  //                                                               setADC2Voltage
  
  /*!
    Kepp in mind, that there two possible shapes in which the data will be 
    accepted:
    <ol>
      <li>nelem == nofSelectedAntennas() : only conversion values for the 
      selected antennas are provided.
      <li>nelem == nofAntennas() : conversion values for all antennas in the 
      dataset are provided; this is the more secure method, as this will allow
      for later re-selection of antennas without affecting the proper conversion
      from ADC values to voltages.
    </ol>
    This routine assignes a single conversion value to each antenna; in case 
    you want to use this step to also flag in the time domain, use the method
    below.

    \param adc2voltage -- [sample,antenna] Weights to convert raw ADC samples to
                          voltages
  */
  void DataReader::setADC2Voltage (Vector<Double> const &adc2voltage)
  {
    bool ok (true);
    uint nofAntennas (adc2voltage.nelements());
    Matrix<double> arr (blocksize_p,nofAntennas);
    
    // insert the input values into the full parameter matrix
    try {
      uint antenna(0);
      uint sample (0);
      for (antenna=0; antenna<nofAntennas; antenna++) {
	for (sample=0; sample<blocksize_p; sample++) {
	  arr(sample,antenna) = adc2voltage(antenna);
	}
      }
    } catch (std::string message) {
      cerr << "[DataReader::setADC2Voltage] " << message << endl;
      ok = false;
    } 
    
    // forward the matrix to internal storage
    if (ok) {
      setADC2Voltage (arr);
    } else {
      cerr << "[DataReader::setADC2Voltage] Not forwarding ADC2Voltage values!" 
	   << endl;
    }
  }
  
  // --------------------------------------------------------------- setADC2Voltage
  
  void DataReader::setADC2Voltage (Matrix<Double> const &adc2voltage)
  {
    casa::IPosition shape (adc2voltage.shape());
    
    // check for the blocksize 
    if (shape(0) != int(blocksize_p)) {
      cerr << "[DataReader::setADC2Voltage] Shape information mistmatch!" << endl;
      cerr << " --> blocksize    = " << blocksize_p << endl;
      cerr << " --> nof. samples = " << shape(0)    << endl;
    } else {
      unsigned int sample (0);
      // check for the number of antennas
      if (shape(1) == int(nofSelectedAntennas())) {
	Matrix<Double> tmp (blocksize_p,nofAntennas());
	tmp = 1.0;
	// re-arrange the provided values to match the correct array shape
	for (int antenna(0); antenna<shape(1); antenna++) {
	  for (sample=0; sample<blocksize_p; sample++) {
	    tmp(sample,selectedAntennas_p(antenna)) = adc2voltage (sample,antenna);
	  }
	}
	// store the values 
	ADC2Voltage_p.resize(tmp.shape());
	ADC2Voltage_p = tmp;
	// book-keeping
	haveADC2Voltage_p = true;
      }
      else if (shape(1) == int(nofAntennas())) {
	ADC2Voltage_p.resize(shape);
	ADC2Voltage_p = adc2voltage;
	// book-keeping
	haveADC2Voltage_p = true;
      }
      else {
	cerr << "[DataReader::setADC2Voltage] Mismatching array shapes" << endl;
	cerr << " --> shape(adc2voltage)     = " << shape                 << endl;
	cerr << " --> nof. antennas          = " << nofAntennas()         << endl;
	cerr << " --> nof. selected antennas = " << nofSelectedAntennas() << endl;
      }
    }
  }
  
  // ---------------------------------------------------------------- setFFT2calFFT
  
  void DataReader::setFFT2calFFT (Matrix<DComplex> const &fft2calfft)
  {
    bool status (true);
    IPosition shape (fft2calfft.shape());  //  [channel,antenna]
    unsigned int nofChannels (fftLength_p);
    unsigned int  nofAntennas         = DataReader::nofAntennas();
    unsigned int  nofSelectedAntennas = DataReader::nofSelectedAntennas();
    unsigned int  nofSelectedChannels = DataReader::nofSelectedChannels();
    
    /*
      Check consistence between array with conversion factors and the array holding
      the selected channels; of course the latter once cannot have more elements as
      the first one - if this is the case though, we have to correct this.
    */
    
    if (uint(shape(0)) < nofSelectedChannels) {
      // report what we are about to to
      cerr << "[DataReader::setFFT2calFFT] WARNING!"
	   << " Need to re-adjust frequencySelection array to keep internals consistent."
	   << " The previously assigned values will be lost!"
	   << endl;
      // correct the channel selection array ...
      Vector<bool> frequencySelection (shape(0),true);
      setSelectedChannels(frequencySelection);
      // ... and update the variable used for further processing
      nofSelectedChannels = DataReader::nofSelectedChannels();
    }
    
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
      cerr << " -- shape(fft2calfft)       = " << shape                 << endl;
      cerr << " -- FFT output length       = " << fftLength_p           << endl;
      cerr << " -- nof. selected channels  = " << nofSelectedChannels   << endl;
      cerr << " -- nof. antennas           = " << nofAntennas           << endl;
      cerr << " -- nof. selected antennas  = " << nofSelectedAntennas   << endl;
      //
      status = false;
    }
    
#ifdef DEBUGGING_MESSAGES
    cout << "[DataReader::setFFT2calFFT (Matrix<DComplex>)]" << endl;
    cout << " -- FFT output length .... = " << fftLength_p           << endl;
    cout << " -- nofChannels .......... = " << nofChannels         << endl;
    cout << " -- nof. selected channels = " << nofSelectedChannels   << endl;
    cout << " -- nofAntennas .......... = " << nofAntennas         << endl;
    cout << " -- nofSelectedAntennas .. = " << nofSelectedAntennas << endl;
    cout << " -- shape(fft2calfft) .... = " << shape                 << endl;
#endif
    
    /*
      If all the test we have been running up to this point hacve succeeded,
      we are clear to go copy the provided input values to internal storage
    */
    if (status) {
      // adjust the size of the internal array
      FFT2CalFFT_p.resize (fftLength_p,nofAntennas);
      FFT2CalFFT_p = 1.0;
      // NB. This loop cannot be right, as the dimension of selectedChannels_p can vary, 
      // while looping over all channels and antennas... Causes segfault when not all channels are selected.
      for (uint antenna(0); antenna<nofAntennas; antenna++) {
	for (uint channel(0); channel<nofChannels; channel++) {
	  FFT2CalFFT_p (selectedChannels_p(channel),selectedAntennas_p(antenna))
	    = fft2calfft(channel,antenna);
	}
      }
    }
  }

  //_____________________________________________________________________________
  //                                                             setHanningFilter

  void DataReader::setHanningFilter (double const &alpha)
  {
    if (alpha == 0.0) {
      applyHanning_p = False;
    } else {
      HanningFilter<Double> tmp (blocksize_p,alpha);
      hanningFilter_p = tmp;
      applyHanning_p = True;
    }
  }
  
  //_____________________________________________________________________________
  //                                                             setHanningFilter

  void DataReader::setHanningFilter (double const &alpha,
				     uint const &beta)
  {
    if (alpha == 0.0) {
      applyHanning_p = False;
    } else {
      HanningFilter<Double> tmp (blocksize_p,
				 alpha,
				 beta);
      hanningFilter_p = tmp;
      applyHanning_p = True;
    }
  }
  
  //_____________________________________________________________________________
  //                                                             setHanningFilter

  void DataReader::setHanningFilter (double const &alpha,
				     uint const &beta,
				     uint const &betaRise, 
				     uint const &betaFall)
  {
    if (alpha == 0.0) {
      applyHanning_p = False;
    } else {
      HanningFilter<Double> tmp (blocksize_p,
				 alpha,
				 beta,
				 betaRise,
				 betaFall);
      hanningFilter_p = tmp;
      applyHanning_p = True;
    }
  }
  
  // ==============================================================================
  //
  //  Antennas and antenna selection
  //
  // ==============================================================================
  
  //_____________________________________________________________________________
  //                                                                  setAntennas

  bool DataReader::setAntennas (Vector<uint> const &antennas)
  {
    bool status (true);
    
    /*
      We might need additional checking against the number of elements in
      adc2voltage, in case the latter is defined.
    */
    
    antennas_p.resize (antennas.shape());
    antennas_p = antennas;
    
    return status;
  }
  
  //_____________________________________________________________________________
  //                                                                  setAntennas

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
  
  //_____________________________________________________________________________
  //                                                                  setAntennas

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
  
  //_____________________________________________________________________________
  //                                                          setSelectedAntennas

  Bool DataReader::setSelectedAntennas (Vector<uint> const &antennaSelection,
					bool const &absolute)
  {
    Bool status (True);
    
    // check if the arrays size is correct
    
    try {
      if (antennaSelection.nelements() > antennas_p.nelements()) {
	cerr << "[DataReader::setSelectedAntennas] Too many selected antennas!"
	     << endl;
	cerr << " shape(antennas)         = " << antennas_p.shape()       << endl;
	cerr << " shape(antennaSelection) = " << antennaSelection.shape() << endl;
	status = False;
      } else if (absolute) {
	selectedAntennas_p.resize(antennaSelection.shape());
	selectedAntennas_p = antennaSelection;
      } else {
	cerr << "[DataReader::setSelectedAntennas] Error:" << endl;
	cerr << "  Non absolute antennaSelection not implemented jet!" << endl;
	status = False;
      };
    } catch (AipsError x) {
      cerr << "[DataReader::setSelectedAntennas]" << x.getMesg() << endl;
      status = False;
    }
    
    return status;
  }
  
  //_____________________________________________________________________________
  //                                                          setSelectedAntennas

  Bool DataReader::setSelectedAntennas (Vector<Bool> const &antennaSelection)
  {
    uint nelem (antennaSelection.nelements());
    
    /*!
      We need to check the number of elements in the selection array; of course
      the number of selected antennas should always be <= the total number of
      antennas. If this condition is not given, do not accept the input array
      and exit with "False".
    */
    if (nelem == nofAntennas()) {
      Vector<uint> selectedAntennas (ntrue(antennaSelection));
      uint counter (0);
      
      //    antennaSelection_p.resize(nelem);
      //    antennaSelection_p = antennaSelection;
      
      for (uint antenna(0); antenna<nelem; antenna++) {
	if (antennaSelection(antenna)) {
	  selectedAntennas (counter) = antenna;
	  counter++;
	}
      }
      
      return setSelectedAntennas (selectedAntennas);
    } else {
      cerr << "[DataReader::setSelectedAntennas]"               << endl;
      cerr << " - Incorrect number of antenna selection flags!" << endl;
      cerr << " - nof. antennas = " << nofAntennas()            << endl;
      cerr << " - nof. flags    = " << nelem                    << endl;
      return False;
    }
  }
  
  // ------------------------------------------------------------ antennaSelection
  
  Vector<bool> DataReader::antennaSelection () const
  {
    Vector<bool> antennaSelectionVec(nofAntennas(),False);
    for (uint antenna(0); antenna<nofSelectedAntennas(); antenna++) {
      antennaSelectionVec(selectedAntennas_p(antenna)) = True;
    };
    return antennaSelectionVec;
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
  
  // ============================================================================
  //
  //  Frequency channels and frequency channel selection
  //
  // ============================================================================
  
  // -------------------------------------------------------- setSelectedChannels
  
  Bool DataReader::setSelectedChannels (Vector<uint> const &channelSelection)
  {
#ifdef DEBUGGING_MESSAGES
    cout << "[DataReader::setSelectedChannels (Vector<uint>)]" << endl;
    cout << " -- channelSelection = " << channelSelection.shape() << endl;
#endif 
    Bool status (True);
    
    selectedChannels_p.resize (channelSelection.shape());
    selectedChannels_p = channelSelection;
    
    selectChannels_p = true;
    
    return status;
  }
  
  // -------------------------------------------------------- setSelectedChannels

  Bool DataReader::setSelectedChannels (Vector<Bool> const &channelSelection)
  {
#ifdef DEBUGGING_MESSAGES
    cout << "[DataReader::setSelectedChannels (Vector<bool>)]" << endl;
    cout << " -- channelSelection = " << channelSelection.shape() << endl;
#endif 
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
      cerr << "[DataReader::setSelectedChannels] ";
      cerr << "Wrong shape of channel selection array!"
		<< endl;
      cerr << " - FFT length              = " << fftLength_p << endl;
      cerr << " - nelem(channelSelection) = " << nelem << endl;
      return False;
    }
  }

  //_____________________________________________________________________________
  //                                                                      summary
  
  void DataReader::summary (std::ostream &os)
  {
    IPosition fxShape;
    IPosition voltageShape;
    IPosition fftShape;
    IPosition calfftShape;
    
    fxShape      = fx().shape();
    voltageShape = voltage().shape();
    fftShape     = fft().shape();
    calfftShape  = calfft().shape();
    
    os << "[DataReader::summary]" << endl;
    // basic parameters
    os << " -- blocksize ............ = " << blocksize_p  << endl;
    os << " -- FFT length ........... = " << fftLength_p  << endl;
    os << " -- file streams connected?  " << streamsConnected_p << endl;
    if (streamsConnected_p) {
      os << " -- nof. file streams .... = " << nofStreams() << endl;
      os << " -- stream positions ..... = " << positions()  << endl;
    }
    // conversion factors
    os << " -- shape(adc2voltage) ... = " << ADC2Voltage().shape() << endl;
    os << " -- shape(fft2calfft) .... = " << fft2calfft().shape()  << endl;
    // output data
    os << " -- nof. antennas ........ = " << nofAntennas()         << endl;
    os << " -- nof. selected antennas = " << nofSelectedAntennas() << endl;
    os << " -- nof. selected channels = " << nofSelectedChannels() << endl;
    os << " -- shape(fx) ............ = " << fxShape               << endl;
    os << " -- shape(voltage) ....... = " << voltageShape          << endl;
    os << " -- shape(fft) ........... = " << fftShape              << endl;
    os << " -- shape(calfft) ........ = " << calfftShape           << endl;
  }

}  // Namespace CR -- end
