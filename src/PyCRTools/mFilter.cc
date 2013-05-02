/**************************************************************************
 *  Filter module for the CR Pipeline Python bindings.                    *
 *                                                                        *
 *  Copyright (c) 2010                                                    *
 *                                                                        *
 *  Martin van den Akker <m.vandenakker@astro.ru.nl>                      *
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
//$FILENAME: HFILE=mFilter.def.h
//-----------------------------------------------------------------------

// ========================================================================
//
//  Header files
//
// ========================================================================

#include "core.h"
#include "math.h"
#include "mFilter.h"

// ========================================================================
//
//  Implementation
//
// ========================================================================

using namespace std;

#undef HFPP_FILETYPE
//--------------------
#define HFPP_FILETYPE CC
//--------------------

// ========================================================================
//$SECTION: Filtering functions
// ========================================================================

//$DOCSTRING: Calculates the upper half of the cross-correlation matrix of a number of antenna data in the frequency domain and adds it to the output vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hCrossCorrelationMatrix
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HComplex)(ccm)()("Upper half of the cross-correlation matrix (output) containing complex visibilities as a function of frequency. The ordering is ``ant0*ant1,ant0*ant2,...,ant1*ant2,...`` - where each ``antN`` contains many frequency bins.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HComplex)(fftdata)()("Vector containing the FFTed data of all antennas subsequently.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HInteger)(nfreq)()("Number of frequency bins per antenna.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Description:
  The length of the ccm vector is ``N * (N - 1) / 2 * N_freq``, where ``N`` is the
  number of antennas and ``N_freq`` the number of frequency bins per
  antenna. The length of the (input) vector is then ``N * N_freq``.

  Usage:
  hCrossCorrelationMatrix(ccm,fftdata,nfreq) -> ccm = ccm(old) + ccm(fftdata)
*/
template <class Iter>
void HFPP_FUNC_NAME(const Iter ccm, const Iter ccm_end,
                    const Iter fftdata,const Iter fftdata_end,
                    const HInteger nfreq)
{
  // Declaration of variables
  Iter it(ccm);
  Iter it1_start(fftdata);
  Iter it1_end(fftdata+nfreq);
  Iter it1(it1_start);
  Iter it2(it1_end);

  // Sanity check
  if (nfreq <= 0) {
    throw PyCR::ValueError("nfreq must be larger than 1");
    return;
  }
  if ((it1 < fftdata) || (it1 >= fftdata_end)) {
    throw PyCR::ValueError("Start iterator outside vector range");
    return;
  }
  if ((it2 < fftdata) || (it2 >= fftdata_end)) {
    throw PyCR::ValueError("Start iterator outside vector range");
    return;
  }

  // Implementation cross-correlation
  while (it != ccm_end) {
    *it += (*it1) * conj(*it2);
    ++it; ++it1; ++it2;
    if (it1 == it1_end) {
      if (it2 >= fftdata_end) {
	it1_start += nfreq;
	it1_end += nfreq;
	if (it1_end >= fftdata_end) return;
	it2 = it1_end;
      };
      it1 = it1_start;
    };
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

// ========================================================================
//
//  Hanning filter
//
// ========================================================================

//-----------------------------------------------------------------------
//$DOCSTRING: Create a Hanning filter.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hGetHanningFilter
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Return vector containing Hanning filter")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(Alpha)()("Height parameter of Hanning function")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_2 (HInteger)(Beta)()("Width parameter of Hanning function")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_3 (HInteger)(BetaRise)()("Rising slope parameter of Hanning function")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_4 (HInteger)(BetaFall)()("Falling slope parameter of Hanning function")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END ----------------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Plot:
  import pycrtools as cr
  import matplotlib.pyplot as plt

  f1 = cr.hArray(float, 100)
  f2 = f1.new()
  f3 = f1.new()
  f4 = f1.new()

  f1.gethanningfilter()
  f2.gethanningfilter(0.6)
  f3.gethanningfilter(0.5, 40)
  f4.gethanningfilter(0.5, 1, 20, 60)

  plt.plot(f1, label='default (Alpha=0.5, Beta=0)')
  plt.plot(f2, label='Alpha=0.6')
  plt.plot(f3, label='Beta=40')
  plt.plot(f4, label='BetaRise=20, BetaFall=60')
  plt.legend(loc='lower center')
*/
template <class Iter>
void hGetHanningFilter(const Iter vec, const Iter vec_end,
		       const HNumber Alpha,
		       const HInteger Beta,
		       const HInteger BetaRise,
		       const HInteger BetaFall) {

  HInteger blocksize = vec_end - vec;

  // Sanity check
  if (blocksize < 1) {
    throw PyCR::ValueError("Vector size must be larger than 1");
    return;
  }
  if (blocksize < Beta) {
    throw PyCR::ValueError("Beta must be smaller or equal to vector size");
    return;
  }
  if (Beta == 0) {
    if (BetaRise < 1) {
      throw PyCR::ValueError("BetaRise must be larger than 1");
      return;
    }
    if (BetaFall < 1) {
      throw PyCR::ValueError("BetaFall must be larger than 1");
      return;
    }
  }

  // Implementation of the Hanning filter
  Iter it_v = vec;
  HInteger idx = 0;

  if (Beta == 0) {            // No width restrictions
    HNumber phase_factor = 2*M_PI/(blocksize-1);

    while ((it_v != vec_end) && (idx < blocksize)) {
      *it_v = (IterValueType) (Alpha - (1-Alpha)*cos(idx*phase_factor));
      ++it_v; ++idx;
    }
  } else {                    // Use rise- and fall widths
    HNumber phase(0.);
    HNumber phase_factor_rise = M_PI/(BetaRise - 1);
    HNumber phase_factor_fall = M_PI/(BetaFall - 1);

    while ((it_v != vec_end) && (idx < blocksize)) {
      if (idx < BetaRise) {     // Rising part
        phase = idx * phase_factor_rise;
        *it_v = (IterValueType) (Alpha - (1 - Alpha)*cos(phase));
      } else if (idx >= (blocksize - BetaFall)) { //  Falling part
        phase = (idx - blocksize + BetaFall) * phase_factor_fall + M_PI;
        *it_v = (IterValueType) (Alpha - (1 - Alpha)*cos(phase));
      } else {                  // Flat part
        *it_v = (IterValueType) (1.);
      }
      ++it_v; ++idx;
    }
  }
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//-----------------------------------------------------------------------
//$DOCSTRING: Create a Hanning filter.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hGetHanningFilter
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Return vector containing Hanning filter")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(Alpha)()("Height parameter of Hanning function")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_2 (HInteger)(Beta)()("Width parameter of Hanning function")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END ----------------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class Iter>
void hGetHanningFilter(const Iter vec, const Iter vec_end,
		       const HNumber Alpha,
		       const HInteger Beta) {
  HInteger blocksize = vec_end - vec;
  HInteger BetaRise = (blocksize - Beta)/2;
  HInteger BetaFall = (blocksize - Beta)/2;

  hGetHanningFilter(vec, vec_end, Alpha, Beta, BetaRise, BetaFall);
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"



//-----------------------------------------------------------------------
//$DOCSTRING: Create a Hanning filter.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hGetHanningFilter
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Return vector containing Hanning filter")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(Alpha)()("Height parameter of Hanning function")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END ----------------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class Iter>
void hGetHanningFilter(const Iter vec, const Iter vec_end,
		       const HNumber Alpha) {
  HInteger Beta = 0;
  hGetHanningFilter(vec, vec_end, Alpha, Beta);
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"



//-----------------------------------------------------------------------
//$DOCSTRING: Create a Hanning filter.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hGetHanningFilter
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Return vector containing Hanning filter")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END ----------------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class Iter>
void HFPP_FUNC_NAME(const Iter vec, const Iter vec_end){
  HNumber Alpha = 0.5;
  hGetHanningFilter(vec, vec_end, Alpha);
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//-----------------------------------------------------------------------
//$DOCSTRING: Creates one half (i.e. rising or falling part) of a Hanning filter and add an offset
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hGetHanningFilterHalf
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Return vector containing Hanning filter")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(height)()("Height of the Hanning function.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_2 (HNumber)(offset)()("Offset added to the Hanning function.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_3 (bool)(falling)()("Return first (False) or second half (True).")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END ----------------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Plot:
  import pycrtools as cr
  import matplotlib.pyplot as plt

  f1 = cr.hArray(float, 100)
  f2 = cr.hArray(float, 100)

  f1.gethanningfilterhalf(1., 0., False)
  f2.gethanningfilterhalf(1., 1., True)

  plt.plot(f1, label='height=1, offset=0, falling=False')
  plt.plot(f2, label='height=1, offset=1, falling=True')
  plt.legend(loc='lower center')
*/
template <class Iter>
void HFPP_FUNC_NAME(const Iter vec, const Iter vec_end,
		    const HNumber height,
		    const HNumber offset,
		    const bool falling
		       ) {
  // Alpha = 0.5
  // Beta = 0 (beta_rise = beta_fall = blocksize)
  HInteger blocksize = vec_end - vec;

  // Sanity check
  if (blocksize < 1) {
    throw PyCR::ValueError("Vector size must be larger than 1");
    return;
  }

  HNumber phase(0);
  HNumber phase_factor = M_PI/(blocksize - 1);
  HNumber phase_offset = 0;

  Iter it_v = vec;
  HInteger idx = 0;

  if (falling) {
    phase_offset = blocksize;
  }

  while ((it_v != vec_end) && (idx < blocksize)) {
    phase = (idx + phase_offset) * phase_factor;
    // Use optimized hanning function with alpha=0.5 and beta=0.
    *it_v = (IterValueType) (0.5 * (1. - cos(phase)));
    *it_v *= height;
    *it_v += offset;
    ++it_v; ++idx;
  }
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//-----------------------------------------------------------------------
//$DOCSTRING: Apply a predefined filter on a vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hApplyFilter
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_1)(data)()("Vector containing the data on which the filter will be applied.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_2)(filter)()("Vector containing the filter.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END ----------------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class Iter, class IterFilter>
void HFPP_FUNC_NAME(const Iter data, const Iter data_end, const IterFilter filter, const IterFilter filter_end){
  Iter       it_d = data;
  IterFilter it_f = filter;

  while ((it_d != data_end) && (it_f != filter_end)) {
    *it_d *= hfcast<IterValueType>(*it_f);
    it_d++; it_f++;
  }
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//-----------------------------------------------------------------------
//$DOCSTRING: Apply a Hanning filter to the start and end of a vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hApplyHanningFilterStartEnd
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(data)()("Input and return vector containing the data on which the Hanning filter will be applied.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HInteger)(flat_start)()("Start sample of the flat part.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_2 (HInteger)(flat_end)()("End sample of the flat part.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END ----------------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class Iter>
void HFPP_FUNC_NAME(const Iter data, const Iter data_end,
                    const HInteger flat_start, const HInteger flat_end)
{
  const HInteger N = data_end - data;
  const HInteger nbefore = flat_start;
  const HInteger nafter = N - flat_end - 1;

  Iter *data_it = data;

  HInteger i=0;
  while (data_it != data_end)
  {
    if (data_it - data < flat_start)
    {
      *data_it *= 0.5 * (1 - cos(M_PI * i/nbefore));
    }
    else if (data_it - data > flat_end)
    {
      *data_it *= 0.5 * (1 - cos(M_PI * (N-i)/nafter));
    }
    data_it++;
    i++;
  }
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//-----------------------------------------------------------------------
//$DOCSTRING: Apply a Hanning filter on a vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hApplyHanningFilter
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(data)()("Input and return vector containing the data on which the Hanning filter will be applied.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END ----------------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class Iter>
void HFPP_FUNC_NAME(const Iter data, const Iter data_end){
  HInteger blocksize = data_end - data;
  vector<HNumber> filter(blocksize);

  hGetHanningFilter(filter.begin(), filter.end());
  hApplyFilter(data, data_end, filter.begin(), filter.end());
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//-----------------------------------------------------------------------
//$DOCSTRING: Apply a Hamming filter on a vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hApplyHammingFilter
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Vector.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END ----------------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class Iter>
void HFPP_FUNC_NAME(const Iter data, const Iter data_end)
{
  HInteger N = std::distance(data, data_end);

  Iter it = data;

  for (HInteger n=0; n<N; n++)
  {
    *it++ = 0.54 - 0.46 * cos(2 * M_PI * n / (N - 1));
  }
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//-----------------------------------------------------------------------
//$DOCSTRING: Apply a Hilbert transform on a vector of full complex data (e.g. N -> N FFT not N -> N / 2 + 1).
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hApplyHilbertTransformFC
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HComplex)(vec)()("Vector.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END ----------------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class Iter>
void HFPP_FUNC_NAME(const Iter data, const Iter data_end)
{
  HInteger N = std::distance(data, data_end);

  if (N % 2 != 0) {
    throw PyCR::ValueError("[hApplyHilbertTransform] vector size must be even (need full complex FFT)");
  }

  Iter it = data;

  // Zero out DC component (required for definition H(c) = 0)
  *it++ = HComplex(0, 0);

  // Shift phases of positive frequencies by -pi/2
  for (HInteger n=1; n<N/2; n++)
  {
    *it = HComplex(0, -1) * *it;
    it++;
  }

  // Shift phases of negative frequencies by +pi/2
  for (HInteger n=0; n<N/2; n++)
  {
    *it = HComplex(0, 1) * *it;
    it++;
  }
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//-----------------------------------------------------------------------
//$DOCSTRING: Apply a Hilbert transform on a vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hApplyHilbertTransform
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HComplex)(vec)()("Vector.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END ----------------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class Iter>
void HFPP_FUNC_NAME(const Iter data, const Iter data_end)
{
  HInteger N = std::distance(data, data_end);

  Iter it = data;

  // Zero out DC component (required for definition H(c) = 0)
  *it++ = HComplex(0, 0);

  // Shift phases of positive frequencies by -pi/2
  for (HInteger n=1; n<N; n++)
  {
    *it = HComplex(0, -1) * *it;
    it++;
  }
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

