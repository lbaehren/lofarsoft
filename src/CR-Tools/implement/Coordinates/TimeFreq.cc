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

#include <Coordinates/TimeFreq.h>

namespace CR { // Namespace CR -- begin
  
  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================
  
  //_____________________________________________________________________________
  //                                                                     TimeFreq
  
  TimeFreq::TimeFreq ()
  {
    setBlocksize (1);
    sampleFrequency_p = 80e06;
    nyquistZone_p     = 1;
    referenceTime_p   = 0;
  }
  
  //_____________________________________________________________________________
  //                                                                     TimeFreq
  
  TimeFreq::TimeFreq (uint const &blocksize)
  {
    setBlocksize (blocksize);
    sampleFrequency_p = 80e06;
    nyquistZone_p     = 1;
    referenceTime_p   = 0;
  }

  //_____________________________________________________________________________
  //                                                                     TimeFreq
  
  TimeFreq::TimeFreq (uint const &blocksize,
		      double const &sampleFrequency,
		      uint const &nyquistZone)
  {
    setBlocksize (blocksize);
    sampleFrequency_p = sampleFrequency;
    nyquistZone_p     = nyquistZone;
    referenceTime_p   = 0;
  }
  
  //_____________________________________________________________________________
  //                                                                     TimeFreq
  
  TimeFreq::TimeFreq (uint const &blocksize,
		      casa::Quantity const &sampleFrequency,
		      uint const &nyquistZone)
  {
    setBlocksize (blocksize);
    setSampleFrequency (sampleFrequency);
    nyquistZone_p     = nyquistZone;
    referenceTime_p   = 0;
  }
  
  //_____________________________________________________________________________
  //                                                                     TimeFreq
  
  TimeFreq::TimeFreq (uint const &blocksize,
		      double const &sampleFrequency,
		      uint const &nyquistZone,
		      double const &referenceTime)
  {
    setBlocksize (blocksize);
    sampleFrequency_p = sampleFrequency;
    nyquistZone_p     = nyquistZone;
    referenceTime_p   = referenceTime;
  }
  
  //_____________________________________________________________________________
  //                                                                     TimeFreq
  
  TimeFreq::TimeFreq (uint const &blocksize,
		      casa::Quantity const &sampleFrequency,
		      uint const &nyquistZone,
		      casa::Quantity const &referenceTime)
  {
    setBlocksize (blocksize);
    setSampleFrequency (sampleFrequency);
    nyquistZone_p     = nyquistZone;
    setReferenceTime (referenceTime);
  }
  
  //_____________________________________________________________________________
  //                                                                     TimeFreq
  
  TimeFreq::TimeFreq (TimeFreq const &other)
  {
    copy (other);
  }
  
  // ============================================================================
  //
  //  Destruction
  //
  // ============================================================================
  
  //_____________________________________________________________________________
  //                                                                    ~TimeFreq
  
  TimeFreq::~TimeFreq ()
  {
    destroy();
  }
  
  void TimeFreq::destroy ()
  {;}
  
  // ============================================================================
  //
  //  Operators
  //
  // ============================================================================
  
  TimeFreq& TimeFreq::operator= (TimeFreq const &other)
  {
    if (this != &other) {
      destroy ();
      copy (other);
    }
    return *this;
  }
  
  void TimeFreq::copy (TimeFreq const &other)
  {
    blocksize_p       = other.blocksize_p;
    fftLength_p       = other.fftLength_p;
    sampleFrequency_p = other.sampleFrequency_p;
    nyquistZone_p     = other.nyquistZone_p;
    referenceTime_p   = other.referenceTime_p;
  }

  // ============================================================================
  //
  //  Parameters
  //
  // ============================================================================

  //________________________________________________________________ setBlocksize

  void TimeFreq::setBlocksize (uint const &blocksize) {
    blocksize_p = blocksize;
    fftLength_p = blocksize_p/2+1;
  }

  //__________________________________________________________ setSampleFrequency

  void TimeFreq::setSampleFrequency (double const &sampleFrequency) {
    sampleFrequency_p = sampleFrequency;
  }

