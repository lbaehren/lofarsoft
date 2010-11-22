/**************************************************************************
 *  FFT module for the CR Pipeline Python bindings.                       *
 *                                                                        *
 *  Copyright (c) 2010                                                    *
 *                                                                        *
 *  Martin van den Akker <martinva@astro.ru.nl>                           *
 *  Heino Falcke <h.falcke@astro.ru.nl>                                   *     
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
  hCopy(scratchfft.begin(),scratchfft.end(), data_in,  data_in_end);
  hNyquistSwap(scratchfft,nyquistZone);
  p = fftw_plan_dft_1d(lenIn, (fftw_complex*) &scratchfft[0], (fftw_complex*) &(*data_out), FFTW_BACKWARD, FFTW_ESTIMATE);
  fftw_execute(p); /* repeat as needed */
  hDiv2(data_out,data_out_end,(IterValueType)lenIn);
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
  hCopy(scratchfft.begin(),scratchfft.end(), data_in,  data_in_end);
  hNyquistSwap(scratchfft,nyquistZone);
  p = fftw_plan_dft_c2r_1d(lenOut, (fftw_complex*) &(scratchfft[0]), (double*) &(*data_out), FFTW_ESTIMATE);
  fftw_execute(p); /* repeat as needed */
  fftw_destroy_plan(p);
  fftw_cleanup();
  hDiv2(data_out,data_out_end,(HNumber)lenOut);
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
  if (fftLength != (uint)(data_out_end - data_out)) {
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

//$DOCSTRING: Multiplies a transposed complex FFT matrix with an appropriate phase factor needed to calculate a second FFT with higher spectral resolution
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hDoubleFFTPhaseMul
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_TYPES HFPP_REAL_NUMERIC_TYPES
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HComplex)(vec)()("Complex input vector containing the Fourier-transformed and transposed data that was read in with a certain block length, stride, and offset.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HInteger)(full_size)()("Full data size of the time series.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_2 (HInteger)(nblocks)()("How many blocks were read in.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_3 (HInteger)(blocklen)()("The block length for individual data blocks that were read in.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_4 (HInteger)(offset)()("How many blocks were skipped before the first block was read in.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

See: hDoubleFFT for an extensive description
*/

template <class Iter>
void HFPP_FUNC_NAME (const Iter vec,const Iter vec_end, const HInteger full_size,  const HInteger nblocks, const HInteger blocklen, const HInteger offset)
{
  HInteger len=vec_end-vec;
  if (len!=(blocklen*nblocks)) {
    throw PyCR::ValueError("Input vector has not the dimension blocklen * nblocks.");
    return;
  };

  HComplex phase_fraction=HComplex(0.0,-CR::_2pi/full_size);
  Iter it=vec;
  HInteger row=offset*blocklen, col=0;
  while (it!=vec_end) {
    *it *= exp(phase_fraction*row*col);
    ++it; ++col;
    if (col == nblocks) {++row; col=0;};
  };
}

//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Returns a vector with phases to be multiplied (after exponentiation) with a transposed complex FFT matrix to calculate a second FFT with higher spectral resolution
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hDoubleFFTPhase
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_TYPES HFPP_REAL_NUMERIC_TYPES
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HComplex)(vec)()("Output vector containing the phases to be applied to a similar matrix with Fourier-transformed and transposed data that was read in with a certain blocklength, stride, and offset.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HInteger)(full_size)()("Full data size of the time series.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_2 (HInteger)(nblocks)()("How many blocks were read in.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_3 (HInteger)(blocklen)()("The block length for individual data blocks that were read in.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_4 (HInteger)(offset)()("How many blocks were skipped before the first block was read in (smaller or equal to stride).")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  See hDoubleFFT for a detailed description.
*/
template <class Iter>
void HFPP_FUNC_NAME (const Iter vec,const Iter vec_end, const HInteger full_size,  const HInteger nblocks, const HInteger blocklen, const HInteger offset)
{
  HInteger len=vec_end-vec;
  if (len!=(blocklen*nblocks)) {
    throw PyCR::ValueError("Input vector has not the dimension blocklen * nblocks.");
    return;
  };

  HComplex phase_fraction=HComplex(0.0,-CR::_2pi/full_size);
  Iter it=vec;
  HInteger row=offset*blocklen, col=0;
  while (it<vec_end) {
    *it = phase_fraction*row*col;
    ++it; ++col;
    if (col == nblocks) {
      ++row; col=0;
    };
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Performs two subsequent FFTs to obtain a spectrum with higher resolution, allowing one to operate only on a fraction of the data in memory
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hDoubleFFT
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_TYPES HFPP_REAL_NUMERIC_TYPES
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HComplex)(vecout)()("Complex output of spectrum (potentially containing gaps) - same size as input vector and must not be the same vector.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HComplex)(vecin)()("Complex input vector containing the Fourier-transformed and transposed data that was read in with a certain block length, stride, and offset. NOTE: the input vector will be overwritten and is used as a temporary scratch vector!")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HInteger)(full_size)()("Full data size of the time series.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_3 (HInteger)(nblocks)()("How many blocks were read in.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_4 (HInteger)(blocklen)()("The block length for individual data blocks that were read in.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_5 (HInteger)(offset)()("How many blocks were skipped before the first block was read in.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

Description of the data input vector:
-------------------------------------

This is how the data is to be provided and dealt with:

First, the time series data is read in as n=nblocks of length l=blocklength each, i.e. giving a vector:

full data -> [B1-1,B1-2,..,B1-l, --STRIDE-1 Blocks to be left out--, B2-1,B2-2,..,B2-l, --STRIDE-1 Blocks to be left out--, ...,  Bn-1,Bn-2,..,Bn-l]

-> [B1-1,B1-2,..,B1-l, B2-1,B2-2,..,B2-l,  ...,  Bn-1,Bn-2,..,Bn-l]

(B2-3 means: 2nd block, and 3rd sample within the block)

It is possible to leave out m=stride-1 blocks in between, so that only a fraction 1/(stride+1) of data is read in.

The next step is to transpose the data into a matrix, giving (B1-2 reads data block 1, sample number 2):

Mb=  [
     [B1-1,B2-1,..,Bn-1],
     [B1-2,B2-2,..,Bn-2],
             ...
     [B1-l,B2-l,..,Bn-l]
     ]

Hence this matrix is incomplete, since it is missing OFFSET times a
similar matrix on top (e.g., when one was starting to reading not from
the first block, but the 2nd, 3rd etc.) and STRIDE-1-OFFSET matrices at
the end.

For clarity: the full matrix would look have looked like (for the example of OFFSET=1 and STRIDE=3 above ... even tough stride is better a power of two!)

Mabc=[
     [A1-1,A2-1,..,An-1],       I
     [A1-2,A2-2,..,An-2],       I
             ...                I  cdataT
     [A1-l,A2-l,..,An-l],       I

     [B1-1,B2-1,..,Bn-1],
     [B1-2,B2-2,..,Bn-2],
             ...
     [B1-l,B2-l,..,Bn-l],

     [C1-1,C2-1,..,Cn-1],
     [C1-2,C2-2,..,Cn-2],
             ...
     [C1-l,C2-l,..,Cn-l]
     ]

The next step is now to take the FFT over rows above and mutliply by
the phase factor calculated in this function.  Finally, the data is
transposed a 2nd time.

Note: Here the function DoubleFFT1 ends (i.e. with the transpose of
the blocks above. This is done to allow for rearranging the blocks in
case they were written to disk. Afterwards DoubleFFT2 picks up again.

The matrix then looks like (if the blocks were merged, which is
essentially also a transpose...)

Mabc2=[
     [A1-1,A1-2,..,A1-l,B1-1,..,B1-l,C1-1,..,C1-l],     I   tmpspec
     [A2-1,A2-2,..,A2-l,B2-1,..,B2-l,C2-1,..,C2-l],     I
     ...
     now take only n/stride rows per memory chunk
     ...
     [An-1,An-2,..,An-l,Bn-1,..,Bn-l,Cn-1,..,Cn-l],
     ]

where one now needs to split the matrix again (after n/stride rows each) to
work with the same memory size.

In a second step (DoubleFFT2), the FFT is taken a second time (again
over what are then the rows above) and the result is tranposed a last time,
giving the final spectrum (with gaps, if the blocks are not properly rearranged again).

Mfinal=[
     [A1-1,A2-1,..,An-1],      (specT: only first nblock_section columns)
     [A1-2,A2-2,..,An-2],       specT2: only first blocklen rows and
             ...                        nblock_section columns
     [A1-l,A2-l,..,An-l],
     [B1-1,B2-1,..,Bn-1],
     [B1-2,B2-2,..,Bn-2],
             ...
     [B1-l,B2-l,..,Bn-l]]
     [C1-1,C2-1,..,Cn-1],
     [C1-2,C2-2,..,Cn-2],
             ...
     [C1-l,C2-l,..,Cn-l]
     ]


Example:

#Input parameters
ncolumns = 128
nrows = 1024  # = bigFFTblocksize / ncolumns
bigFFTblocksize = ncolumns*nrows

#Prepare some vectors
a=hArray(complex,[nrows, ncolumns])
a.random(-2.0,2.0)
b=hArray(complex,copy=a)
bT=b.transpose()

#Do only one big FFT
bigFFT=hArray(complex,[bigFFTblocksize])
bigFFT.fftw(a)

#Do two FFTs with the steps described above
aT=hArray(a).transpose()
aT[...].fftw(aT[...])
aT.doublefftphasemul(bigFFTblocksize,nrows,ncolumns,0)
a.transpose(aT)
a[...].fftw(a[...])
aT.transpose(a)

#Do the steps just above in one go
bT.doublefft(b,bigFFTblocksize,nrows,ncolumns,0)

-> bigFFT, aT, & bT all contain the same spectra

*/
template <class Iter>
void HFPP_FUNC_NAME (const Iter vecout,const Iter vecout_end, const Iter vecin,const Iter vecin_end, const HInteger full_size,  const HInteger nblocks, const HInteger blocklen, const HInteger offset)
{
  if ((vecin_end-vecin)!=(blocklen*nblocks) || (vecout_end-vecout)!=(blocklen*nblocks)) {
    throw PyCR::ValueError("Input vector has not the dimension blocklen * nblocks.");
    return;
  };
  //First transpose
  hTranspose(vecout,vecout_end,vecin,vecin_end,blocklen);
  //FFT over rows
  Iter it1(vecout), it2(vecout+nblocks);
  while (it2 <= vecout_end){
    hFFTw(it1,it2,it1,it2);
    it1=it2; it2+=nblocks;
  };
  //Multiply with phase factor
  hDoubleFFTPhaseMul(vecout,vecout_end,full_size,nblocks,blocklen,offset);
  //Second transpose
  hTranspose(vecin,vecin_end,vecout,vecout_end,nblocks);
  //And FFT over rows
  it1=vecin; it2=vecin+blocklen;
  while (it2 <= vecin_end){
    hFFTw(it1,it2,it1,it2);
    it1=it2; it2+=blocklen;
  };
  //Final transpose
  hTranspose(vecout,vecout_end,vecin,vecin_end,blocklen);
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Performs two subsequent FFTs to obtain a spectrum with higher resolution, allowing one to operate only on a fraction of the data in memory
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hDoubleFFT1
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_TYPES HFPP_REAL_NUMERIC_TYPES
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HComplex)(vecout)()("Complex output of spectrum (potentially containing gaps) - same size as input vector and must not be the same vector.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HComplex)(vecin)()("Complex input vector containing the Fourier-transformed and transposed data that was read in with a certain block length, stride, and offset. NOTE: the input vector will be overwritten and is used as a temporary scratch vector!")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HInteger)(full_size)()("Full data size of the time series.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_3 (HInteger)(nblocks)()("How many blocks were read in.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_4 (HInteger)(blocklen)()("The block length for individual data blocks that were read in.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_5 (HInteger)(offset)()("How many blocks were skipped before the first block was read in.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

See DoubleFFT for description!

NOTE: here is output is in the input vector "vecin" - vecout is just a scratch vector
*/
template <class Iter>
void HFPP_FUNC_NAME (const Iter vecout,const Iter vecout_end, const Iter vecin,const Iter vecin_end, const HInteger full_size,  const HInteger nblocks, const HInteger blocklen, const HInteger offset)
{
  if ((vecin_end-vecin)!=(blocklen*nblocks) || (vecout_end-vecout)!=(blocklen*nblocks)) {
    throw PyCR::ValueError("Input vector has not the dimension blocklen * nblocks.");
    return;
  };
  //First transpose
  hTranspose(vecout,vecout_end,vecin,vecin_end,blocklen);
  //FFT over rows
  Iter it1(vecout), it2(vecout+nblocks);
  while (it2 <= vecout_end){
    hFFTw(it1,it2,it1,it2);
    it1=it2; it2+=nblocks;
  };
  //Multiply with phase factor
  hDoubleFFTPhaseMul(vecout,vecout_end,full_size,nblocks,blocklen,offset);
  //2nd tranpsoe
  hTranspose(vecin,vecin_end,vecout,vecout_end,nblocks);

}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Performs two subsequent FFTs to obtain a spectrum with higher resolution, allowing one to operate only on a fraction of the data in memory
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hDoubleFFT2
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_TYPES HFPP_REAL_NUMERIC_TYPES
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HComplex)(vecout)()("Complex output of spectrum - same size as input vector and must not be the same vector.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HComplex)(vecin)()("Complex input vector obtianed from DoubleFFT1")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HInteger)(nblocks)()("How many blocks were read in.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_3 (HInteger)(blocklen)()("The block length for individual data blocks that were read in.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

See DoubleFFT for description!

    cdataT[offset].doublefft(cdata[offset],fullsize,nblocks,blocklen,offset)

    is the same as:

    cdataT[offset].doublefft1(cdata[offset],fullsize,nblocks,blocklen,offset)
    cdataT[offset].doublefft2(cdata[offset],nblocks,blocklen)

    but result is in cdata (and not cdataT)
*/
template <class Iter>
void HFPP_FUNC_NAME (const Iter vecout,const Iter vecout_end, const Iter vecin,const Iter vecin_end, const HInteger nblocks, const HInteger blocklen)
{
  if ((vecin_end-vecin)!=(blocklen*nblocks) || (vecout_end-vecout)!=(blocklen*nblocks)) {
    throw PyCR::ValueError("Input vector has not the dimension blocklen * nblocks.");
    return;
  };
  //FFT over rows -> 2nd transpose
  Iter iti1(vecin),  iti2(vecin+blocklen);
  while ((iti2 <= vecin_end)){
    hFFTw(iti1,iti2,iti1,iti2);
    iti1=iti2; iti2+=blocklen;
  };
  //Final transpose
  hTranspose(vecout,vecout_end,vecin,vecin_end,blocklen);
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

