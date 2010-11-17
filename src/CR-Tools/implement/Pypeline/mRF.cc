/**************************************************************************
 *  RF module for the CR Pipeline Python bindings.                        *
 *                                                                        *
 *  Copyright (c) 2010                                                    *
 *                                                                        *
 *  Martin van den Akker <martinva@astro.ru.nl>                           *
 *                                                                        *
 *  This library is free software: you can redistribute it and/or modify  *
 *  it under the terms of the GNU General Public License as published by  *
 *  the Free Software Foundation, either version 3 of the License, or     *
 *  (at your option) any later version.                                   *
 *                                                                        *
 *  This library is distributed in the hope that it will be useful,       *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *  GNU General Public License for more details.                          *
 *                                                                        *
 *  You should have received a copy of the GNU General Public License     *
 *  along with this library. If not, see <http://www.gnu.org/licenses/>.  *
 **************************************************************************/

// ========================================================================
//
//  Wrapper Preprocessor Definitions
//
// ========================================================================

//-----------------------------------------------------------------------
//Some definitions needed for the wrapper preprosessor:
//-----------------------------------------------------------------------
//$FILENAME: HFILE=mRF.def.h
//-----------------------------------------------------------------------

// ========================================================================
//
//  Header files
//
// ========================================================================

#include "core.h"
#include "mMath.h"
#include "mVector.h"
#include "mFilter.h"
#include "mRF.h"

#include <Calibration/RFIMitigation.h>



// ========================================================================
//
//  Implementation
//
// ========================================================================

#undef HFPP_FILETYPE
//--------------------
#define HFPP_FILETYPE CC
//--------------------

// ========================================================================
//$SECTION: Functions for Radio Frequency data analysis
// ========================================================================

// ========================================================================
//
//  Wrapper functions
//
// ========================================================================

//$DOCSTRING: Calculates the square root of the power of a complex spectrum and add it to an output vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hSpectralPower
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_FUNC_MASTER_ARRAY_PARAMETER 1 // Use the second parameter as the master array for looping and history informations
#define HFPP_PARDEF_0 (HNumber)(outvec)()("Vector containing a copy of the input values converted to a new type")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HComplex)(vecin)()("Input vector containing the complex spectrum. (Looping parameter)")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!

  \brief $DOCSTRING
  $PARDOCSTRING

  The fact that the result is added to the output vector allows one to
  call the function multiple times and get a summed spectrum. If you
  need it only once, just fill the vector with zeros.

  The number of loops (if used with an hArray) is here determined by
  the second parameter!

