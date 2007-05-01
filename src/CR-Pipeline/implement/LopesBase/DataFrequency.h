
#if !defined(DATAFREQUENCY_H)
#define DATAFREQUENCY_H

/* $Id: DataFrequency.h,v 1.3 2006/10/31 18:24:08 bahren Exp $ */

// C++ Standard library
#include <iostream>
#include <ostream>
#include <fstream>
using namespace std;

#include <casa/aips.h>
#include <casa/Arrays/Array.h>
#include <casa/Arrays/Vector.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/Arrays/IPosition.h>
#include <casa/BasicSL/Complex.h>
#include <casa/Quanta/QC.h>
#include <scimath/Mathematics.h>
#include <tasking/Glish.h>
#include <tasking/Glish/GlishEvent.h>
#include <tasking/Glish/GlishRecord.h>

#include <casa/namespace.h>

/*!
  \class DataFrequency

  \ingroup LopesBase

  \brief Frequency domain settings of an observation

  \author Lars Baehren

  \test
  - tDataFrequency.cc

  <h3>Synopsis</h3>

  We build up a data structure mirroring the related contents of the LOPES Tools
  data object (essentially a Glish record):
  \verbatim
  FFTSize         := as_integer(data.getmeta('FFTSize'));
  FFTlen          := as_integer(data.getmeta('FFTlen'));
  FrequencyUnit   := as_double(data.head('FrequencyUnit'));
  SamplerateUnit  := as_double(data.head('SamplerateUnit'));
  Samplerate      := as_double(data.head('Samplerate'));
  NyquistZone     := as_integer(data.head('NyquistZone'));
  FrequencyRange  := [as_double(data.getmeta('FrequencyLow')),
                      as_double(data.getmeta('FrequencyHigh'))];
  \endverbatim
 */

class DataFrequency {

  //! Unit for the frequency values
  Double FrequencyUnit_p;
  //! Unit for the sample rate
  Double SamplerateUnit_p;
  //! Samplerate given in units of SamplerateUnit
  Double Samplerate_p;
  //! The Nyquist Zone
  Int NyquistZone_p;
  //! Maximum frequency range
  Vector<Double> FrequencyRange_p;
  //! Time domain size of FFT
  Int FFTSize_p;
  //! Frequency domain size of FFT
  Int FFTlen_p;
  //! Selected frequency bands
  Matrix<Double> FrequencyBands_p;
  //! Masking array for selection/deselection of frequency channels
  Vector<Bool> FrequencyMask_p;

 public:

  // === Construction / Destruction ============================================

  /*!
    \brief Empty constructor
  */
  DataFrequency ();

  /*!
    \brief Construction from Glish record
  */
  DataFrequency (GlishRecord&);

  /*!
    \brief Destructor
   */
/*   ~DataFrequency (); */

  // === Initialization ========================================================

  /*!
    \brief Set the data from a Glish record
  */
  void setDataFrequency (GlishRecord&);

  // === Rates and units =======================================================

  //! Set the unit in which frequency values are expressed
  void setFrequencyUnit (Double);

  //! Get the unit in which frequency values are expressed
  Double FrequencyUnit ();

  //! Set the unit in which the samplerate is expressed
  void setSamplerateUnit (Double);

  //! Get the unit in which the samplerate is expressed
  Double SamplerateUnit ();

  //! Set the samplerate, in units of the SamplerateUnit
  void setSamplerate (Double);

  //! Get the samplerate, in units of the SamplerateUnit
  Double Samplerate ();

  void setFrequencyLow (Double);

  void setFrequencyHigh (Double);

  Vector<Double> frequencyRange ();

  void setFrequencyRange (const Vector<Double>&);

  // === Frequency selection ===================================================

  /*!
    \brief Divide frequency range into sub-bands.

    Divide the available frequency range, \f$ [\nu_{\rm min},\nu_{\rm max}] \f$,
    into a set of \f$ N \f$ frequency sub-bands. Subdivision can be done
    <ul>
      <li>linear
      \f[ \nu[n] = \nu_{\rm min} + n \cdot \frac{\nu_{\rm max} -
      \nu_{\rm min}}{N_\nu} \quad , \qquad n \in [0,N_{\nu}] \f]
      <li>logarithmic
      \f[ \nu[n] = \nu_{\rm min} \cdot 10^{\left[ \log \left(
      \frac{\nu_{\rm max}}{\nu_{\rm min}} \right) \cdot \frac{n}{N_{\nu}}
      \right]} \quad , \qquad \nu \in [0,N_{\nu}] \f]
    </ul>

    \param nofBands - Number of sub-bands.
    \param division - Method to divide the frequency range into sub-bands:
                      <i>lin</i> (linear; this also is the default,
		      <i>log</i> (logarithmic).
   */
  void setFrequencyBands (const Int,
			  const String);
  /*!
    \brief Divide frequency range into sub-bands.
   
    \param freqRange - Frequency range which is about to be sub-divided into
                       bands.
    \param nofBands  - Number of sub-bands.
    \param division  - Method to divide the frequency range into sub-bands:
                       <i>lin</i> (linear; this also is the default,
		       <i>log</i> (logarithmic).
   */
  void setFrequencyBands (const Vector<Double>&,
			  const Int,
			  const String);

