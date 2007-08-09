/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2007                                                    *
 *   Lars Baehren (bahren@astron.nl)                                       *
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

#include <iostream>
#include <string>
#include <vector>
#include <iomanip>

using std::vector;

namespace CR {

  // ============================================================================
  //
  //  class TimeFreq -- Definition
  //
  // ============================================================================
  
  /*!
    \class TimeFreq
    
    \ingroup CR-Pipeline

    \brief Container for the basic parameters in the time-frequency domain

    \author Lars B&auml;hren
    
    \date 2007/03/06

    Most of the quantities in the time-frequency domain can be derived from a
    small set of basic variables:
    <ul>
      <li><b>blocksize</b> -- The number of samples in a block of data, which is
          getting processed.
      <li><b>sampleFrequency</b> -- The frequency, [Hz], with which the data are
          sampled in the analog-to-digital conversion step
      <li><b>nyquistZone</b> -- The Nyquist zone. The sample rate divided by two
          (\f$ \nu_{\rm sample}/2 \f$) is known as the <i>Nyquist frequency</i>
	  and the frequency range from DC (or 0 Hz) to \f$ \nu_{\rm sample}/2 \f$
	  is called the first Nyquist zone.
    </ul>
  */
  class TimeFreq {
    
    //! Blocksize, [samples]
    uint blocksize_p;
    //! Sample frequency in the ADC, [Hz]
    double sampleFrequency_p;
    //! Nyquist zone,  [1]
    uint nyquistZone_p;
    
  public:
    
    /*!
      \brief Default constructor

      Initialized object with <tt>blocksize=1</tt>, <tt>sampleFrequency=80e06</tt>
      and <tt>nyquistZone=1</tt>.
    */
    TimeFreq ();
    
    /*!
      \brief Argumented constructor

      \param blocksize       -- Blocksize, [samples]
      \param sampleFrequency -- Sample frequency in the ADC, [Hz]
      \param nyquistZone     -- Nyquist zone,  [1]
    */
    TimeFreq (uint const &blocksize,
	      double const &sampleFrequency,
	      uint const &nyquistZone);

    // == Access to the internal parameters =====================================

    /*!
      \brief Get the blocksize, \f$ N_{\rm Blocksize} \f$

      \return blocksize -- Blocksize, [samples]
    */
    inline uint blocksize () {
      return blocksize_p;
    }

    /*!
      \brief Set the blocksize, \f$ N_{\rm Blocksize} \f$

      \param blocksize -- Blocksize, [samples]
    */
    inline void setBlocksize (uint const &blocksize) {
      blocksize_p = blocksize;
    }

    /*!
      \brief Get the sample frequency, \f$ \nu_{\rm Sample} \f$

      \return sampleFrequency -- Sample frequency in the ADC, [Hz]
    */
    inline double sampleFrequency () {
      return sampleFrequency_p;
    }

    /*!
      \brief Set the sample frequency, \f$ \nu_{\rm Sample} \f$

      \param sampleFrequency -- Sample frequency in the ADC, [Hz]
    */
    inline void setSampleFrequency (double const &sampleFrequency) {
      sampleFrequency_p = sampleFrequency;
    }

    /*!
      \brief Get the Nyquist zone, \f$ N_{\rm Nyquist} \f$

      \return nyquistZone -- Nyquist zone,  [1]
    */
    inline uint nyquistZone () {
      return nyquistZone_p;
    }

    /*!
      \brief Set the Nyquist zone, \f$ N_{\rm Nyquist} \f$

      \param nyquistZone -- Nyquist zone,  [1]
    */
    inline void setNyquistZone (uint const &nyquistZone) {
      nyquistZone_p = nyquistZone;
    }

    /*!
      \brief Provide a summary of the internal status

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
    void summary ();

    // == Quantities derived from the fundamental parameters ====================

    /*!
      \brief Get the output length of the FFT, \f$ N_{\rm FFT} \f$

      \return fftLength -- The output length of the FFT, [channels], i.e. the 
                           number of frequency channels resulting from the FFT
			   of a data block of \f$ N_{\rm Blocksize} \f$
			   samples.
    */
    inline uint fftLength () {
      return blocksize_p/2+1;
    }

    /*!
      \brief Get the sample interval, \f$ T_{\rm Sample} \f$

      \return sampleInterval -- The sample interval, [s], i.e. the inverse of the
                                sample frequency: \f$ T_{\rm Sample} =
				1/\nu_{\rm Sample}  \f$
    */
    inline double sampleInterval () {
      return 1/sampleFrequency_p;
    }

    /*!
      \brief Get the increment along the frequency axis, \f$ \delta \nu \f$

      \return frequencyIncrement -- The increment along the frequency axis, [Hz],
                                    \f[ \delta \nu =
				    \frac{\nu_{\rm Sample}}{N_{\rm Blocksize}} \f]
    */
    inline double frequencyIncrement () {
      return sampleFrequency_p/blocksize_p;
    }

