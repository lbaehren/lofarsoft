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

#ifndef TIMEFREQ_H
#define TIMEFREQ_H

// Header files from standard library
#include <iostream>
#include <string>
#include <vector>

using std::vector;

#ifdef HAVE_CASA
#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/Vector.h>
#include <casa/BasicSL/String.h>
#include <casa/Quanta.h>
#include <coordinates/Coordinates/LinearCoordinate.h>
#include <coordinates/Coordinates/SpectralCoordinate.h>
#include <measures/Measures/MFrequency.h>
using casa::LinearCoordinate;
using casa::SpectralCoordinate;
#endif 

namespace CR { // Namespace CR -- begin
  
  /*!
    \class TimeFreq
    
    \ingroup Coordinates
    
    \brief Container for the basic parameters in the time-frequency domain

    \author Lars B&auml;hren

    \date 2007/03/06

    \test tTimeFreq.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
      <li>[start filling in your text here]
    </ul>
    
    <h3>Synopsis</h3>
    
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

    <h3>Example(s)</h3>

    <ul>
      <li>Construction of a new object, providing all basic parameters:
      \code
      uint blocksize (2048);
      double sampleFrequency (80e06);
      uint nyquistZone (2);
      
      CR::TimeFreq data (blocksize,sampleFrequency,nyquistZone);
      \endcode
      <li>Assigning a new value to the sample frequency can be done in a number
      of ways (all of will will have the same result):
      \code
      // sample frequency in its natural units, [Hz]
      data.setSampleFrequency (80e06);

      // use combination of numerical value and unit
      data.setSampleFrequency (80,"MHz");

      // physical quantity encapsulating numerical value and unit
      data.setSampleFrequency (casa::Quantity(80,"MHz"));
      \endcode
    </ul>
    
  */  
  class TimeFreq {

  protected:
    
    //! Blocksize, \f$ N_{\rm Blocksize} \f$ , [samples]
    uint blocksize_p;
    //! Output length of the FFT, [samples]
    uint fftLength_p;
    //! Sample frequency in the ADC, \f$ \nu_{\rm Sample} \f$, [Hz]
    double sampleFrequency_p;
    //! Nyquist zone, \f$ N_{\rm Nyquist} \f$,  [1]
    uint nyquistZone_p;
    //! Reference time, \f$ t_0 \f$
    double referenceTime_p;

  public:
    
    // ------------------------------------------------------------- Construction
    
    /*!
      \brief Default constructor
      
      Initialized object with <tt>blocksize=1</tt>, <tt>sampleFrequency=80e06</tt>
      and <tt>nyquistZone=1</tt>.
    */
    TimeFreq ();
    
    /*!
      \brief Argumented constructor
      
      Initialized object with <tt>sampleFrequency=80e06</tt> and
      <tt>nyquistZone=1</tt>.
      
      \param blocksize       -- Blocksize, [samples]
    */
    TimeFreq (uint const &blocksize);
    
    /*!
      \brief Argumented constructor
      
      \param blocksize       -- Blocksize, [samples]
      \param sampleFrequency -- Sample frequency in the ADC, [Hz]
      \param nyquistZone     -- Nyquist zone,  [1]
    */
    TimeFreq (uint const &blocksize,
	      double const &sampleFrequency,
	      uint const &nyquistZone);
    
    /*!
      \brief Argumented constructor
      
      \param blocksize       -- Blocksize, [samples]
      \param sampleFrequency -- Sample frequency in the ADC, [Hz]
      \param nyquistZone     -- Nyquist zone,  [1]
      \param referenceTime   -- Reference time, \f$ t_0 \f$
    */
    TimeFreq (uint const &blocksize,
	      double const &sampleFrequency,
	      uint const &nyquistZone,
	      double const &referenceTime);
    
    /*!
      \brief Copy constructor
      
      \param other -- Another TimeFreq object from which to create this new
      one.
    */
    TimeFreq (TimeFreq const &other);
    
    // -------------------------------------------------------------- Destruction

    /*!
      \brief Destructor
    */
    virtual ~TimeFreq ();
    
    // ---------------------------------------------------------------- Operators
    
    /*!
      \brief Overloading of the copy operator
      
      \param other -- Another TimeFreq object from which to make a copy.
    */
    TimeFreq& operator= (TimeFreq const &other); 
    
    // --------------------------------------------------------------- Parameters
    