  //______________________________________________________________ setNyquistZone

  void TimeFreq::setNyquistZone (uint const &nyquistZone) {
    nyquistZone_p = nyquistZone;
  }
  
  // -------------------------------------------------------------------- summary
  
  /*!
    \param os -- Output stream to which the summary is written.
    
    For an object created via the default constructor, the output should be
    \verbatim
    -- Fundamental parameters:
    Blocksize      [samples] = 1
    Sample frequency    [Hz] = 8e+07
    Nyquist zone             = 1
    -- Derived quantities:
    Sample interval      [s] = 1.25e-08
    Frequency increment [Hz] = 8e+07
    \endverbatim
  */
  void TimeFreq::summary (std::ostream &os)
  {
    os << "[TimeFreqSkymap] Summary of internal parameters." << std::endl;
    os << "-- Blocksize      [samples] = " << blocksize_p          << std::endl;
    os << "-- Sample frequency    [Hz] = " << sampleFrequency_p    << std::endl;
    os << "-- Nyquist zone             = " << nyquistZone_p        << std::endl;
    os << "-- Reference time     [sec] = " << referenceTime_p      << std::endl;
    os << "-- FFT length    [channels] = " << fftLength_p          << std::endl;
    os << "-- Sample interval      [s] = " << sampleInterval()     << std::endl;
#ifdef HAVE_CASA
    os << "-- Frequency band      [Hz] = " << frequencyRange()      << std::endl;
    os << "-- Shape of the axes        = " << shape()              << std::endl;
    os << "-- Axis value increment     = " << increment()          << std::endl;
#endif
  }
  
  // ============================================================================
  //
  //  Methods
  //
  // ============================================================================

  // ---------------------------------------------------------------------- shape

#ifdef HAVE_CASA
  casa::IPosition TimeFreq::shape () {
    return casa::IPosition (2,blocksize(),fftLength());
  }
#else 
  std::vector<int> TimeFreq::shape () {
    std::vector<int> shape(2);
    shape[0] = blockize();
    shape[1] = fftLength();
    return shape;
  }
#endif

  // ------------------------------------------------------------------ increment

#ifdef HAVE_CASA
  casa::Vector<double> TimeFreq::increment ()
  {
    casa::Vector<double> vec (2);

    vec(0) = sampleInterval();
    vec(1) = sampleFrequency_p/blocksize_p;

    return vec;
  }
#else 
  std::vector<double> TimeFreq::increment ()
  {
    std::vector<double> vec (2);

    vec[0] = sampleInterval();
    vec[1] = sampleFrequency_p/blocksize_p;

    return vec;
}
#endif

  // ------------------------------------------------------------- frequencyRange

#ifdef HAVE_CASA
    casa::Vector<double> TimeFreq::frequencyRange ()
    {
      casa::Vector<double> range(2);
      
      range(0) = 0.5*(nyquistZone_p-1)*sampleFrequency_p;
      range(1) = 0.5*nyquistZone_p*sampleFrequency_p;
      
      return range;
    }
#else
    std::vector<double> TimeFreq::frequencyRange ()
    {
      std::vector<double> range(2);
      
      range[0] = 0.5*(nyquistZone_p-1)*sampleFrequency_p;
      range[1] = 0.5*nyquistZone_p*sampleFrequency_p;
      
      return range;
    }
#endif
  
  // ------------------------------------------------------------ frequencyValues

#ifdef HAVE_CASA
  casa::Vector<double> TimeFreq::frequencyValues ()
  {
    uint nofChannels (fftLength());
    double incr (TimeFreq::increment()(1));
    casa::Vector<double> band (frequencyRange());
    casa::Vector<double> frequencies (nofChannels);
    
    for (uint k(0); k<nofChannels; k++) {
      frequencies(k) = band(0) + k*incr;
    }
    
    return frequencies;
  }  
#else
  std::vector<double> TimeFreq::frequencyValues ()
  {
    uint nofChannels (fftLength());
    double incr (TimeFreq::increment()(1));
    std::vector<double> band (frequencyRange());
    std::vector<double> frequencies (nofChannels);
    
    for (uint k(0); k<nofChannels; k++) {
      frequencies[k] = band[0] + k*incr;
    }
    
    return frequencies;
  }  
#endif

