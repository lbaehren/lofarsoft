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

/* $Id: AxisCoordinates.h,v 1.3 2006/07/05 16:01:08 bahren Exp $*/

#ifndef AXISCOORDINATES_H
#define AXISCOORDINATES_H

#include <stdlib.h>
#include <map>
#include <cmath>

#include <casa/Arrays.h>
#include <casa/Arrays/ArrayMath.h>

using casa::Vector;

namespace CR {  // Namespace CR -- begin

/*!
  \class AxisCoordinates

  \ingroup Coordinates

  \brief Coordinates associated with the data samples axis

  \author Lars B&auml;hren

  \date 2006/03/13

  \test tAxisCoordinates.cc

  <h3>Prerequisite</h3>

  <ul type="square">
    <li>One should be familiar with the mechanism how coordinate axes were handled
        in the original CR-Tools implementation.
	\code 
	FileOffset=0,
	FileBlocksize=65536,
	Dummystoredregion=[1 65536] ,
	Offset=0,
	Blocksize=65536,
	Filesize=65536,
	ZeroOffset=0,
	Blocknum=1,
	FFTSize=65536,
	FFTlen=32769,
	FreqBin=1220.70312,
	FrequencyLow=40000000,
	FrequencyHigh=80000000,
	PowerNormFaktor=45057,
	PhaseGradstoredregion=[1 32769] ,
	RawFFTstoredregion=[1 32769] ,
	AntennaGainstoredregion=[1 32769] ,
	FieldStrengthFactor=0.0145101371]
	\endcode
	The conversion between different types of coordinates is described in the
	<tt>Coordinfo</tt> field of the data object.
  </ul>

  <h3>Synopsis</h3>

  

  <h3>Example(s)</h3>

*/

class AxisCoordinates {

  //! Map of coordinate name/unit pairs.
  std::map<const char*, const char*> axes_p;
  //! Offset from the start of the data segment, [samples]
  int offset_p;
  //! Size of a single data block, [samples]
  int blocksize_p;
  //! Synchronization between sample and time axis
  int presync_p;
  //! Sample frequency, [Hz]
  double sampleFrequency_p;
  //! Frequency range, [Hz]
  Vector<double> frequencyRange_p;

 public:

  /*!
    \brief Names of the coordinates that can be associated with the axis
  */
  enum COORDINATE {
    //! Position
    Pos,
    //! Time, [s]
    Time,
    //! f(x), i.e. the raw ADC values 
    fx,
    //! Voltage, derived from f(x), [Volt]
    Voltage,
    //! Intermediate frequency, [Hz]
    IntermedFreq,
    //! Frequency, [Hz]
    Frequency,
    //! Absolute of the FFT
    AbsFFT,
    //! Power
    Power,
    //! Mean power
    MeanPower,
    //! Calibrated power
    CalPower,
    //! Noise temperature, [K]
    NoiseT,
    //! Sky temperature, [K]
    SkyT,
    //! ??
    MNT,
    //! Phase, [degrees]
    Phase,
    //! Phase difference, [degrees]
    PhaseDiff,
    //! Phase gradient, [degrees]
    PhaseGrad,
    //! Antenna gain, [Volt]
    AntennaGain,
    //! Beam temperature
    BeamT,
    //! Beam flag
    BeamF,
    //! Time lag
    TimeLag,
    //! Position lag
    PosLag,
    //! Cross-correlation
    CrossCorr,
    //! Power temperature
    PowerT,
    //! A flag ??
    FlagF,
    //! A flag ??
    FlagT,
    //! Flux, [Jy]
    Flux
  };

  // --------------------------------------------------------------- Construction

  /*!
    \brief Default constructor

    Constructs a new AxisCoordinates objects with some default values for the
    internal parameters:
    - frequencyRange  = [40,80] MHz
    - sampleFrequency = 80 MHz
    - blocksize       = 1024
    - offset          = 0
    - presync         = 0
  */
  AxisCoordinates ();

  /*!
    \brief Argumented constructor

    \param blocksize       -- Size of a single data block, [samples]
    \param offset          -- Offset from the start of the data segment, [samples]
    \param presync         -- Synchronization between sample and time axis
    \param sampleFrequency -- Sample frequency, [Hz]
    \param frequencyRange  -- Frequency range, [Hz]
  */
  AxisCoordinates (const int& blocksize,
		   const int& offset,
		   const int& presync,
		   const double& sampleFrequency,
		   const Vector<double>& frequencyRange);