Example:
spectrum=hArray(float,[1,128])
cplxfft=hArray(complex,[10,128],fill=1+0j)
spectrum[...].spectralpower(cplxfft[...])
*/
template <class Iter, class Iterin>
void HFPP_FUNC_NAME(const Iter vecout, const Iter vecout_end, const Iterin vecin, const Iterin vecin_end)
{
  // Declaration of variables
  Iterin itin(vecin);
  Iter itout(vecout);

  // Sanity check
  if ((vecin_end<vecin) || (vecout_end<vecout)) {
    throw PyCR::ValueError("Incorrect size of input vector.");
    return;
  }

  // Calculation of spectral power
  while ((itin != vecin_end) && (itout != vecout_end)) {
    *itout+=sqrt(real((*itin)*conj(*itin)));
    ++itin; ++itout;
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"



//$DOCSTRING: Convert the ADC value to a voltage.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hADC2Voltage
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Numeric input and output vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(adc2voltage)()("Scaling factor of the gain")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END ----------------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class Iter>
void HFPP_FUNC_NAME(const Iter vec, const Iter vec_end, const HNumber adc2voltage) {
  Iter it = vec;
  while(it != vec_end) {
    *it *= adc2voltage;
    it++;
  }
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


// ___________________________________________________________________
//                                                    hRFIDownsampling

//$DOCSTRING: Generate a downsampled vector containing the mean and rms values of the spectrum amplitudes. This is needed to generate a baseline for the RFI mitigation.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hRFIDownsampling
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HNumber)(amplitudeVec)()("Return vector containing the average value of the amplitude of the resampled spectrum bin.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(rmsVec)()("Return vector containing the root mean square value of the amplitude of the resampled spectrum bin.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HNumber)(spectrumVec)()("Vector containing the amplitude of the spectrum data.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END ----------------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class Iter>
void HFPP_FUNC_NAME (Iter amplitudeVec, Iter amplitudeVec_end,
		     Iter rmsVec, Iter rmsVec_end,
		     const Iter spectrumVec, const Iter spectrumVec_end
		     ) {
  CR::RFIMitigation rfiMitigation;

  // === Check vector size ===================================
  uint fullSize = spectrumVec_end - spectrumVec;
  uint resampledSize = amplitudeVec_end - amplitudeVec;

  uint amplitudeVecSize = amplitudeVec_end - amplitudeVec;
  uint rmsVecSize = rmsVec_end - rmsVec;
  if (amplitudeVecSize != rmsVecSize) {
    throw PyCR::ValueError("Output vectors are not of the same size.");
    cout << "ERROR: Output vectors are not of the same size." << endl;
  }

  // === Create correct vector types =========================
  IPosition shape_FullSize(1, fullSize);
  IPosition shape_ResampledSize (1, resampledSize);
  Vector<Double> spectrumVector(shape_FullSize, reinterpret_cast<Double*>(&(*spectrumVec)), casa::SHARE);
  Vector<Double> amplitudeVector(shape_ResampledSize, reinterpret_cast<Double*>(&(*amplitudeVec)), casa::SHARE);
  Vector<Double> rmsVector(shape_ResampledSize, reinterpret_cast<Double*>(&(*rmsVec)), casa::SHARE);

  // === Call RM::doDownSampling() ===========================
  //  cout << "Warning: functionality not implemented yet." << endl;
  rfiMitigation.doDownsampling(spectrumVector,
			       resampledSize,
			       amplitudeVector,
			       rmsVector);
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


// ___________________________________________________________________
//                                                 hRFIBaselineFitting

//$DOCSTRING: Perform a baseline fitting on the amplitude of the spectrum vector and use the fit to create an interpolated spectrum.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hRFIBaselineFitting
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HNumber)(fitVec)()("Output vector containing the baseline fit.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(amplitudeVec)()("Input vector containing the average value of the amplitude of the resampled spectrum bin.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HNumber)(rmsVec)()("Input vector containing the root mean square value of the amplitude of the resampled spectrum bin.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_3 (HNumber)(rmsThresholdValue)()("RMS values above this threshold value will be excluded from the fit.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END ----------------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class Iter>
void hRFIBaselineFitting(Iter fitVec, Iter fitVec_end,
			 const Iter amplitudeVec, const Iter amplitudeVec_end,
			 const Iter rmsVec, const Iter rmsVec_end,
			 const HNumber rmsThresholdValue
			 ) {
  CR::RFIMitigation rfiMitigation;
  int fitMethodType = 0; // Enumerator for different fit methods will be added

  // === Check vector size ===================================
  uint fullSize = fitVec_end - fitVec;
  uint resampledSize = amplitudeVec_end - amplitudeVec;

  uint amplitudeVecSize = amplitudeVec_end - amplitudeVec;
  uint rmsVecSize = rmsVec_end - rmsVec;
  if (amplitudeVecSize != rmsVecSize) {
    throw PyCR::ValueError("Input vectors are not of the same size.");
    cout << "ERROR: Input vectors are not of the same size." << endl;
    exit(0);
  }

  // === Create correct vector types =========================
  IPosition shape_FullSize(1,fullSize);
  IPosition shape_resampledsize(1,resampledSize);
  Vector<Double> amplitudeVector(shape_resampledsize, reinterpret_cast<Double*>(&(*amplitudeVec)), casa::SHARE);
  Vector<Double> rmsVector(shape_resampledsize, reinterpret_cast<Double*>(&(*rmsVec)), casa::SHARE);
  Vector<Double> fitVector(shape_FullSize, reinterpret_cast<Double*>(&(*fitVec)), casa::SHARE);

  // === call RM::doBaselineFitting() ========================
  //  cout << "Warning: functionality not implemented yet." << endl;
  rfiMitigation.doBaselineFitting(amplitudeVector,
				  rmsVector,
				  rmsThresholdValue,
				  fitVector,
				  fitMethodType);
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


// ___________________________________________________________________
//                                                        hRFIFlagging

//$DOCSTRING: Create a flag vector defining the regions that need to be mitigated in the spectrum.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hRFIFlagging
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HInteger)(flagVec)()("Vector describing which parts of the spectrum vector need to be mitigated.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(flagThresholdValue)()("Value of the threshold above which a frequency bin will be flagged.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_2 (HNumber)(spectrumVec)()("Input vector containing the spectrum.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_3 (HNumber)(fitVec)()("Input vector containing the baseline fit of the spectrum.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END ----------------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class IterN, class IterI>
void hRFIFlagging(IterI flagVec, IterI flagVec_end,
		  const HNumber flagThresholdValue,
		  const IterN spectrumVec, const IterN spectrumVec_end,
		  const IterN fitVec, const IterN fitVec_end
		  ) {
  CR::RFIMitigation rfiMitigation;

  // === Check vector size ===================================
  uint fullSize = spectrumVec_end - spectrumVec;

  uint fitVecSize = fitVec_end - fitVec;
  uint flagVecSize = flagVec_end - flagVec;
  if ((fitVecSize != fullSize) ||
      (flagVecSize != fullSize)) {
    throw PyCR::ValueError("Input vectors are not of the same size.");
    cout << "ERROR: Input vectors are not of the same size." << endl;
    exit(0);
  }

  // === create correct vector types =========================
  IPosition shape_FullSize(1,fullSize);
  Vector<Double> spectrumVector(shape_FullSize, reinterpret_cast<Double*>(&(*spectrumVec)), casa::SHARE);
  Vector<Double> fitVector(shape_FullSize, reinterpret_cast<Double*>(&(*fitVec)), casa::SHARE);
  Vector<Int> flagVector(shape_FullSize, reinterpret_cast<Int*>(&(*flagVec)), casa::SHARE);

  // === call RM::doFlagging() ===============================
  //  cout << "Warning: functionality not implemented yet." << endl;
  rfiMitigation.doRFIFlagging(spectrumVector,
			      fitVector,
			      flagThresholdValue,
			      flagVector);
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


// ___________________________________________________________________
//                                                      hRFIMitigation

//$DOCSTRING: Final part of the RFI mitigation: remove the flagged regions from the spectrum
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hRFIMitigation
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HNumber)(mitigatedSpectrumVec)()("Output vector containing the mitigated spectrum.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(spectrumVec)()("Input vector containing the spectrum.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HNumber)(fitVec)()("Input vector containing the baseline fit of the spectrum.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_3 (HInteger)(flagVec)()("Input vector describing which parts of the spectrum vector need to be mitigated.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END ----------------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class IterN, class IterI>
void hRFIMitigation(IterN mitigatedSpectrumVec, IterN mitigatedSpectrumVec_end,
		    const IterN spectrumVec, const IterN spectrumVec_end,
		    const IterN fitVec, const IterN fitVec_end,
		    const IterI flagVec, const IterI flagVec_end
		    ) {
  CR::RFIMitigation rfiMitigation;

  // === Check vector size ===================================
  uint fullSize = spectrumVec_end - spectrumVec;

  uint fitVecSize = fitVec_end - fitVec;
  uint flagVecSize = flagVec_end - flagVec;
  uint mitigatedSpectrumVecSize = mitigatedSpectrumVec_end - mitigatedSpectrumVec;

  if ((fitVecSize != fullSize) ||
      (flagVecSize != fullSize) ||
      (mitigatedSpectrumVecSize != fullSize)) {
    throw PyCR::ValueError("Input and output vectors are not of the same size.");
    cout << "ERROR: Input and output vectors are not of the same size." << endl;
    exit(0);
  }

  // === Create correct vector types =========================
  IPosition shape_FullSize(1,fullSize);
  Vector<Double> spectrumVector(shape_FullSize, reinterpret_cast<Double*>(&(*spectrumVec)), casa::SHARE);
  Vector<Int> flagVector(shape_FullSize, reinterpret_cast<Int*>(&(*flagVec)), casa::SHARE);
  Vector<Double> fitVector(shape_FullSize, reinterpret_cast<Double*>(&(*fitVec)), casa::SHARE);
  Vector<Double> mitigatedSpectrumVector(shape_FullSize, reinterpret_cast<Double*>(&(*mitigatedSpectrumVec)), casa::SHARE);

  // === call RM::doRFIMitigation() ==========================
  //  cout << "Warning: functionality not implemented yet." << endl;
  rfiMitigation.doRFIMitigation(spectrumVector,
				fitVector,
				flagVector,
				mitigatedSpectrumVector);
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"