  // ------------------------------------------------------------ frequencyValues
  
#ifdef HAVE_CASA
    Vector<double> TimeFreq::frequencyValues (Vector<bool> const &selection)
    {
      uint channel (0);
      uint nofChannels (fftLength());
      Vector<double> frequencies (frequencyValues());
      Vector<double> selectedFrequencies;
      
      /*
	Check if the selection vector has the same number of elements as the
	vector with the full number of frequency values
      */
      if (frequencies.nelements() == selection.nelements()) {
	uint nofSelectedChannels (0);
	// get the number of selected channels
	for (channel=0; channel<nofChannels; channel++) {
	  nofSelectedChannels += uint(selection(channel));
	}
	// resize the vector returning the results
	selectedFrequencies.resize(nofSelectedChannels);
	// fill in the values
	for (uint k(0), channel=0; channel<nofChannels; channel++) {
	  if (selection(channel)) {
	    selectedFrequencies(k) = frequencies(channel);
	    k++;
	  }
	}
      } else {
	std::cerr << "[TimeFreq::frequencyValues] Mismatching vector sizes!"
		  << std::endl;
	selectedFrequencies.resize (nofChannels);
	selectedFrequencies = frequencies;
      }
      
      return selectedFrequencies;
    }
#else
  std::vector<double> TimeFreq::frequencyValues (std::vector<bool> const &selection)
  {
    uint channel (0);
    uint nofChannels (fftLength());
    std::vector<double> frequencies (frequencyValues());
    std::vector<double> selectedFrequencies;
    
    /*
      Check if the selection vector has the same number of elements as the
      vector with the full number of frequency values
    */
    if (frequencies.size() == selection.size()) {
      uint nofSelectedChannels (0);
      // get the number of selected channels
      for (channel=0; channel<nofChannels; channel++) {
	nofSelectedChannels += uint(selection[channel]);
      }
      // resize the vector returning the results
      selectedFrequencies.resize(nofSelectedChannels);
      // fill in the values
      for (uint k(0), channel=0; channel<nofChannels; channel++) {
	if (selection[channel]) {
	  selectedFrequencies[k] = frequencies[channel];
	  k++;
	}
      }
    } else {
      std::cerr << "[TimeFreq::frequencyValues] Mismatching vector sizes!"
		<< std::endl;
      selectedFrequencies.resize (nofChannels);
      selectedFrequencies = frequencies;
    }

    return selectedFrequencies;
  }
#endif
  
  // ------------------------------------------------------------ frequencyValues

#ifdef HAVE_CASA
  casa::Vector<double> TimeFreq::frequencyValues (casa::Vector<double> const &range)
  {
    uint nofChannels (fftLength());
    uint nofSelectedChannels (0);
    Vector<bool> selection (nofChannels);
    Vector<double> frequencies (frequencyValues());

    for (uint channel (0); channel<nofChannels; channel++) {
      if (frequencies(channel)>range[0] && frequencies(channel)<range[1]) {
	selection(channel) = true;
	nofSelectedChannels++;
      } else {
	selection(channel) = false;
      }
    }

    // give some feedback in case no frequency channel was selected
    if (nofSelectedChannels == 0) {
      std::cerr << "[TimeFreq::frequencyValues] No frequency channels selected!"
		<< std::endl;
      std::cerr << "-- maximum available range  = [ " << frequencies[0] << " .. "
		<< frequencies[nofChannels-1] << " ]" << std::endl;
      std::cerr << "-- selected frquency range = [ " << range[0] << " .. "
		<< range[1] << " ]" << std::endl;
    }
    
    return frequencyValues (selection);
  }
#else
  std::vector<double> TimeFreq::frequencyValues (std::vector<double> const &range)
  {
    uint nofChannels (fftLength());
    uint nofSelectedChannels (0);
    std::vector<bool> selection (nofChannels);
    std::vector<double> frequencies (frequencyValues());

    for (uint channel (0); channel<nofChannels; channel++) {
      if (frequencies[channel]>range[0] && frequencies[channel]<range[1]) {
	selection[channel] = true;
	nofSelectedChannels++;
      } else {
	selection[channel] = false;
      }
    }

    // give some feedback in case no frequency channel was selected
    if (nofSelectedChannels == 0) {
      std::cerr << "[TimeFreq::frequencyValues] No frequency channels selected!"
		<< std::endl;
      std::cerr << "-- maximum available range  = [ " << frequencies[0] << " .. "
		<< frequencies[nofChannels-1] << " ]" << std::endl;
      std::cerr << "-- selected frquency range = [ " << range[0] << " .. "
		<< range[1] << " ]" << std::endl;
    }
    
    return frequencyValues (selection);
  }
#endif