    /*!
      \brief Get the limits of the frequency band, \f$ [ \nu_{\rm min},\nu_{\rm max} ] \f$

      \return frequencyBand -- The upper and lower limit covered by the
                               combination of sample frequency and Nyquist zone
    */
    vector<double> frequencyBand ();
    
    /*!
      \brief Get the limits of the frequency band
      
      \retval minFrequency -- The lower limit of the frequency band, [Hz]
      \retval maxFrequency -- The upper limit of the frequency band, [Hz]
    */
    void frequencyBand (double &minFrequency,
			double &maxFrequency);
    
    /*!
      \brief Get the values along the frequency axis, \f$ \{ \nu \} \f$

      \return frequencyValues -- The frequency values associated with the FFT
                                 channels, [Hz]. Given the sample frequency
				 \f$ \nu_{\rm Sample} \f$ and the Nyquist zone
				 \f$ N_{\rm Nyquist} \f$, the value of the
				 \f$k\f$-th channel is given by
				 \f[ \nu [k] = \nu_{\rm min} + k \cdot \delta \nu \f]
    */
    vector<double> frequencyValues ();

  };
  
  // ============================================================================
  //
  //  class TimeFreq -- Implementation
  //
  // ============================================================================
  
  TimeFreq::TimeFreq ()
  {
    blocksize_p       = 1;
    sampleFrequency_p = 80e06;
    nyquistZone_p     = 1;
  }
  
  TimeFreq::TimeFreq (uint const &blocksize,
		      double const &sampleFrequency,
		      uint const &nyquistZone)
  {
    blocksize_p       = blocksize;
    sampleFrequency_p = sampleFrequency;
    nyquistZone_p     = nyquistZone;
  }

  void TimeFreq::summary ()
  {
    std::cout << "-- Fundamental parameters:"                           << std::endl;
    std::cout << " Blocksize      [samples] = " << blocksize_p          << std::endl;
    std::cout << " Sample frequency    [Hz] = " << sampleFrequency_p    << std::endl;
    std::cout << " Nyquist zone             = " << nyquistZone_p        << std::endl;
    std::cout << "-- Derived quantities:"                               << std::endl;
    std::cout << " FFT length    [channels] = " << fftLength()          << std::endl;
    std::cout << " Sample interval      [s] = " << sampleInterval()     << std::endl;
    std::cout << " Frequency increment [Hz] = " << frequencyIncrement() << std::endl;
    std::cout << " Frequency band      [Hz] = " << frequencyBand()[0]
	      << " .. "
	      << frequencyBand()[1] << std::endl;
  }

  vector<double> TimeFreq::frequencyBand ()
  {
    vector<double> band(2);

    frequencyBand (band[0],band[1]);

    return band;
  }

  void TimeFreq::frequencyBand (double &minFrequency,
				double &maxFrequency)
  {
    minFrequency = (nyquistZone_p-1)*0.5*sampleFrequency_p;
    maxFrequency = minFrequency + 0.5*sampleFrequency_p;
  }
  
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
  
}  // namespace CR -- end


int main ()
{
  uint blocksize (2048);
  double sampleFrequency (80e06);
  uint nyquistZone (1);

  /*
    [1] Create TimeFreq object and check its internal status
  */
  {
    CR::TimeFreq data1;
    CR::TimeFreq data2 (blocksize,sampleFrequency,nyquistZone);

    std::cout << "\n[1] Object created via default constructor" << std::endl;
    data1.summary();

    std::cout << "\n[2] Object created via argumented constructor" << std::endl;
    data2.summary();
  }

  /*
    [2] Test influence of changing Nyquist zone.
  */
  {
    CR::TimeFreq data (blocksize,sampleFrequency,nyquistZone);

    std::cout << "\n[1] Object created via default constructor" << std::endl;
    data.summary();

    std::cout << "\n[2] Update of Nyquist zone" << std::endl;
    data.setNyquistZone (2);
    data.summary();
    
    std::cout << "\n[3] Update of Nyquist zone" << std::endl;
    data.setNyquistZone (3);
    data.summary();

    // get the frequency values
    uint nofChannels (data.fftLength());
    vector<double> freq (data.frequencyValues());

    std::cout << " freq[0]   = " << freq[0] << std::endl;
    std::cout << " freq[1]   = " << freq[1] << std::endl;
    std::cout << " freq[2]   = " << freq[2] << std::endl;
    std::cout << " freq[N-2] = " << freq[nofChannels-2] << std::endl;
    std::cout << " freq[N-1] = " << freq[nofChannels-1] << std::endl;
    
  }
  
  return 0;
}
