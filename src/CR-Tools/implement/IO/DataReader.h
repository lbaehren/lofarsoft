/*-------------------------------------------------------------------------*
 | $Id: template-class.h,v 1.20 2007/06/13 09:41:37 bahren Exp           $ |
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

#ifndef DATAREADER_H
#define DATAREADER_H

#include <iostream>
#include <fstream>

#include <crtools.h>

#include <casa/iostream.h>
#include <casa/fstream.h>
#include <casa/string.h>
#include <casa/Arrays.h>
#include <casa/Arrays/Array.h>
#include <casa/Arrays/ArrayIO.h>
#include <casa/Arrays/Cube.h>
#include <casa/Arrays/IPosition.h>
#include <casa/Arrays/Vector.h>
#include <casa/BasicSL/Complex.h>
#include <casa/Containers/Record.h>
#include <casa/Quanta.h>
#include <scimath/Mathematics.h>
#include <scimath/Mathematics/FFTServer.h>

#include <Coordinates/TimeFreq.h>
#include <IO/DataIterator.h>
#include <Math/HanningFilter.h>

using namespace std;
using namespace casa;
using namespace CR;

namespace CR {  //  Namespace CR -- begin
  
  /*!
    \class DataReader
    
    \ingroup IO
    
    \brief Brief description for class DataReader
    
    \author Lars B&auml;hren
    
    \date 2006/05/04
    
    \test tDataReader.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
      <li>[AIPS++] <a href="http://aips2.nrao.edu/docs/scimath/implement/Mathematics/FFTServer.html">FFTServer</a>
      <li>[CR] DataIterator
    </ul>
    
    For a set of higher level construction/helper routines see DataReaderTools.
    
    <h3>Motivation</h3>
    
    Reading in data, processing them within the LOPES-Tools kernel (data-kernel.g)
    and passing them to a Glish client has proven to be the main bottleneck of
    large computations; typically the time spent on this part is much larger than
    e.g. the beamforming for the computation of a Skymap. In order to overcome
    this problem, (a) as little as possible computation should be done in Glish
    and (b) as little as possible data should be passed over the Glish bus
    connecting the (present) LOPES-Tools data kernel and the client program.
    
    The original motivation for designing this class was to have a facility for
    fast input of raw data (ADC values after signal conversion); recently the code
    has been expanded to also handle LOPES event files and -- at a later point --
    data formats as generated by the Aviary plugin infrastructure.
    
    <h3>Synopsis</h3>
    
    The functionality and logic for dealing with a certain data set is distributed
    over the base class (DataReader) and the derived classes (e.g. ITS_Capture,
    LopesEvent).
    
    <ul>
      <li><b>Functions required in derived classes</b>
      <table border="0" cellpadding="3">
        <tr bgcolor="silver">
	  <td width="15%">Function</td>
	  <td>Description</td>
	</tr>
	<tr valign="top">
	  <td bgcolor="orange">DataReader::setStreams()</td>
	  <td>
	  This function serves as the central point where the input streams are
	  set up to read in the data (from disk); a function prototype is
	  provided in the base class as a virtual function, but then needs to
	  be reimplemented in the derived classes to actually set the streams
	  \code
       Bool LopesEvent::setStreams ()
       {
         bool status (true);
       
	     uint nofSamples (blocksize_p);
	     Vector<uint> antennas (length_);
	     Vector<Double> adc2voltage (DataReader::adc2voltage());
	     Matrix<DComplex> fft2calfft (DataReader::fft2calfft());
	     Vector<String> filenames (length_);
	     DataIterator *iterator;

	     // set and check the variables
	
	     // initialize the internals of the underlying DataReader object
	     DataReader::init (nofSamples,
	                       antennas,
			       adc2voltage,
			       fft2calfft,
			       filenames,
			       iterator);
	
	     return status;
       }
       \endcode
       </td>
     </tr>
    <tr valign="top">
      <td bgcolor="red">DataReader::init()</td>
      <td>
      This is the core function by which the derived classes initialize the
      underlying DataReader object (see example above) -- it <b>must</b> be 
      called to ensure the internal structure of framework provided by the 
      DataReader is intact.
      </td>
    </tr>
    <tr valign="top">
      <td bgcolor="green">DataReader::setHanningFilter()</td>
      <td>
      A HanningFilter can optionally be inserted into the signal chain; by
      default it is switched off, so you will need to activate it first (see
      tDataReader.cc for further examples):
      \code
      {
        Double alpha (0.5);
	
	DataReader dr  (blocksize, adc2voltage, fft2calfft);
	dr.setHanningFilter (alpha);
	
	Matrix<DComplex> fft_filtered (dr.fft());
      }
      \endcode
      </td>
    </tr>
  </table>
    <li><b>Fields in the Header Record.</b><br>
    This is the list of (mandatory) fields in the header record (as accessed with
    <tt>dr.header()</tt>) of a DataReader object. The mandatory fields have to be
    set by all child classes of the DataReader in order to be usable by the
    (upcoming) standard tools.
    <table>
      <tr>
        <td class="indexkey">Field Name
	<td class="indexkey">mandatory?
	<td class="indexkey">Data Type
	<td class="indexkey">Desctription
      </tr>
      <tr>
        <td class="indexkey">Date
	<td>yes
	<td>uInt
	<td>Date of the observation. Standard unix date i.e. (GMT-)seconds since
	1.1.1970
      </tr>
      <tr>
	<td class="indexkey">AntennaIDs
	<td>yes
	<td>Vector<Int>
	<td>The IDs of the antennas, i.e. an unique number for each channel.
      </tr>
      <tr>
	<td class="indexkey">Observatory
	<td>yes
	<td>String
	<td>Name of the Observatory, e.g. LOPES, LORUN, ITS, etc.
      </tr>
    </table>
  </ul>

  <h3>Example(s)</h3>

  Using the DataReader with a client program as of this data requires two
  segments of code to be present:
  <ul>
    <li><b>Glish:</b> Your Glish script needs to set up a records containing
    all necessary information required for the successful creation of a
    DataReader object; this is done via
    \code
    RecordForDataReader (rec,param);
    \endcode
    where <tt>rec</tt> is (a reference to) the data record and <tt>param</tt>
    is the record passed along with the Glish bus event.
    <li><b>C/C++:</b> While the DataReader class contains the general framework
    for the processing of the data up to cross-correlation spectra, the data
    type specific details are implemented in a set of separate classes (e.g.
    ITSCapture, LopesEvent, ...). Parsing of the record passed along with the
    Glish event and proper construction of a DataReader object is wrapped by
    DataReaderTools::assignFromRecord -- therefor all you client needs is
    \code
    DataReader *dr;
    dr = DataReaderTools::assignFromRecord (glishRec,true);
    \endcode
    where of course <tt>dr</tt> is a pointer to a DataReader object,
    <tt>glishRec</tt> is the record passed along with the Glish event and the
    optional parameter <tt>verbose=true</tt> enables the verbose mode of 
    DataReaderTools::assignFromRecord. After this access to the data is as simple
    as:
    \code
    Matrix<Double> fx (dr->fx());
    Matrix<Double> voltage (dr->voltage());
    Matrix<DComplex> fft (dr->fft());
    Matrix<DComplex> calfft (dr->calfft());
    \endcode
   </ul>
  */
  class DataReader : public TimeFreq {
    
    //! Conversion from ADC values to voltages, [sample,antenna]
    Matrix<Double> adc2voltage_p;
    
    //! Conversion from raw to calibrated FFT [channel,antenna]
    Matrix<DComplex> fft2calfft_p;
    
    //! Selection of the antennas
    Vector<uint> antennas_p;
    
    //! Enable/Disable frequency channel selection
    Bool selectChannels_p;
    
    //! Hanning filter to be applied to the time-domain data (optional)
    HanningFilter<Double> hanningFilter_p;
    
    //! Apply the Hanning filter to the data?
    Bool applyHanning_p;
    
    //! At which block of the data volume do we start reading data
    uint startBlock_p;
    
  protected:
    
    //! Record structure to store the meta data.
    Record header_p;
    
    // ------------------------------------------------------------- data streams
    
    //! Number of filestream, from which data are read
    uint nofStreams_p;
    
    //! File streams connecting to the stored data.
    std::fstream *fileStream_p;
    
    //! Book-keeping: have the streams been connected?
    bool streamsConnected_p;
    
    //! Array of DataIterator objects for handling progressing through data volume
    DataIterator *iterator_p;
    
    /*!
      \brief Set up the streams for reading in the data
      
      \return status -- Status of the operation; returns <tt>false</tt> if an
              error was encountered.
    */
    virtual bool setStreams () { return true;};
    
    /*!
      \brief Set the record collecting header information.

      \return status -- Status of the operation; returns <tt>false</tt> in case an
              error was encountered.
    */
    virtual bool setHeaderRecord ();
    
    /*!
      \brief Initialization of internal parameters
      
      \param blocksize   -- Size of a block of data, [samples]
      \param adc2voltage -- [antenna] Multiplication factors for conversion
                            from ADC values to voltages
      \param fft2calfft  -- [channel,antenna] Multiplication factors for
                            conversion from raw to calibrated FFT
    */
    void init (uint const &blocksize,
	       Vector<Double> const &adc2voltage,
	       Matrix<DComplex> const &fft2calfft);
    
    /*!
      \brief Initialization of internal parameters
      
      \param blocksize   -- Size of a block of data, [samples]
      \param adc2voltage -- [sample,antenna] Multiplication factors for
                            conversion from ADC values to voltages
      \param fft2calfft  -- [channel,antenna] Multiplication factors for
                            conversion from raw to calibrated FFT
    */
    void init (uint const &blocksize,
	       Matrix<Double> const &adc2voltage,
	       Matrix<DComplex> const &fft2calfft);
    
    /*!
      \brief Initialize the internal structure of the DataReader
      
      \param blocksize   -- Size of a block of data, [samples]
      \param antennas    -- Antennas included in the experiment/observation and
                            containing valid data files
      \param adc2voltage -- Multiplication factors for conversion from ADC values
                            to voltages
      \param fft2calfft  -- Multiplication factors for conversion from raw to
                            calibrated FFT
      \param filenames   -- Names of the files from which to read in the data
      \param iterators   -- Set of DataIterator objects used for navigation within
                            the file streams.
      
      This is the primary method for the derived classes to initialize the
      underlying DataReader object handling conversion of data.
    */
    void init (uint const &blocksize,
	       Vector<uint> const &antennas,
	       Vector<Double> const &adc2voltage,
	       Matrix<DComplex> const &fft2calfft,
	       Vector<String> const &filenames,
	       DataIterator const *iterators);
    
    // ----------------------------------------------------- (selected of) antennas
    
    //! Selection of the antennas
    Vector<uint> selectedAntennas_p;
    
    // ----------------------------------------------------- (selected of) channels
    
    //! Selection of the frequency channels
    Vector<uint> selectedChannels_p;
    
  public:
    
    // --------------------------------------------------------------- Construction
    
    /*!
      \brief Default constructor
      
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
    DataReader ();
    
    /*!
      \brief Argumented constructor
      
      \param blocksize   -- Size of a block of data, [samples]
    */
    DataReader (uint const &blocksize);
    
    /*!
      \brief Argumented constructor
      
	  \param blocksize       -- Blocksize, [samples]
	  \param sampleFrequency -- Sample frequency in the ADC, [Hz]
	  \param nyquistZone     -- Nyquist zone,  [1]
	*/
    DataReader (uint const &blocksize,
		uint const &nyquistZone,
		Double const &samplerate);
    
  /*!
    \brief Argumented constructor

    \param blocksize   -- Size of a block of data, [samples]
    \param adc2voltage -- Multiplication factors for conversion from ADC values
                          to voltages
    \param fft2calfft  -- Multiplication factors for conversion from raw to
                          calibrated FFT
  */
  DataReader (uint const &blocksize,
	      Vector<Double> const &adc2voltage,
	      Matrix<DComplex> const &fft2calfft);

  /*!
    \brief Argumented constructor

    \param blocksize   -- Size of a block of data, [samples]
    \param adc2voltage -- Multiplication factors for conversion from ADC values
                          to voltages
    \param fft2calfft  -- Multiplication factors for conversion from raw to
                          calibrated FFT
  */
  DataReader (uint const &blocksize,
	      Matrix<Double> const &adc2voltage,
	      Matrix<DComplex> const &fft2calfft);

  /*!
    \brief Argumented constructor

    \todo implentation incomplete

    \param filename    -- Name of the file from which to read in the data
    \param blocksize   -- Size of a block of data, [samples]
    \param var         -- Variable type as which the data are stored in the file
    \param adc2voltage -- Multiplication factors for conversion from ADC values
                          to voltages
    \param fft2calfft  -- Multiplication factors for conversion from raw to
                          calibrated FFT
  */
  template <class T> 
    DataReader (String const &filename,
		uint const &blocksize,
		T const &var,
		Vector<Double> const &adc2voltage,
		Matrix<DComplex> const &fft2calfft);
  
  /*!
    \brief Copy constructor

    \todo Based on (a) the problem of creating a copy of a stream - what does
    this actually mean - and (b) the triggered discussion with Marcel and Ger
    there a good argumentes to question while we should have a copy
    constructor/function for this class; as long as we are just read from disk
    files things can be done conceptually, but as soon as we make the
    generalization to input streams, copy no longer makes sense.

    \param other -- Another DataReader object from which to create this new
                    one.
  */
  DataReader (DataReader const &other);

  // ---------------------------------------------------------------- Destruction

  /*!
    \brief Destructor
  */
  virtual ~DataReader ();

  // ------------------------------------------------------------------ Operators

  /*!
    \brief Overloading of the copy operator

    \param other -- Another DataReader object from which to make a copy.
  */
  DataReader& operator= (DataReader const &other); 

  // ----------------------------------------------------------------- Parameters
  
  /*!
    \brief Set the blocksize, \f$ N_{\rm Blocksize} \f$
    
    \param blocksize -- Blocksize, [samples]
  */
  void setBlocksize (uint const &blocksize);

  /*!
    \brief Set the blocksize, \f$ N_{\rm Blocksize} \f$
    
    \param blocksize -- Blocksize, [samples]
    \param adc2voltage -- [sample,antenna] Weights to convert raw ADC samples to
                          voltages
  */
  void setBlocksize (uint const &blocksize,
		     Matrix<double> const &adc2voltage);
  
  /*!
    \brief Set the blocksize, \f$ N_{\rm Blocksize} \f$
    
    \param blocksize   -- Blocksize, [samples]
    \param adc2voltage -- [sample,antenna] Weights to convert raw ADC samples to
                          voltages
    \param fft2calfft  -- [channel,antenna]
  */
  void setBlocksize (uint const &blocksize,
		     Matrix<double> const &adc2voltage,
		     Matrix<DComplex> const &fft2calfft);
  
  /*!
    \brief Provide a summary of the internal parameters to standard output
   */
  inline void summary () {
    summary (std::cout);
  }
  
  /*!
    \brief Provide a summary of the internal parameters
    
    \param os -- Output stream, to which the summary is written
  */
  void summary (std::ostream &os);
  
  /*!
    \brief Get the number of files to which the data streams are connected

    \return nofStreams -- Number of streams used for reading in the data.
  */
  inline uint nofStreams () const {
    return nofStreams_p;
  }

  /*!
    \brief Get the output length of the FFT
    
    \return fftLength -- Output length of the FFT, [Samples]
  */
  inline uint fftLength () const {
    return fftLength_p;
  }

  /*!
    \brief Weights for conversion from raw ADC samples to voltages

    \return fx2voltage -- Weights to convert raw ADC samples to voltages
  */
  inline Matrix<Double> ADC2Voltage () const {
    return adc2voltage_p;
  }

  /*!
    \brief Set the weights for conversion from raw ADC samples to voltages

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
  void setADC2Voltage (Vector<Double> const &adc2voltage);

  /*!
    \brief Set the weights for conversion from raw ADC samples to voltages

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

    Example:
    \code
    int nofAntennas;
    int blocksize;
    // Create DataReader object
    DataReader dr;
    // 
    // .. a number of operations on the new object
    //

    // create matrix to store the conversion factors
    Matrix<double> adc2voltage (blocksize,nofAntennas);
    // assign some values to the array
    adc2voltage = some_function();
    // store the conversion factors inside the DataReader object
    dr.setADC2Voltage (adc2voltage);
    \endcode

    \param fx2voltage -- [blocksize,antennas] Weights to convert raw ADC samples
                         to voltages
  */
  void setADC2Voltage (Matrix<Double> const &adc2voltage);

  /*!
    \brief Weights for conversion from raw to calibrated FFT

    \return fft2calfft -- Weights to convert the output of the Fourier
                          transform on the voltages to calibrated spectra,
			  accounting for the slope of the bandpass filter.
  */
  inline Matrix<DComplex> fft2calfft () const {
    return fft2calfft_p;
  }

  /*!
    \brief Set the weights for conversion from raw to calibrated FFT

    \param fft2calfft -- Weights to convert the output of the Fourier
                         transform on the voltages to calibrated spectra,
			 accounting for the slope of the bandpass filter.
  */
  void setFFT2calFFT (Matrix<DComplex> const &fft2calfft);

  /*!
    \brief Get the record with the header information

    \return header -- Record containing the header information
  */
  inline Record headerRecord () const {
    return header_p;
  };
  
  /*!
    \brief Set the record with the header information
    
    \param header -- Record containing the header information
    
    \return status -- Status of the operation; returns <tt>false</tt> in case an
            error was encountered.
  */
  bool setHeaderRecord (Record const &header);

  // ----------------------------------------- Navigation through the data volume

  /*!
    \brief Get the number of the block at which we start reading the data

    \param startBlock -- The number of the block at which we start reading the
                         data
  */
  void setStartBlock (uint const &startBlock);

  /*!
    \brief Get the counter for the current block of data

    \return block -- The counter for the current block of data.
  */
  inline uint block () {
    return iterator_p[0].block();
  }

  /*!
    \brief Set a specific data block to read next
  */
  void setBlock (uint const &block);

  /*!
    \brief Get the stride between two subsequent blocks of data 

    \return stride -- Stride between two subsequent blocks of data 
  */
  inline uint stride () {
    return iterator_p[0].stride();
  }

  /*!
    \brief Set the stride between two subsequent blocks of data 

    \param stride -- Stride between two subsequent blocks of data 
  */
  void setStride (uint const & stride);

  /*!
    \brief Get the shift between the start of the data segment and the first block.

    \return shift -- \f$ N_{\rm Shift} \f$, a shift w.r.t. \f$ N_{\rm Start} \f$,
            for all the streams managed by the DataReader
  */
  std::vector<int> shift ();

  /*!
    \brief Get the shift between the start of the data segment and the first block.

    \return shift -- \f$ N_{\rm Shift} \f$, a shift w.r.t. \f$ N_{\rm Start} \f$,
            for a single specified data stream
  */
  inline int shift (unsigned int const &index) {
    return iterator_p[index].shift();
  }

  /*!
    \brief Set the shift between the start of the data segment and the first block.

    \param shift -- \f$ N_{\rm Shift} \f$, a shift w.r.t. \f$ N_{\rm Start} \f$;
           this shift will be applied to all the streams managed by the DataReader.
  */
  void setShift (int const &shift);

  /*!
    \brief Set the shift between the start of the data segment and the first block.

    \param shift -- \f$ N_{\rm Shift} \f$, a shift w.r.t. \f$ N_{\rm Start} \f$
  */
  void setShift (int const &shift,
		 unsigned int const &index);

  /*!
    \brief Set the shift between the start of the data segment and the first block.

    \param shift -- \f$ N_{\rm Shift} \f$, a shift w.r.t. \f$ N_{\rm Start} \f$
  */
  void setShift (std::vector<int> const &shift);

  /*!
   \brief Forward position pointer to the next block of data
   */
  void nextBlock ();

  /*!
    \brief Go back to the block at which we start reading the data
  */
  inline void toStartBlock () {
    setBlock (startBlock_p);
  }
  
  /*!
    \brief Get the current positions of the attached data streams
    
    \return positions -- [nofStreams] Vector with the current positions of the
                         attached data streams.
  */
  Vector<unsigned int> positions ();

  // -------------------------------------------------------------------- Methods

  /*!
    \brief Get the raw time series after ADC conversion
    
    \return fx -- [sample,antenna] Raw ADC time series, [Counts]
  */
  virtual Matrix<Double> fx ();

  /*!
    \brief Get the voltage time series
    
    \return voltage -- [sample,antenna] Voltage time series, [Volt]
  */
  virtual Matrix<Double> voltage ();

  /*!
    \brief Raw FFT of the voltage time series
    
    \return fft -- [channel,antenna] Raw FFT of the voltage time series

    <b>Note:</b> Depending on the Nyquist-zone the FFTed data may be flipped! 
    Use the <tt>invfft()</tt> function for the inverse FFT.
  */
  virtual Matrix<DComplex> fft ();

  /*!
    \brief Get the calibrated FFT
    
    \return calfft -- [channel,antenna] Calibrated FFT, i.e. spectra after correction for the antenna
                      gain-curves.
  */
  virtual Matrix<DComplex> calfft ();

  /*!
    \brief Do the inverse Fourier transform. (On the calfft data.)

    \return tsdata -- [sample,antenna] Reconstructed time series data, ["calibrated" Volts]
  */
    virtual inline Matrix<Double> invfft() {
      return invfft(calfft());
    };

  /*!
    \brief Do the inverse Fourier transform.

    \param fftdata -- Matrix with the fft-data as output from <tt>fft()</tt> or <tt>calfft()</tt>.

    \return tsdata -- [sample,antenna] Reconstructed time series data, [depends on input]
  */
    virtual Matrix<Double> invfft (Matrix<DComplex> fftdata);

  /*!
    \brief Do the inverse Fourier transform. on a single antenna trace

    \param fftdata -- Vector with the fft-data as output from <tt>fft()</tt> or <tt>calfft()</tt>.

    \return tsdata -- [sample] Reconstructed time series data, [depends on input]
  */
    virtual Vector<Double> invfft (Vector<DComplex> fftdata);


  /*!
    \brief Get the cross-correlation spectra

    \param fromCalFFT -- Cross-correlation spectra from the calibrated FFT data? If
                         set to <i>False</i>, the cross-correlation is carried out on
			 the raw FFT.

    \return ccSpectra -- Data cube with the cross-correlation spectra,
                         [nfreq,nant,nant]
  */
  virtual Cube<DComplex> ccSpectra (Bool const &fromCalFFT=True);

  /*!
    \brief Get the visibilities

    \param fromCalFFT -- Cross-correlation spectra from the calibrated FFT data? If
                         set to <i>False</i>, the cross-correlation is carried out
			 on the raw FFT.

    \return vis -- The visibilities

    \todo The antenna combinations for the baselines must be provided as well;
    either this information is stored internally, when computing the visibilities
    or is computed on the fly when requested.
    \todo We need an additional switch, to include/exclude the auto-correlation
    products (this feature is required when passing data to the MSSimulator).
  */
  virtual Matrix<DComplex> visibilities (Bool const &fromCalFFT=True);

  // ------------------------------------------------------ Selection of antennas

  /*!
    \brief Number of antennas available in the data set.

    \return nofAntennas -- Number of antennas available in the data set.
  */
  inline uint nofAntennas () const {
    return antennas_p.nelements();
  }

  /*!
    \brief Get the IDs of the available antennas

    \return antennas -- IDs of the antennas available in the data set.
  */
  inline Vector<uint> antennas () const {
    return antennas_p;
  }

  /*!
    \brief Define the set of available antennas in the data set.

    \param antennas -- Vector with the numbers of the available antennas

    \return status -- Status of the operation; returns <i>true</i> if everything
                      went fine.
  */
  bool setAntennas (Vector<uint> const &antennas);

  /*!
    \brief Define the set of available antennas in the data set.

    \param antennas         -- 
    \param selectedAntennas -- 

    \return status -- Status of the operation; returns <i>true</i> if everything
                      went fine.
  */
  bool setAntennas (Vector<uint> const &antennas,
		    Vector<uint> const &selectedAntennas);

  /*!
    \brief Define the set of available antennas in the data set.

    \param antennas         -- Vector with the numbers of the available antennas
    \param antennaSelection -- 

    \return status -- Status of the operation; returns <i>true</i> if everything
                      went fine.
  */
  bool setAntennas (Vector<uint> const &antennas,
		    Vector<bool> const &antennaSelection);

  /*!
    \brief Return array with the antenna selection flags

    \return antennaSelection -- Antenna selection flags
  */
  Vector<bool> antennaSelection () const;

  /*!
    \brief Get the number of selected antennas

    \return nofSelectedAntennas -- The number of antennas in the data set, which
                                   have been selected for processing
  */
  uint nofSelectedAntennas () const {
    return selectedAntennas_p.nelements();
  }

  /*!
    \brief Get the number of baselines

    \param allAntennas 

    \return nofBaselines
  */
  uint nofBaselines (bool const &allAntennas=false) const;

  /*!
   \brief Selection of the antennas in the dataset
   
   \return antennaSelection -- Selection of the antennas in the dataset
  */
  Vector<uint> selectedAntennas () const {
    return selectedAntennas_p;
  }

  /*!
   \brief Selection of the antennas in the dataset
   
   \param antennaSelection -- Selection of the antennas in the dataset
  */
  Bool setSelectedAntennas (Vector<uint> const &antennaSelection,
			    bool const &absolute=true);

  /*!
   \brief Selection of the antennas in the dataset
   
   \param antennaSelection -- Selection of the antennas in the dataset
  */
  Bool setSelectedAntennas (Vector<Bool> const &antennaSelection);

  // -------------------------------------------- Selection of frequency channels

  Vector<uint> selectedChannels () const {
    return selectedChannels_p;
  }

  /*!
    \brief Set the numbers of the frequency channels selected for processing

    \param channelSelection -- 

    \return status -- 
  */
  Bool setSelectedChannels (Vector<uint> const &channelSelection);

  /*!
    \brief Set the numbers of the frequency channels selected for processing

    \param channelSelection -- 

    \return status -- 
  */
  Bool setSelectedChannels (Vector<Bool> const &channelSelection);

  /*!
    \brief Get the number of selected frequency channels

    \return nofSelectedChannels -- The number of selected frequency channels
  */
  inline uint nofSelectedChannels () const {
    return selectedChannels_p.nelements();
  };

  /*!
    \brief Get the frequency values corresponding to the channels in the FFT

    \param onlySelected -- Return the frequency values only for the selected 
                           frequency channels; otherwise the complete frequency
			   band in the FFT is covered

    \return frequency -- Frequency values corresponding to the channels in the
                         FFT, [Hz]
   */
  Vector<Double> frequencyValues (Bool const &onlySelected=True);
  
  // ------------------------------- Overloading of functions from the base class
  
  /*!
    \brief Get the values along the time axis
    
    \return timeValues -- Time values \f$ \{ \nu \} \f$ for the samples within
            a data block of length \f$ N_{\rm Blocksize} \f$ with zero offset;
	    i.e. this function returns the first \f$ N_{\rm Blocksize} \f$ time
	    values (as we know nothing here about blocks etc.).
  */
#ifdef HAVE_CASA
  virtual inline casa::Vector<double> timeValues () {
    return TimeFreq::timeValues (block(),true);
  }
#else
  virtual inline vector<double> timeValues () {
    return TimeFreq::timeValues (block(),true);
  }
#endif
  
  // ------------------------------------------------------------- Hanning filter
  
  /*!
    \brief Enable/Disable the Hanning filter

    \param alpha -- Slope parameter of the HanningFilter.

    Insert a Hanning filter before the the Fourier transform; the slope
    parameter \f$ \alpha \f$ can be used to obtain a Hanning- or a
    Hammingfilter. To disable the HanningFilter call this function with
    \f$ \alpha = 0 \f$.
   */
  void setHanningFilter (double const &alpha);
  /*!
    \brief Enable/Disable the Hanning filter

    \param alpha -- Slope parameter of the HanningFilter.
    \param beta  -- Width of the plateau with w[n]=1.
  */
  void setHanningFilter (double const &alpha,
			 uint const &beta);
  
  /*!
    \brief Enable/Disable the Hanning filter

    \param alpha     -- Slope parameter of the HanningFilter.
    \param beta      -- Width of the plateau with w[n]=1.
    \param betaRise  -- Width before beginning of the plateau
    \param betaFall  -- Width after end of the plateau. beta + betaRise + betaFall must equal 1.
  */
 
  void setHanningFilter (double const &alpha,
			 uint const &beta,
                         uint const &betaRise, 
			 uint const &betaFall);


  /*!
    \brief Get the name of the class
    
    \return className -- The name of the class, DataReader.
  */
  virtual std::string className () const {
    return "DataReader";
  }
  
  private:
  
  /*!
    \brief Unconditional copying
  */
  void copy (DataReader const &other);
  
  /*!
    \brief Unconditional deletion 
  */
  void destroy(void);

  /*!
    \brief Initialization to default parameters
    
    \param blocksize   -- Size of a block of data, [samples]
  */
  void init (uint const &blocksize);

  /*!
    \brief Initialization
    
    \param blocksize   -- Size of a block of data, [samples]
  */
  void init (uint const &blocksize,
	     uint const &nyquistZone,
	     Double const &samplerate);

  /*!
    \brief Select the frequency channels
  */
  Matrix<DComplex> selectChannels (Matrix<DComplex> const &data);
};

}  //  Namespace CR -- end

#endif /* DATAREADER_H */