  // ------------------------------------------------------------ frequencyValues

#ifdef HAVE_CASA
  casa::Vector<double> TimeFreq::frequencyValues (double const &min,
						  double const &max)
  {
    Vector<double> range (2);

    if (min <= max) {
      range(0) = min;
      range(1) = max;
    } else {
      range(0) = max;
      range(1) = min;
    }

    return frequencyValues(range);
  }
#else
  std::vector<double> TimeFreq::frequencyValues (double const &min,
						 double const &max)
  {
    std::vector<double> range (2);

    if (min <= max) {
      range[0] = min;
      range[1] = max;
    } else {
      range[0] = max;
      range[1] = min;
    }

    return frequencyValues(range);
  }
#endif

  //_____________________________________________________________________________
  //                                                                 sampleValues
  
#ifdef HAVE_CASA
  casa::Vector<uint> TimeFreq::sampleValues (uint const &sampleOffset,
					     bool const &offsetIsBlock)
  {
    casa::Vector<uint> values (blocksize_p);
    
    if (offsetIsBlock) {
      values = blocksize_p*sampleOffset;
    } else {
      values = sampleOffset;
    }
    
    for (uint n(0); n<blocksize_p; n++) {
      values(n) += n;
    }
    
    return values;
  }
#else 
  std::vector<uint> TimeFreq::sampleValues (uint const &sampleOffset,
					    bool const &offsetIsBlock)
  {
    std::vector<uint> values (blocksize_p);
    
    if (offsetIsBlock) {
      for (uint n(0); n<blocksize_p; n++) {
	values(n) = blocksize_p*sampleOffset+n;
      }
    } else {
      for (uint n(0); n<blocksize_p; n++) {
	values(n) = sampleOffset+n;
      }
    }
    
    return values;
  }
#endif
  
  //_____________________________________________________________________________
  //                                                                   timeValues
  
#ifdef HAVE_CASA
  casa::Vector<double> TimeFreq::timeValues (uint const &sampleOffset,
					     bool const &offsetIsBlock)
  {
    // Get the sample values for the current block ...
    casa::Vector<uint> samples = sampleValues(sampleOffset,
					      offsetIsBlock);
    // ... and convert these values to time
    return timeValues (samples);
  }
#else  
  std::vector<double> TimeFreq::timeValues (uint const &sampleOffset,
				       bool const &offsetIsBlock)
  {
    // Get the sample values for the current block ...
    std::vector<uint> samples = sampleValues(sampleOffset,
					     offsetIsBlock);
    // ... and convert these values to time
    return timeValues (samples);
  }
#endif
  