  /*!
    \brief Set the limits of a selected frequency band, [Hz]
    
    \param freqMIN - Lower limit of the selected frequency band.
    \param freqMAX - Upper limit of the selected frequency band.
   */
  void setFrequencyBands (const Double&, const Double&);

  /*!
    \brief Set the limits of a selected frequency band, [Hz]

    \param freqBand - Vector with the limits of a selected frequency band.
   */
  void setFrequencyBands (const Vector<Double>&);

  /*!
    \brief Set the limits of multiple selected frequency bands, [Hz]

    \param freqBands - Array with the limits of multiple selected frequency
                       bands, \f$ [N_{\rm Bands} \times 2] \f$
   */
  void setFrequencyBands (const Matrix<Double>&);

  /*!
    Get the limits of multiple selected frequency bands, [Hz]

    \return freqBands - Array with the limits of multiple selected frequency
                        bands, \f$ [N_{\rm Bands} \times 2] \f$
   */
  Matrix<Double> FrequencyBands ();

  void setFrequencyMask (Vector<Bool>&);

  /*!
    \brief Get the frequency mask
  */
  Vector<Bool> FrequencyMask ();

  /*!
   \brief Get the frequency mask for a user defined frequency band

   \param freqMin - Lower limit of the selected frequency band.
   \param freqMax - Upper limit of the selected frequency band.
  */
  Vector<Bool> FrequencyMask (const Double, const Double);
  
  //! Get the frequency mask for a user defined frequency band
  Vector<Bool> FrequencyMask (const Vector<Double>&);

  // === Access to the frequency values ========================================

  /*!
    \brief Get all allowed frequency values

    Get all frequencies within the range [FrequencyLow,FrequencyHigh] for the
    given Samplerate.

    The frequency values are computed according to
    \f[ \nu(k) = \nu_{\rm min} + k \cdot \frac{\nu_{\rm sample}}{N_{\rm FFT}} \f]
    where \f$ N_{\rm FFT} \f$ is the number of frequency bins for an FFT of an 
    input length of \f$ N \f$ samples and \f$ \nu_{\rm sample} \f$ is the
    sampling frequency.
  */
  Vector<Double> Frequencies ();

  /*!
    \brief Get all selected/deselected frequency values

    \param selected - Return the selected frequency values, i.e. those within 
                      the selected frequency bands.
   */
  Vector<Double> Frequencies (Bool);

  /*!
    \brief Get the frequency values within a defined frequency range

    Get all frequencies within the range [LimitLow,LimitHigh] for the
    given Samplerate; if the limits of the requested frequency range lie
    outside [FrequencyLow,FrequencyHigh], just the valid set of frequencies 
    will be returned.
  */
  Vector<Double> Frequencies (Double,Double);

  /*!
    \brief Get the frequency values within a defined frequency range
    
    Get all frequencies within the range [LimitLow,LimitHigh] for the
    given Samplerate; if the limits of the requested frequency range lie
    outside [FrequencyLow,FrequencyHigh], just the valid set of frequencies 
    will be returned.
  */
  Vector<Double> Frequencies (Vector<Double>&);

  /*!
    \brief Get the frequency values selected by an masking array
  */
  Vector<Double> Frequencies (Vector<Bool>&);

  /*!
    \brief Width of a frequency bin
   */
  Double FrequencyBin ();

  // === Fourier transform settings ============================================

  //! Set the Nyquist Zone in which we operate
  void setNyquistZone (Int);
  //! Get the Nyquist Zone in which we operate
  Int NyquistZone ();

  void setFFTSize (Int);
  Int FFTSize ();

  void setFFTlen (Int);
  Int FFTlen ();

  // === Conversion of quantities in the frequency domain ======================

  /*!
    \brief Convert power to noise temperature

    \f$ T = P / \left( \Delta \nu \cdot k_B \right)^{-1} \f$, where 
    <ul>
    <li>\f$ P \f$ = Power, \f$ [W = J s^{-1}] \f$
    <li>\f$ \Delta \nu \f$ = Frequency bandwidth, \f$ [{\rm Hz} = s^{-1}] \f$
    <li>\f$ k_B \f$ = Boltzman constant
    </ul>

    Origin: <tt>TIM40convPower2NoiseT</tt> (data-TIM40.g)
  */
  Vector<Double> Power2NoiseT (const Vector<Double>&);

  /*!
    \brief Convert raw/calibrated FFT data to power spectrum

    Origin: <tt>TIM40convFFT2Power</tt> (data-TIM40.g)
   */
  Vector<Double> FFT2Power (Vector<DComplex> const &fft);

  // === Logging facility to output stream =====================================

  void show (std::ostream&);
  
 private:
  
  //! Get all allowed frequency values
  Vector<Double> calcFrequencies ();

  //! Create a masking array based on a set of selected frequency bands
  Vector<Bool> maskFromBands();

  /*!
    \brief Determine the number of selected frequency channels in a masking array

    \param mask     - Masking array used for channel selection
    \param selected - Return the number of selected or deselected channels?
    \return nofChannels - Number of selected/deselected frequency channels
  */
  Int channelsInMask (const Vector<Bool>&, const Bool);

};


#endif
