/**************************************************************************
 *  FFT module for the CR Pipeline Python bindings.                       *
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
//$FILENAME: HFILE=mFFT.def.h
//-----------------------------------------------------------------------

// ========================================================================
//
//  Header files
//
// ========================================================================

#include "core.h"
#include "mVector.h"
#include "mMath.h"
#include "mFFT.h"

#include "casacore/scimath/Mathematics/FFTServer.h"

#include <fftw3.h>


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
//$SECTION: FFT functions
// ========================================================================

//-----------------------------------------------------------------------
//$DOCSTRING: Apply a forward FFT on a complex vector and return it in a second
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hFFTw
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HComplex)(data_out)()("Return vector in which the FFT transformed data is stored.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HComplex)(data_in)()("Complex Input vector to make the FFT from.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END ----------------------------------------------------------
/*!
  outvec.fftw(invec) -> return FFT of invec in outvec

  \brief $DOCSTRING
  $PARDOCSTRING

  This implementation uses fftw3 - for more information see: http://www.fftw.org/fftw3.pdf

The DFT results are stored in-order in the array out, with the
zero-frequency (DC) component in data_out[0]. If data_in != data_out, the transform
is out-of-place and the input array in is not modified. Otherwise, the
input array is overwritten with the transform.

Users should note that FFTW computes an unnormalized DFT. Thus,
computing a forward followed by a backward transform (or vice versa)
results in the original array scaled by n.

*/
template <class Iter>
void HFPP_FUNC_NAME(const Iter data_out, const Iter data_out_end,
		    const Iter data_in,  const Iter data_in_end)
{
  // Declaration of variables
  int lenIn  = data_in_end - data_in;
  int lenOut = data_out_end - data_out;
  fftw_plan p;

  // Sanity check
  if (lenIn != lenOut) {
    throw PyCR::ValueError("In- and output vectors do not have the same size.");
    // ERROR(BOOST_PP_STRINGIZE(HFPP_FUNC_NAME) <<": input and output vector have different sizes! N=" << lenIn << " Nout=" << lenOut);
    return;
  };

  // Implementation
  p = fftw_plan_dft_1d(lenIn, (fftw_complex*) &(*data_in), (fftw_complex*) &(*data_out), FFTW_FORWARD, FFTW_ESTIMATE);
  fftw_execute(p); /* repeat as needed */
  fftw_destroy_plan(p);
  fftw_cleanup();
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//-----------------------------------------------------------------------
//$DOCSTRING: Apply a backward FFT on a complex vector and return it in a second (scrambles input vector)
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hInvFFTw
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HComplex)(data_out)()("Return vector in which the FFT transformed data is stored.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HComplex)(data_in)()("Complex Input vector to make the FFT from.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END ----------------------------------------------------------
/*!

  outvec.fftw(invec) -> return backward FFT of invec in outvec

  \brief $DOCSTRING
  $PARDOCSTRING

  This implementation uses fftw3 - for more information see: http://www.fftw.org/fftw3.pdf

  !!!! Note that the input vector will be modified and scrambled !!!!

The DFT results are stored in-order in the array out, with the
zero-frequency (DC) component in data_out[0]. If data_in != data_out, the transform
is out-of-place and the input array in is not modified. Otherwise, the
input array is overwritten with the transform.

Users should note that FFTW computes an unnormalized DFT. Thus,
computing a forward followed by a backward transform (or vice versa)
results in the original array scaled by n.

*/
template <class Iter>
void HFPP_FUNC_NAME(const Iter data_out, const Iter data_out_end,
		    const Iter data_in,  const Iter data_in_end)
{
  // Declaration of variables
  int lenIn  = data_in_end - data_in;
  int lenOut = data_out_end - data_out;
  fftw_plan p;

  // Sanity check
  if (lenIn != lenOut) {
    throw PyCR::ValueError("In- and output vectors do not have the same size.");
    // ERROR(BOOST_PP_STRINGIZE(HFPP_FUNC_NAME) <<": input and output vector have different sizes! N=" << lenIn << " Nout=" << lenOut);
    return;
  };

  // Implementation
  p = fftw_plan_dft_1d(lenIn, (fftw_complex*) &(*data_in), (fftw_complex*) &(*data_out), FFTW_BACKWARD, FFTW_ESTIMATE);
  fftw_execute(p); /* repeat as needed */
  fftw_destroy_plan(p);
  fftw_cleanup();
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//-----------------------------------------------------------------------
//$DOCSTRING: Apply a backward FFT on a complex vector and return it properly scaled and without scrambling the input vector
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hSaveInvFFTw
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_CLASSES HFPP_CLASS_STL HFPP_CLASS_hARRAY HFPP_CLASS_hARRAYALL
#define HFPP_FUNCDEF (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HComplex)(data_out)()("Return vector in which the FFT transformed data is stored.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HComplex)(data_in)()("Complex Input vector to make the FFT from.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HInteger)(nyquistZone)()("Nyquist zone")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END ----------------------------------------------------------
/*!

  outvec.fftw(invec) -> return backward FFT of invec in outvec

  \brief $DOCSTRING
  $PARDOCSTRING

  This implementation uses fftw3 - for more information see: http://www.fftw.org/fftw3.pdf

  This inverse fft will preserve the input (at the cost of
  performance, compared to the pure hInvFFTw). It will also swap the
  data back if it was previously swapped by hNyquistSwap (no swap if
  NyquistZone=0) and multiply with the correct normalisation factor
  (1/N).

  The DFT results are stored in-order in the array out, with the
  zero-frequency (DC) component in data_out[0].

See also: hFFTw, hInvFFTw

*/
template <class Iter>
void HFPP_FUNC_NAME(const Iter data_out, const Iter data_out_end,
		    const Iter data_in,  const Iter data_in_end,
		    const HInteger nyquistZone)
{
  // Declaration of variables
  int lenIn  = data_in_end - data_in;
  int lenOut = data_out_end - data_out;
  fftw_plan p;
  vector<IterValueType> scratchfft(lenIn);

  // Sanity check
  if (lenIn != lenOut) {
    throw PyCR::ValueError("In- and output vectors do not have the same size.");
    // ERROR(BOOST_PP_STRINGIZE(HFPP_FUNC_NAME) <<": input and output vector have different sizes! N=" << lenIn << " Nout=" << lenOut);
    return;
  };

  // Implementation
  PyCR::Vector::hCopy(scratchfft.begin(),scratchfft.end(), data_in,  data_in_end);
  hNyquistSwap(scratchfft,nyquistZone);
  p = fftw_plan_dft_1d(lenIn, (fftw_complex*) &scratchfft[0], (fftw_complex*) &(*data_out), FFTW_BACKWARD, FFTW_ESTIMATE);
  fftw_execute(p); /* repeat as needed */
  PyCR::Math::hDiv2(data_out,data_out_end,(IterValueType)lenIn);
  fftw_destroy_plan(p);
  fftw_cleanup();
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//-----------------------------------------------------------------------
//$DOCSTRING: Reorders the elements in a complex vector that was created by a FFT according to its Nyquistzone, such that frequencies always increase from left to right
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hNyquistSwap
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HComplex)(vec)()("Complex input and output vector.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HInteger)(nyquistZone)()("Nyquist zone. 1: nu= 0 - Nu_max, 2: nu=Nu_max - 2 * Nu_max, etc ...  ")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END ----------------------------------------------------------
/*!
  vec.nyquistswap()

  \brief $DOCSTRING
  $PARDOCSTRING

  The order of the elements in the vector will be reversed and replaced with their negative complex conjugate.

  The operation is only performed for an even Nyquist Zone (2,4,6,..). Otherwise nothing is done.

*/
template <class Iter>
void HFPP_FUNC_NAME(const Iter vec, const Iter vec_end,
		    const HInteger nyquistZone)
{
  if (hfodd(nyquistZone) || (nyquistZone==0)) return;
  Iter it1(vec),it2(vec_end-1);
  HComplex tmp;
  while (it2 >= it1) {
    tmp=*it1;
    *it1=-conj(*it2);
    *it2=-conj(tmp);
    ++it1; --it2;
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//-----------------------------------------------------------------------
//$DOCSTRING: Apply a complex-to-real (inverse) FFT using fftw3 and return it properly scaled and without scrambling the input vector
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hSaveInvFFTw
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_CLASSES HFPP_CLASS_STL HFPP_CLASS_hARRAY HFPP_CLASS_hARRAYALL
#define HFPP_FUNCDEF (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HNumber)(data_out)()("Return vector of N real elements in which the inverse FFT (DFT) transformed data is stored. ")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HComplex)(data_in)()("Complex input vector of The length is N/2+1 elements to make the FFT from.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HInteger)(nyquistZone)()("Nyquist zone")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END ----------------------------------------------------------
/*!

  outvec.fftw(invec) -> return backward FFT of invec in outvec

  \brief $DOCSTRING
  $PARDOCSTRING

  This implementation uses fftw3 - for more information see: http://www.fftw.org/fftw3.pdf

  This inverse fft will preserve the input (at the cost of
  performance, compared to the pure hInvFFTw). It will also swap the
  data back if it was previously swapped by hNyquistSwap (no swap if
  NyquistZone=0) and multiply with the correct normalisation factor
  (1/N).

  The DFT results are stored in-order in the array out, with the
  zero-frequency (DC) component in data_out[0].

See also: hFFTw, hInvFFTw

*/
template <class IterOut,class Iter>
void HFPP_FUNC_NAME(const IterOut data_out, const IterOut data_out_end,
		    const Iter  data_in,  const Iter  data_in_end,
		    const HInteger nyquistZone)
{
  // Declaration of variables
  int lenIn = data_in_end - data_in;
  int lenOut = data_out_end - data_out;
  vector<IterValueType> scratchfft(lenOut);
  fftw_plan p;

  // Sanity check
  if (lenIn != (lenOut/2+1)) {
    throw PyCR::ValueError("Input or output vector has the wrong size. This should be: N(in) = N(out)/2+1.");
    //ERROR(BOOST_PP_STRINGIZE(HFPP_FUNC_NAME) << ": input and output vector have wrong sizes! N(out)=" << N  << " N(in)=" << Nin<< " (should be = " << N/2+1 << ")");
    return;
  };

  // Implementation
  PyCR::Vector::hCopy(scratchfft.begin(),scratchfft.end(), data_in,  data_in_end);
  hNyquistSwap(scratchfft,nyquistZone);
  p = fftw_plan_dft_c2r_1d(lenOut, (fftw_complex*) &(scratchfft[0]), (double*) &(*data_out), FFTW_ESTIMATE);
  fftw_execute(p); /* repeat as needed */
  fftw_destroy_plan(p);
  fftw_cleanup();
  PyCR::Math::hDiv2(data_out,data_out_end,(HNumber)lenOut);
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//-----------------------------------------------------------------------
//$DOCSTRING: Apply a real-to-complex FFT using fftw3
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hFFTw
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HComplex)(data_out)()("Return vector in which the FFT (DFT) transformed data is stored. The length is N/2+1.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(data_in)()("Real input vector of N elements to make the FFT from.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END ----------------------------------------------------------
/*!
  complexvec.fftw(floatvec) -> return FFT of floatvec in complexvec

  \brief $DOCSTRING
  $PARDOCSTRING

  This implementation uses fftw3 - for more information see: http://www.fftw.org/fftw3.pdf

Users should note that FFTW computes an unnormalized DFT. Thus,
computing a forward followed by a backward transform (or vice versa)
results in the original array scaled by N.

The size N can be any positive integer, but sizes that are products of
small factors are transformed most efficiently (although prime sizes
still use an O(N log N) algorithm).  The two arguments are input and
output arrays of the transform.  These vectors can be equal,
indicating an in-place transform.

*/
template <class IterOut,class IterIn>
void HFPP_FUNC_NAME(const IterOut data_out, const IterOut data_out_end,
		    const IterIn  data_in,  const IterIn  data_in_end)
{
  // Declaration of variables
  int lenIn = data_in_end - data_in;
  int lenOut = data_out_end - data_out;
  fftw_plan p;

  // Sanity check
  if (lenOut != (lenIn/2+1)) {
    throw PyCR::ValueError("Input or output vector has the wrong size. This should be: N(out) = N(in)/2+1.");
    //ERROR(BOOST_PP_STRINGIZE(HFPP_FUNC_NAME) << ": input and output vector have wrong sizes! N(in)=" << lenIn << " N(out)=" << lenOut << " (should be = " << lenIn/2+1 << ")");
    return;
  };

  // Implementation
  p = fftw_plan_dft_r2c_1d(lenIn, (double*) &(*data_in), (fftw_complex*) &(*data_out), FFTW_ESTIMATE);
  fftw_execute(p); /* repeat as needed */
  fftw_destroy_plan(p);
  fftw_cleanup();
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//-----------------------------------------------------------------------
//$DOCSTRING: Apply a complex-to-real (inverse) FFT using fftw3
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hInvFFTw
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HNumber)(data_out)()("Return vector of N real elements in which the inverse FFT (DFT) transformed data is stored. ")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HComplex)(data_in)()("Complex input vector of The length is N/2+1 elements to make the FFT from.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END ----------------------------------------------------------
/*!
  vec.invfftw(complexvec) -> return (inv) FFT of complexvec in vec

  \brief $DOCSTRING
  $PARDOCSTRING

  This implementation uses fftw3 - for more information see: http://www.fftw.org/fftw3.pdf

Users should note that FFTW computes an unnormalized DFT. Thus,
computing a forward followed by a backward transform (or vice versa)
results in the original array scaled by N.

The size N can be any positive integer, but sizes that are products of
small factors are transformed most efficiently (although prime sizes
still use an O(N log N) algorithm).  The two arguments are input and
output arrays of the transform.  These vectors can be equal,
indicating an in-place transform.

*/
template <class IterOut,class IterIn>
void HFPP_FUNC_NAME(const IterOut data_out, const IterOut data_out_end,
		    const IterIn  data_in,  const IterIn  data_in_end)
{
  // Declaration of variables
  int lenIn = data_in_end - data_in;
  int lenOut = data_out_end - data_out;
  fftw_plan p;

  // Sanity check
  if (lenIn != (lenOut/2+1)) {
    throw PyCR::ValueError("Input or output vector has the wrong size. This should be: N(in) = N(out)/2+1.");
    // ERROR(BOOST_PP_STRINGIZE(HFPP_FUNC_NAME) << ": input and output vector have wrong sizes! N(out)=" << lenOut << " N(in)=" << lenIn << " (should be = " << lenOut/2+1 << ")");
    return;
  };

  // Implementation
  p = fftw_plan_dft_c2r_1d(lenOut, (fftw_complex*) &(*data_in), (double*) &(*data_out), FFTW_ESTIMATE);
  fftw_execute(p); /* repeat as needed */
  fftw_destroy_plan(p);
  fftw_cleanup();
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"




//-----------------------------------------------------------------------
//$DOCSTRING: Apply an FFT on a vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hFFTCasa
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HComplex)(data_out)()("Return vector in which the FFT transformed data is stored.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(data_in)()("Vector containing the input data of the FFT.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HInteger)(nyquistZone)()("Nyquist zone")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END ----------------------------------------------------------
/*!
  complexvec.fftcasa(floatvec) -> return FFT for floatvec in complexvec.

  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class IterOut, class IterIn>
void HFPP_FUNC_NAME(const IterOut data_out, const IterOut data_out_end,
		    const IterIn  data_in,  const IterIn  data_in_end,
		    const HInteger nyquistZone) {
  // Declaration of variables
  uint channel;
  uint blocksize(data_in_end - data_in);
  uint fftLength(blocksize/2+1);
  uint nofChannels(fftLength);
  IPosition shape_in(1,blocksize);
  IPosition shape_out(1,fftLength);
  casa::FFTServer<Double,DComplex> fftserver(shape_in, casa::FFTEnums::REALTOCOMPLEX);

  Vector<Double> cvec_in(shape_in, 0.);
  Vector<DComplex> cvec_out(shape_out, 0.);

  IterIn it_in = data_in;
  IterOut it_out = data_out;
  Vector<Double>::iterator it_vin=cvec_in.begin();

  // Sanity check
  if (fftLength != (data_out_end - data_out)) {
    throw PyCR::ValueError("Input or output vector has the wrong size. This should be: N(out) = N(in)/2+1.");
    return;
  }

  // Implementation

  // make copy of input vector since fft will also modify the order of the input data.
  while ((it_in != data_in_end) && (it_vin != cvec_in.end())) {
    *it_vin  = *it_in;
    it_in++; it_vin++;
  }

  // Apply FFT

  fftserver.fft(cvec_out, cvec_in);

  // Is there some aftercare needed (checking/setting the size of the in/output vector)
  switch (nyquistZone) {
  case 1:
  case 3:
  case 5:
    {
      it_out = data_out;
      channel = 0;
      while ((it_out != data_out_end) && (channel < nofChannels)) {
	*it_out = cvec_out(channel);
	it_out++; channel++;
      }
    }
    break;
  case 2:
  case 4:
  case 6:
    {
      /// See datareader for implementation.
      it_out = data_out;
      channel = 0;
      while ((it_out != data_out_end) && (channel < nofChannels)) {
	*it_out = conj(cvec_out(fftLength - channel - 1));
	it_out++; channel++;
      }
    }
    break;
  }
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//-----------------------------------------------------------------------
//$DOCSTRING: Apply an Inverse FFT on a vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hInvFFTCasa
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HNumber)(data_out)()("Return vector in which the inverse FFT transformed data is stored.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HComplex)(data_in)()("Vector containing the input data of the FFT.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HInteger)(nyquistZone)()("Nyquist zone")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END ----------------------------------------------------------
/*!
  floatvec.invfftcasa(complexvec) -> return inverse FFT for complexvecvec in floatvec.

  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class IterOut, class IterIn>
void HFPP_FUNC_NAME(const IterOut data_out, const IterOut data_out_end,
		    const IterIn data_in,   const IterIn data_in_end,
		    const HInteger nyquistZone) {
  // Declaration of variables
  uint channel;
  uint blocksize(data_out_end - data_out);
  uint fftLength(blocksize/2+1);
  uint nofChannels(fftLength);
  IPosition shape_in(1,fftLength);
  IPosition shape_out(1,blocksize);

  Vector<DComplex> cvec_f(shape_in, reinterpret_cast<DComplex*>(&(*data_in)), casa::SHARE);
  Vector<DComplex> cvec_in(fftLength);
  Vector<Double> cvec_out(shape_out, 0.);

  if ((data_in_end - data_in) != (int)fftLength) {
    throw PyCR::ValueError("Input or output vector has the wrong size. This should be: N(in) = N(out)/2+1.");
    // cerr << "[invfft] Bad input: len(data_in) != fftLength" << endl;
    // cerr << "  len(data_in) = " << (data_in_end - data_in) << endl;
    // cerr << "  fftLength    = " << fftLength << endl;
    return;
  };

  try {
    FFTServer<Double,DComplex> server(shape_out,
				      FFTEnums::REALTOCOMPLEX);
    switch (nyquistZone) {
    case 1:
    case 3:
      for (channel=0; channel<nofChannels; channel++) {
   	cvec_in(channel) = cvec_f(channel);
      }
      break;
    case 2:
      for (channel=0; channel<nofChannels; channel++) {
  	cvec_in(channel) = conj(cvec_f(fftLength - channel - 1));
      }
      break;
    }
    server.fft(cvec_out,cvec_in);

    // Copy result back to data_out
    Vector<Double>::iterator it_vout= cvec_out.begin();
    IterOut it_dout = data_out;
    while ((it_vout != cvec_out.end()) && (it_dout != data_out_end)) {
      *it_dout = *it_vout;
      it_vout++; it_dout++;
    }
  } catch (AipsError x) {
    cerr << "[invfft]" << x.getMesg() << endl;
  }
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"



//$DOCSTRING: Apply an FFT on a vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hFFT
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HComplex)(data_out)()("Return vector in which the FFT transformed data is stored.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(data_in)()("Vector containing the input data of the FFT.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HInteger)(nyquistZone)()("Nyquist zone")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END ----------------------------------------------------------
/*!
  complexvec.fft(floatvec) -> return FFT of floatvec in complexvec.

  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class IterOut, class IterIn>
void HFPP_FUNC_NAME(const IterOut data_out, const IterOut data_out_end,
		    const IterIn  data_in,  const IterIn  data_in_end,
		    const HInteger nyquistZone) {

  hFFTCasa(data_out, data_out_end,
	   data_in,  data_in_end,
	   nyquistZone);
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Apply an inverse FFT on a vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hInvFFT
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HNumber)(data_out)()("Return vector in which the inverse FFT transformed data is stored.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HComplex)(data_in)()("Vector containing the input data of the FFT.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HInteger)(nyquistZone)()("Nyquist zone")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END ----------------------------------------------------------
/*!
  floatvec.invfft(complexvec) -> return inverse FFT of complexvec in floatvec.

  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class IterIn, class IterOut>
void HFPP_FUNC_NAME(const IterOut data_out, const IterOut data_out_end,
		    const IterIn data_in,   const IterIn data_in_end,
		    const HInteger nyquistZone) {

  hInvFFTCasa(data_out, data_out_end,
	      data_in, data_in_end,
	      nyquistZone);
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