  //_____________________________________________________________________________
  //                                                                   timeValues
  
#ifdef HAVE_CASA
  casa::Vector<double> TimeFreq::timeValues (casa::Vector<uint> const &sampleValues)
  {
    uint nelem (sampleValues.size());
    casa::Vector<double> times (blocksize_p);

    /*
      Since the function accepts vector of sample values of arbitray shape,
      we at least give a warning in case the input vector does not match the
      internally stored parameter for the blocksize
    */
    if (nelem != blocksize_p) {
      std::cerr << "[TimeFreq::timeValues] Selection vector has wrong length!"
		<< std::endl;
    }
    
    for (uint n(0); n<nelem; n++) {
      times(n) = referenceTime_p + sampleValues(n)/sampleFrequency_p;
    }
    
    return times;
    
  }
#else  
  std::vector<double> TimeFreq::timeValues (std::vector<uint> const &sampleValues)
  {
    uint nelem (sampleValues.size());
    std::vector<double> times (blocksize_p);

    /*
      Since the function accepts vector of sample values of arbitray shape,
      we at least give a warning in case the input vector does not match the
      internally stored parameter for the blocksize
    */
    if (nelem != blocksize_p) {
      std::cerr << "[TimeFreq::timeValues] Selection vector has wrong length!"
		<< std::endl;
    }
    
    for (uint n(0); n<nelem; n++) {
      times[n] = referenceTime_p + sampleValues[n]/sampleFrequency_p;
    }
    
    return times;
    
  }
#endif

  // ============================================================================
  //
  //  Optional methods which require casacore (or CASA)
  //
  // ============================================================================

#ifdef HAVE_CASA
  
  //_____________________________________________________________________________
  //                                                              sampleFrequency

  double TimeFreq::sampleFrequency (casa::String const &unit)
  {
    casa::Quantity rate (sampleFrequency_p,"Hz");
    
    return rate.getValue(unit);
  }
  
  // --------------------------------------------------------- setSampleFrequency

  void TimeFreq::setSampleFrequency (double const &value,
				     std::string const &unit)
  {
    // Unit needs to be converted to casa::String beforehand
    casa::String sampleFrequencyUnit (unit);
    // Create Quantity ...
    casa::Quantity rate (value,sampleFrequencyUnit);
    // ... and pass it on
    setSampleFrequency (rate);
  }
  
  // ------------------------------------------------------------------- timeAxis
  
  casa::LinearCoordinate TimeFreq::timeAxis ()
  {    
    double refValue (0.0);
    double increment (sampleInterval());
    double refPixel (0.0);
    
    return timeAxis (refValue,
		     increment,
		     refPixel);
  }

  // ------------------------------------------------------------------- timeAxis
  
  casa::LinearCoordinate TimeFreq::timeAxis (double const &crval,
					     double const &cdelt,
					     double const &crpix)
  {
    uint nofAxes (1);

    casa::Vector<casa::String> names  (nofAxes,"Time");
    casa::Vector<casa::String> units  (nofAxes,"s");
    casa::Vector<casa::Double> refVal (nofAxes,crval);
    casa::Vector<casa::Double> inc    (nofAxes,cdelt);
    casa::Matrix<casa::Double> pc     (nofAxes,nofAxes);
    casa::Vector<casa::Double> refPix (nofAxes,crpix);
    
    pc            = 0.0;
    pc.diagonal() = 1.0;

    return casa::LinearCoordinate (names,
				   units,
				   refVal,
				   inc,
				   pc,
				   refPix);
  }
  
  // -------------------------------------------------------------- frequencyAxis

  casa::SpectralCoordinate TimeFreq::frequencyAxis ()
  {
    double crval    = frequencyRange()(0);
    double cdelt    = increment()(1);
    double crpix    = 0;
    
    return frequencyAxis (crval,
			  cdelt,
			  crpix);
  }
  
  // -------------------------------------------------------------- frequencyAxis
  
  SpectralCoordinate TimeFreq::frequencyAxis (double const &crval,
					      double const &cdelt,
					      double const &crpix)
  {
    double restfreq  = 1.420405752E9;
    double refValue  = crval;
    double increment = cdelt;
    double refPixel  = crpix;

    SpectralCoordinate axis (casa::MFrequency::TOPO,
			     refValue,
			     increment,
			     refPixel,
			     restfreq);

    // Add the world axis-name
    casa::Vector<casa::String> names  (1,"Frequency");
    axis.setWorldAxisNames(names);
    
    return axis;
  }

  #endif

} // Namespace CR -- end