    /*!
      \brief Get the name of the class
      
      \return className -- The name of the class, TimeFreq.
    */
    std::string className () const {
      return "TimeFreq";
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
    
    // ------------------------------------------------------ Internal parameters

    // --- blocksize ---------------------------------------
    
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
    virtual void setBlocksize (uint const &blocksize);
    
    // --- sampleFrequency ---------------------------------

    /*!
      \brief Get the sample frequency, \f$ \nu_{\rm Sample} \f$

      \return sampleFrequency -- Sample frequency in the ADC, [Hz]
    */
    inline double sampleFrequency () {
      return sampleFrequency_p;
    }

    /*!
      \brief Get the sample frequency, \f$ \nu_{\rm Sample} \f$

      \param unit -- The unit of the returned frequency, e.g. MHz

      \return sampleFrequency -- Sample frequency in the ADC
    */
    double sampleFrequency (casa::String const &unit);

    /*!
      \brief Set the sample frequency, \f$ \nu_{\rm Sample} \f$

      \param sampleFrequency -- Sample frequency in the ADC, [Hz]
    */
    inline void setSampleFrequency (double const &sampleFrequency) {
      sampleFrequency_p = sampleFrequency;
    }

    /*!
      \brief Set the sample frequency, \f$ \nu_{\rm Sample} \f$

      \param value -- Sample frequency in the ADC
      \param unit  -- Unit in which the value is given, e.g. MHz
    */
    void setSampleFrequency (double const &value,
			     std::string const &unit);
    
    /*!
      \brief Set the sample frequency, \f$ \nu_{\rm Sample} \f$, from a Quantity

      \param sampleFrequency -- Sample frequency in the ADC, provided as a
                                casa::Quantity (i.e. a value with a unit).
     */
    void setSampleFrequency (casa::Quantity const &sampleFrequency);

    // --- nyquistZone -------------------------------------

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

    // --- referenceTime -----------------------------------

    /*!
      \brief Get the reference time, i.e. the start of the time axis

      \return referenceTime -- The reference time, \f$ t_0 \f$, marking the
              start of the time axis
    */
    inline double referenceTime () {
      return referenceTime_p;
    }

    /*!
      \brief Set the reference time, i.e. the start of the time axis

      \param referenceTime -- The reference time, \f$ t_0 \f$, marking the
             start of the time axis
    */
    inline void setReferenceTime (double const &referenceTime) {
      referenceTime_p = referenceTime;
    }

    // ------------------------------------------------------------------ Methods

    /*!
      \brief Get the output length of the FFT, \f$ N_{\rm FFT} \f$

      \return fftLength -- The output length of the FFT, [channels], i.e. the 
                           number of frequency channels resulting from the FFT
			   of a data block of \f$ N_{\rm Blocksize} \f$
			   samples.
    */
    inline uint fftLength () {
      return fftLength_p;
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

    /*!
      \brief Get the values along the frequency axis, \f$ \{ \nu \} \f$

      \param selection -- Mask for the selection of frequency channels

      \return frequencyValues -- The frequency values associated with the FFT
    */
    vector<double> frequencyValues (vector<bool> const &selection);

    /*!
      \brief Get the values along the frequency axis, \f$ \{ \nu \} \f$

      \param range -- Frequency range, [Hz], for which the values are returned

      \return frequencyValues -- The frequency values associated with the FFT
    */
    vector<double> frequencyValues (vector<double> const &range);

    /*!
      \brief Get the values along the frequency axis, \f$ \{ \nu \} \f$

      \param min -- Lower limit \f$ \nu_{\rm low} \f$ of the selected frequency
                    interval, [Hz]
      \param max -- Upper limit \f$ \nu_{\rm high} \f$ of the selected frequency
                    interval, [Hz]

      \return frequencyValues -- The frequency values associated with the FFT
    */
    vector<double> frequencyValues (double const &min,
				    double const &max);

    /*!
      \brief Get the values along the time axis

      \return timeValues -- Time values \f$ \{ t_n \f$ for the samples within
                            a data block of length \f$ N_{\rm Blocksize} \f$
			    with zero offset; i.e. this function returns the 
			    first \f$ N_{\rm Blocksize} \f$ time values (as
			    we know nothing here about blocks etc.).
    */
    inline vector<double> timeValues () {
      return timeValues (0);
    }
    
    /*!
      \brief Get the values along the time axis
      
      \param sampleOffset -- Offset \f$ n_0 \f$ in the sample number
             \f[ t[n] = t_0 + (n + n_0) T_{\rm Sample} \f]

      \return timeValues -- Time values \f$ \{ t_n \f$ for the samples within
      a data block of length \f$ N_{\rm Blocksize} \f$
    */
    vector<double> timeValues (uint const &sampleOffset);
    
    /*!
      \brief Get the values along the time axis

      \param sampleValues -- The value of the samples, for which the related
             times are returned

      \return timeValues -- Time values \f$ \{ t_n \f$ for the samples within
              a data block of length \f$ N_{\rm Blocksize} \f$
    */
    vector<double> timeValues (vector<uint> const &sampleValues);
    
    /*!
      \brief Create time axis coordinate from parameters
      
      \return coord -- The time axis as linear coordinate
    */
    LinearCoordinate timeAxis ();
    
    /*!
      \brief [optional:CASA] Create time axis coordinate from parameters

      \param crval -- Reference value in world coordinates, [s]
      \param cdelt -- Increment along the coordinate axis, [s]
      \param crpix -- Reference pixel
      
      \return coord -- The time axis as linear coordinate
    */
    LinearCoordinate timeAxis (double const &crval,
			       double const &cdelt,
			       double const &crpix);
    
    /*!
      \brief [optional:CASA] Create frequency axis coordinate from parameters
    */
    SpectralCoordinate frequencyAxis ();

    /*!
      \brief [optional:CASA] Create frequency axis coordinate from parameters
      
      \param crval -- Reference value in world coordinates, [Hz]
      \param cdelt -- Increment along the coordinate axis, [Hz]
      \param crpix -- Reference pixel
      
      \return coord -- The frequency axis as spectral coordinate
    */
    SpectralCoordinate frequencyAxis (double const &crval,
				      double const &cdelt,
				      double const &crpix);
    
  private:
    
    /*!
      \brief Unconditional copying
    */
    void copy (TimeFreq const &other);
    
    /*!
      \brief Unconditional deletion 
    */
    void destroy(void);
    
  };
  
} // Namespace CR -- end

#endif /* TIMEFREQ_H */
  