  /*!
    \brief Copy constructor

    \param other -- Another AxisCoordinates object from which to create this new
                    one.
  */
  AxisCoordinates (AxisCoordinates const& other);

  // ---------------------------------------------------------------- Destruction

  /*!
    \brief Destructor
  */
  ~AxisCoordinates ();

  // ------------------------------------------------------------------ Operators

  /*!
    \brief Overloading of the copy operator

    \param other -- Another AxisCoordinates object from which to make a copy.
  */
  AxisCoordinates& operator= (AxisCoordinates const &other); 

  // ----------------------------------------------------------------- Parameters

  /*!
    \brief Get the size of a single data block

    \return blocksize -- Size of a single data block, [samples]
  */
  int blocksize () {
    return blocksize_p;
  }

  /*!
    \brief Set the size of a single data block

    \param blocksize -- Size of a single data block, [samples]
  */
  void setBlocksize (const int& blocksize) {
    blocksize_p = blocksize;
  }

  /*!
    \brief Get the offset from the start of the data segment

    \return offset -- Offset from the start of the data segment, [samples]
   */
  int offset () {
    return offset_p;
  }

  /*!
    \brief Set the offset from the start of the data segment

    \param offset -- Offset from the start of the data segment, [samples]
   */
  void setOffset (const int& offset) {
    offset_p = offset;
  }

  /*!
    \brief Get the presync shift

    \return presync -- The presync shift, i.e. the position where t=0
  */
  int presync () {
    return presync_p;
  }

  /*!
    \brief Set the presync shift
    
    \param presync -- The presync shift, i.e. the position where t=0
  */
  void setPresync (const int& presync) {
    presync_p = presync;
  }

  /*!
    \brief The sampling frequency

    \return sampleFrequency -- The sampling frequency, [Hz]
  */
  double sampleFrequency () {
    return sampleFrequency_p;
  }

  void setSampleFrequency (const double& sampleFrequency) {
    sampleFrequency_p = sampleFrequency;
  }

  Vector<double> frequencyRange () {
    return frequencyRange_p;
  }

  void setFrequencyRange (const Vector<double>& frequencyRange);

  /*!
    \brief Get the width of a frequency bin
    
    Thye width of a frequency bin can be derived from the frequency range and 
    the output size of the FFT:
    \f[ \delta \nu = \frac{\nu_{\rm max} - \nu_{\rm min}}{N_{\rm FFT} -1} \f]
    
    \return Width of a bin in frequency space
  */
  double frequencyBin ();
  
  /*!
    \brief Output length of the FFT

    The output length of the FFT can be derived from the <i>blocksize</i> 
    \f$ N_{\rm Blocksize} \f$:
    \f[ N_{\rm FFT} = \frac{N_{\rm Blocksize}}{2} + 1 \f]
    
    \return fftLength -- Output length of the FFT
   */
  int fftLength ();

  // -------------------------------------------------------------------- Methods

  /*!
    \brief Get the numerical values of the coordinate axis.

    \param which -- Values for which coordinate axis? This is modelled after the
                    <tt>data.get()</tt> mechanism present in the original Glish
		    implementation of the data kernel.

    \return values --Numerical values of the coordinate axis
  */
  Vector<double> axisValues (const AxisCoordinates::COORDINATE& which);

 private:

  /*!
    \brief Set up the default map of coordinate name/unit pairs.
  */
  void AxisCoordinates::setCoordinatesMap ();

  /*!
    \brief Get values for position axis
  */
  Vector<double> getPos ();

  /*!
    \brief Get values for time axis

    Time axis is derived from position axis
  */
  Vector<double> getTime ();

  /*!
    \brief Get values for frequency axis

    Frequency axis is derived from frequency range and blocksize
  */
  Vector<double> getFrequency ();

  /*!
    \brief Get values for intermediate frequency axis

    This axis is derived from the frequency bin value, \f$ \delta \nu \f$.
  */
  Vector<double> getIntermedFreq ();

  /*!
    \brief Unconditional copying
  */
  void copy (AxisCoordinates const& other);

  /*!
    \brief Unconditional deletion 
  */
  void destroy(void);

};

}  // Namespace CR -- end

#endif /* _AXISCOORDINATES_H_ */
