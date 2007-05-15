/***************************************************************************
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

/* $Id: TimeFreq.cc,v 1.9 2007/05/03 14:54:58 bahren Exp $*/

#include <Coordinates/TimeFreq.h>

namespace CR { // Namespace CR -- begin
  
  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================
  
  TimeFreq::TimeFreq ()
  {
    blocksize_p       = 1;
    sampleFrequency_p = 80e06;
    nyquistZone_p     = 1;
    referenceTime_p   = 0;
  }
  
  TimeFreq::TimeFreq (uint const &blocksize)
  {
    blocksize_p       = blocksize;
    sampleFrequency_p = 80e06;
    nyquistZone_p     = 1;
    referenceTime_p   = 0;
  }

  TimeFreq::TimeFreq (uint const &blocksize,
		      double const &sampleFrequency,
		      uint const &nyquistZone)
  {
    blocksize_p       = blocksize;
    sampleFrequency_p = sampleFrequency;
    nyquistZone_p     = nyquistZone;
    referenceTime_p   = 0;
  }
  
  TimeFreq::TimeFreq (uint const &blocksize,
	    double const &sampleFrequency,
	    uint const &nyquistZone,
	    double const &referenceTime)
  {
    blocksize_p       = blocksize;
    sampleFrequency_p = sampleFrequency;
    nyquistZone_p     = nyquistZone;
    referenceTime_p   = referenceTime;
  }
  
  TimeFreq::TimeFreq (TimeFreq const &other)
  {
    copy (other);
  }
  
  // ============================================================================
  //
  //  Destruction
  //
  // ============================================================================
  
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
    sampleFrequency_p = other.sampleFrequency_p;
    nyquistZone_p     = other.nyquistZone_p;
    referenceTime_p   = other.referenceTime_p;
  }

  // ============================================================================
  //
  //  Parameters
  //
  // ============================================================================
  
  void TimeFreq::summary ()
  {
    std::cout << "-- Fundamental parameters:"                           << "\n";
    std::cout << " Blocksize      [samples] = " << blocksize_p          << "\n";
    std::cout << " Sample frequency    [Hz] = " << sampleFrequency_p    << "\n";
    std::cout << " Nyquist zone             = " << nyquistZone_p        << "\n";
    std::cout << " Reference time     [sec] = " << referenceTime_p      << "\n";
    std::cout << "-- Derived quantities:"                               << "\n";
    std::cout << " FFT length    [channels] = " << fftLength()          << "\n";
    std::cout << " Sample interval      [s] = " << sampleInterval()     << "\n";
    std::cout << " Frequency increment [Hz] = " << frequencyIncrement() << "\n";
    std::cout << " Frequency band      [Hz] = " << frequencyBand()[0]
	      << " .. "
	      << frequencyBand()[1]
	      << std::endl;
  }
  
  // ============================================================================
  //
  //  Methods
  //
  // ============================================================================

  // -------------------------------------------------------------- frequencyBand
  
  vector<double> TimeFreq::frequencyBand ()
  {
    vector<double> band(2);
    
    frequencyBand (band[0],band[1]);
    
    return band;
  }
  
  // -------------------------------------------------------------- frequencyBand

  void TimeFreq::frequencyBand (double &minFrequency,
				double &maxFrequency)
  {
    minFrequency = (nyquistZone_p-1)*0.5*sampleFrequency_p;
    maxFrequency = minFrequency + 0.5*sampleFrequency_p;
  }
  
  // ------------------------------------------------------------ frequencyValues

  vector<double> TimeFreq::frequencyValues ()
  {
    uint nofChannels (fftLength());
    double increment (frequencyIncrement());
    vector<double> band (frequencyBand());
    vector<double> frequencies (nofChannels);
    
    for (uint k(0); k<nofChannels; k++) {
      frequencies[k] = band[0] + k*increment;
    }
    
    return frequencies;
  }  

  // ------------------------------------------------------------ frequencyValues
  
  vector<double> TimeFreq::frequencyValues (vector<bool> const &selection)
  {
    uint channel (0);
    uint nofChannels (fftLength());
    vector<double> frequencies (frequencyValues());
    vector<double> selectedFrequencies;
    
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
  
  // ------------------------------------------------------------ frequencyValues

  vector<double> TimeFreq::frequencyValues (vector<double> const &range)
  {
    uint nofChannels (fftLength());
    uint nofSelectedChannels (0);
    vector<bool> selection (nofChannels);
    vector<double> frequencies (frequencyValues());

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

  // ------------------------------------------------------------ frequencyValues

  vector<double> TimeFreq::frequencyValues (double const &min,
					    double const &max)
  {
    vector<double> range (2);

    if (min <= max) {
      range[0] = min;
      range[1] = max;
    } else {
      range[0] = max;
      range[1] = min;
    }

    return frequencyValues(range);
  }
  
  // ----------------------------------------------------------------- timeValues

  vector<double> TimeFreq::timeValues (uint const &sampleOffset) {
    vector<uint> sampleValues (blocksize_p,sampleOffset);
    return timeValues (sampleValues);
  }
  
  // ----------------------------------------------------------------- timeValues
  
  vector<double> TimeFreq::timeValues (vector<uint> const &sampleValues)
  {
    uint nelem (sampleValues.size());

    /*
      check if the provided selection vector has the correct shape; if this is
      not the case, we fall back onto the default method.
    */
    if (nelem == blocksize_p) {
      vector<double> times (blocksize_p);

      for (uint n(0); n<nelem; n++) {
	times[n] = referenceTime_p + sampleValues[n]/sampleFrequency_p;
      }

      return times;
    } else {
      // provide warning message ...
      std::cerr << "[TimeFreq::timeValues] Selection vector has wrong length!"
		<< std::endl;
      // ... and call default method
      return timeValues(0);
    }
    
  }

  // ============================================================================
  //
  //  Optional methods
  //
  // ============================================================================

#ifdef AIPS_STDLIB

  // ------------------------------------------------------------ sampleFrequency

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
  
  // --------------------------------------------------------- setSampleFrequency

  void TimeFreq::setSampleFrequency (casa::Quantity const &sampleFrequency)
  {
    setSampleFrequency (sampleFrequency.getValue("Hz"));
  }

  // ------------------------------------------------------------------- timeAxis
  
  LinearCoordinate TimeFreq::timeAxis ()
  {    
    double refValue (0.0);
    double increment (sampleInterval());
    double refPixel (0.0);
    
    return timeAxis (refValue,
		     increment,
		     refPixel);
  }

  // ------------------------------------------------------------------- timeAxis
  
  LinearCoordinate TimeFreq::timeAxis (double const &crval,
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

    return LinearCoordinate (names,
			     units,
			     refVal,
			     inc,
			     pc,
			     refPix);
  }
  
  // -------------------------------------------------------------- frequencyAxis

  SpectralCoordinate TimeFreq::frequencyAxis ()
  {
    vector<double> band (frequencyBand());
    
    double crval    = band[0];
    double cdelt    = frequencyIncrement();
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

} // Namespace CR -- end

#endif
  
