/**************************************************************************
 *  IO module for the CR Pipeline Python bindings.                        *
 *                                                                        *
 *  Copyright (c) 2010                                                    *
 *                                                                        *
 *  Pim Schellart <p.schellart@astro.ru.nl>                               *
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
//$FILENAME: HFILE=mFFTW.def.h
//-----------------------------------------------------------------------

// ========================================================================
//
//  Header files
//
// ========================================================================

#include <sstream>

#include "core.h"
#include "mFFTW.h"


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

//__________________________________________________________________________
//                                                                  FFTWPlan

FFTWPlanManyDft::FFTWPlanManyDft (int N, int howmany, int istride, int idist, int ostride, int odist, enum fftw_sign sign, enum fftw_flags flags)
{
  // Store input and output size
  isize = N * howmany;
  osize = N * howmany;

  // Allocate space for arrays
  in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * isize);
  out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * osize);

  // Create FFTW plan
  p = fftw_plan_many_dft(1, &N, howmany, in, NULL, istride, idist, out, NULL, ostride, odist, sign, flags);
}
  
FFTWPlanManyDft::~FFTWPlanManyDft ()
{
  fftw_free(in);
  fftw_free(out);
  fftw_destroy_plan(p);
}

std::ostream& operator<<(std::ostream& output, const FFTWPlanManyDft& d)
{
    output << "FFTWPlanManyDft";

    return output;
}

FFTWPlanManyDftR2c::FFTWPlanManyDftR2c (int N, int howmany, int istride, int idist, int ostride, int odist, enum fftw_flags flags)
{
  // Store input and output size
  isize = N * howmany;
  osize = (N / 2 + 1) * howmany;

  // Allocate space for arrays
  in = (double*) fftw_malloc(sizeof(double) * isize);
  out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * osize);

  // Create FFTW plan
  p = fftw_plan_many_dft_r2c(1, &N, howmany, in, NULL, istride, idist, out, NULL, ostride, odist, flags);
}

FFTWPlanManyDftR2c::~FFTWPlanManyDftR2c ()
{
  fftw_free(in);
  fftw_free(out);
  fftw_destroy_plan(p);
}

std::ostream& operator<<(std::ostream& output, const FFTWPlanManyDftR2c& d)
{
    output << "FFTWPlanManyDftR2c";

    return output;
}

FFTWPlanManyDftC2r::FFTWPlanManyDftC2r (int N, int howmany, int istride, int idist, int ostride, int odist, enum fftw_flags flags)
{
  // Store input and output size
  isize = (N / 2 + 1) * howmany;
  osize = N * howmany;

  // Allocate space for arrays
  in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * isize);
  out = (double*) fftw_malloc(sizeof(double) * osize);

  // Create FFTW plan
  p = fftw_plan_many_dft_c2r(1, &N, howmany, in, NULL, istride, idist, out, NULL, ostride, odist, flags);
}

FFTWPlanManyDftC2r::~FFTWPlanManyDftC2r ()
{
  fftw_free(in);
  fftw_free(out);
  fftw_destroy_plan(p);
}

std::ostream& operator<<(std::ostream& output, const FFTWPlanManyDftC2r& d)
{
    output << "FFTWPlanManyDftC2r";

    return output;
}

//-----------------------------------------------------------------------
//$DOCSTRING: Executes an FFTW plan.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hFFTWExecutePlan
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_FUNC_MASTER_ARRAY_PARAMETER 0 // Use the first parameter as the master array for looping and history informations
#define HFPP_PARDEF_0 (HComplex)(out)()("Return vector in which the FFT transformed data is stored.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HComplex)(in)()("Input vector to make the FFT from.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (FFTWPlanManyDft)(plan)()("FFTW plan.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END ----------------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Example:
  >>> # Set up input/output arrays
  >>> x = cr.hArray(complex, N)
  >>> y = cr.hArray(complex, N)

  >>> # Create plan
  >>> p = cr.FFTWPlanManyDft(N, 1, 1, 1, 1, 1, cr.fftw_sign.FORWARD, cr.fftw_flags.ESTIMATE)

  >>> # Execute plan
  >>> cr.hFFTWExecutePlan(y, x, p)
*/
template <class CIter>
void HFPP_FUNC_NAME(const CIter out, const CIter out_end,
		    const CIter in,  const CIter in_end,
        FFTWPlanManyDft &plan)
{
  // Get array lengths
  const int Nin =  std::distance(in, in_end);
  const int Nout = std::distance(out, out_end);

  // Sanity check
  if (Nin != plan.isize || Nout != plan.osize)
  {
    throw PyCR::ValueError("In- and output vectors do not have the required size.");
    return;
  };

  // Copy data from input array
  memcpy(plan.in, &(*in), Nin * sizeof(fftw_complex));

  // Execute plan
  plan.execute();

  // Copy data to output array
  memcpy(&(*out), plan.out, Nout * sizeof(fftw_complex));
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//-----------------------------------------------------------------------
//$DOCSTRING: Executes an FFTW plan.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hFFTWExecutePlanRC
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_FUNC_MASTER_ARRAY_PARAMETER 0 // Use the first parameter as the master array for looping and history informations
#define HFPP_PARDEF_0 (HComplex)(out)()("Return vector in which the FFT transformed data is stored.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(in)()("Input vector to make the FFT from.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (FFTWPlanManyDft)(plan)()("FFTW plan.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END ----------------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Example:
  >>> # Set up input/output arrays
  >>> x = cr.hArray(complex, N)
  >>> y = cr.hArray(complex, N)

  >>> # Create plan
  >>> p = cr.FFTWPlanManyDft(N, 1, 1, 1, 1, 1, cr.fftw_sign.FORWARD, cr.fftw_flags.ESTIMATE)

  >>> # Execute plan
  >>> cr.hFFTWExecutePlan(y, x, p)
*/
template <class CIter, class NIter>
void HFPP_FUNC_NAME(const CIter out, const CIter out_end,
		    const NIter in,  const NIter in_end,
        FFTWPlanManyDft &plan)
{
  // Get array lengths
  const int Nin =  std::distance(in, in_end);
  const int Nout = std::distance(out, out_end);

  // Sanity check
  if (Nin != plan.isize || Nout != plan.osize)
  {
    throw PyCR::ValueError("In- and output vectors do not have the required size.");
    return;
  };

  // Copy data from input array
  NIter in_it = in;

  for (int i=0; i<Nin; i++)
  {
    plan.in[i][0] = *in_it++;
    plan.in[i][1] = 0;
  }

  // Execute plan
  plan.execute();

  // Copy data to output array
  memcpy(&(*out), plan.out, Nout * sizeof(fftw_complex));
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//-----------------------------------------------------------------------
//$DOCSTRING: Executes an FFTW plan.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hFFTWExecutePlanCR
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_FUNC_MASTER_ARRAY_PARAMETER 0 // Use the first parameter as the master array for looping and history informations
#define HFPP_PARDEF_0 (HNumber)(out)()("Return vector in which the FFT transformed data is stored.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HComplex)(in)()("Input vector to make the FFT from.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (FFTWPlanManyDft)(plan)()("FFTW plan.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END ----------------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Example:
  >>> # Set up input/output arrays
  >>> x = cr.hArray(complex, N)
  >>> y = cr.hArray(complex, N)

  >>> # Create plan
  >>> p = cr.FFTWPlanManyDft(N, 1, 1, 1, 1, 1, cr.fftw_sign.FORWARD, cr.fftw_flags.ESTIMATE)

  >>> # Execute plan
  >>> cr.hFFTWExecutePlan(y, x, p)
*/
template <class CIter, class NIter>
void HFPP_FUNC_NAME(const NIter out, const NIter out_end,
		    const CIter in,  const CIter in_end,
        FFTWPlanManyDft &plan)
{
  // Get array lengths
  const int Nin =  std::distance(in, in_end);
  const int Nout = std::distance(out, out_end);

  // Sanity check
  if (Nin != plan.isize || Nout != plan.osize)
  {
    throw PyCR::ValueError("In- and output vectors do not have the required size.");
    return;
  };

  // Copy data from input array
  memcpy(plan.in, &(*in), Nin * sizeof(fftw_complex));

  // Execute plan
  plan.execute();

  // Copy data from output array
  NIter out_it = out;

  for (int i=0; i<Nout; i++)
  {
    *out_it = plan.out[i][0];
    out_it++;
  }
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//-----------------------------------------------------------------------
//$DOCSTRING: Executes an FFTW plan.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hFFTWExecutePlan
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_FUNC_MASTER_ARRAY_PARAMETER 0 // Use the first parameter as the master array for looping and history informations
#define HFPP_PARDEF_0 (HComplex)(out)()("Return vector in which the FFT transformed data is stored.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(in)()("Input vector to make the FFT from.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (FFTWPlanManyDftR2c)(plan)()("FFTW plan.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END ----------------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Example:
  >>> # Set up input/output arrays
  >>> x = cr.hArray(float, N)
  >>> y = cr.hArray(complex, N/2+1)

  >>> # Create plan
  >>> p = cr.FFTWPlanManyDftR2c(N, 1, 1, 1, 1, 1, cr.fftw_flags.ESTIMATE)

  >>> # Execute plan
  >>> cr.hFFTWExecutePlan(y, x, p)
*/
template <class CIter, class Iter>
void HFPP_FUNC_NAME(const CIter out, const CIter out_end,
		    const Iter in,  const Iter in_end,
        FFTWPlanManyDftR2c &plan)
{
  // Get array lengths
  const int Nin =  std::distance(in, in_end);
  const int Nout = std::distance(out, out_end);

  // Sanity check
  if (Nin != plan.isize || Nout != plan.osize)
  {
    throw PyCR::ValueError("In- and output vectors do not have the required size.");
    return;
  };

  // Copy data from input array
  memcpy(plan.in, &(*in), Nin * sizeof(double));

  // Execute plan
  plan.execute();

  // Copy data to output array
  memcpy(&(*out), plan.out, Nout * sizeof(fftw_complex));
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//-----------------------------------------------------------------------
//$DOCSTRING: Executes an FFTW plan.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hFFTWExecutePlan
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_FUNC_MASTER_ARRAY_PARAMETER 0 // Use the first parameter as the master array for looping and history informations
#define HFPP_PARDEF_0 (HNumber)(out)()("Return vector in which the FFT transformed data is stored.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HComplex)(in)()("Input vector to make the FFT from.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (FFTWPlanManyDftC2r)(plan)()("FFTW plan.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END ----------------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Example:
  >>> # Set up input/output arrays
  >>> x = cr.hArray(complex, N/2+1)
  >>> y = cr.hArray(float, N)

  >>> # Create plan
  >>> p = cr.FFTWPlanManyDftC2r(N, 1, 1, 1, 1, 1, cr.fftw_flags.ESTIMATE)

  >>> # Execute plan
  >>> cr.hFFTWExecutePlan(y, x, p)
*/
template <class Iter, class CIter>
void HFPP_FUNC_NAME(const Iter out, const Iter out_end,
		    const CIter in,  const CIter in_end,
        FFTWPlanManyDftC2r &plan)
{
  // Get array lengths
  const int Nin =  std::distance(in, in_end);
  const int Nout = std::distance(out, out_end);

  // Sanity check
  if (Nin != plan.isize || Nout != plan.osize)
  {
    throw PyCR::ValueError("In- and output vectors do not have the required size.");
    return;
  };

  // Copy data from input array
  memcpy(plan.in, &(*in), Nin * sizeof(fftw_complex));

  // Execute plan
  plan.execute();

  // Copy data to output array
  memcpy(&(*out), plan.out, Nout * sizeof(double));
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//-----------------------------------------------------------------------
//$DOCSTRING: Convert between CASA and FFTW definitions for FFT
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hFFTConvert
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_FUNC_MASTER_ARRAY_PARAMETER 0 // Use the first parameter as the master array for looping and history informations
#define HFPP_PARDEF_0 (HComplex)(vec)()("")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END ----------------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

*/
template <class CIter>
void HFPP_FUNC_NAME(const CIter vec, const CIter vec_end)
{
  CIter it_vec = vec;

  int sign = -1;
  while (it_vec != vec_end)
  {
    *it_vec *= sign;
    sign = -(sign);
    ++it_vec;
  }
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//-----------------------------------------------------------------------
//$DOCSTRING: Resample input vector to output vector length using FFTW
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hFFTWResample
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_FUNC_MASTER_ARRAY_PARAMETER 0 // Use the first parameter as the master array for looping and history informations
#define HFPP_PARDEF_0 (HNumber)(out)()("Output vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(in)()("Input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END ----------------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Note that using the FFT to resample assumes the signal to be periodic.
*/
template <class NIter>
void HFPP_FUNC_NAME(const NIter out, const NIter out_end, const NIter in, const NIter in_end)
{
  const int Nin = std::distance(in, in_end);
  const int Nout = std::distance(out, out_end);
  const int buffer_size = Nin > Nout ? Nin : Nout;

  // allocate 
  fftw_complex * tmp_fd = (fftw_complex*)fftw_malloc((Nout/2+1)*sizeof(fftw_complex));
  double * buffer = (double*)fftw_malloc(buffer_size*sizeof(double));
  
  // create fftw plans
  fftw_plan fft_plan = fftw_plan_dft_r2c_1d(Nin, buffer, tmp_fd, FFTW_ESTIMATE);
  fftw_plan ifft_plan = fftw_plan_dft_c2r_1d(Nout, tmp_fd, buffer, FFTW_ESTIMATE);
  
  // zero out tmp_fd
  memset(tmp_fd, 0, (Nout/2+1)*sizeof(fftw_complex));
  
  // copy data from input array
  memcpy(buffer, &(*in), Nin * sizeof(double));

  // execute the plans (forward then reverse)
  fftw_execute_dft_r2c(fft_plan, buffer, tmp_fd);
  fftw_execute_dft_c2r(ifft_plan, tmp_fd, buffer);
  
  // Copy data to output array
  memcpy(&(*out), buffer, Nout * sizeof(double));

  // Compensate for normalization
  NIter out_it = out;
  while (out_it != out_end)
  {
    *out_it = *out_it / Nin;
    out_it++;
  }

  // cleanup
  fftw_free(tmp_fd);
  fftw_free(buffer);
  fftw_destroy_plan(fft_plan);
  fftw_destroy_plan(ifft_plan);
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//-----------------------------------------------------------------------
//$DOCSTRING: Resample input vector to output vector length using FFTW
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hFFTWDownsample
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_FUNC_MASTER_ARRAY_PARAMETER 0 // Use the first parameter as the master array for looping and history informations
#define HFPP_PARDEF_0 (HNumber)(out)()("Output vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(in)()("Input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END ----------------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Note that using the FFT to resample assumes the signal to be periodic.
*/
template <class NIter>
void HFPP_FUNC_NAME(const NIter out, const NIter out_end, const NIter in, const NIter in_end)
{
  const int Nin = std::distance(in, in_end);
  const int Nout = std::distance(out, out_end);
  const int buffer_size = Nin;

  // allocate 
  fftw_complex * tmp_fd = (fftw_complex*)fftw_malloc((Nin/2+1)*sizeof(fftw_complex));
  double * buffer = (double*)fftw_malloc(buffer_size*sizeof(double));
  
  // create fftw plans
  fftw_plan fft_plan = fftw_plan_dft_r2c_1d(Nin, buffer, tmp_fd, FFTW_ESTIMATE);
  fftw_plan ifft_plan = fftw_plan_dft_c2r_1d(Nout, tmp_fd, buffer, FFTW_ESTIMATE);
  
  // zero out tmp_fd
  memset(tmp_fd, 0, (Nout/2+1)*sizeof(fftw_complex));
  
  // copy data from input array
  memcpy(buffer, &(*in), Nin * sizeof(double));

  // execute the plans (forward then reverse)
  fftw_execute_dft_r2c(fft_plan, buffer, tmp_fd);
  
  memset(tmp_fd+(Nout/2 + 1), 0, (Nin - (Nout/2+1))*sizeof(fftw_complex));
  
  fftw_execute_dft_c2r(ifft_plan, tmp_fd, buffer);
  
  // Copy data to output array
  memcpy(&(*out), buffer, Nout * sizeof(double));

  // Compensate for normalization
  NIter out_it = out;
  while (out_it != out_end)
  {
    *out_it = *out_it / Nin;
    out_it++;
  }

  // cleanup
  fftw_destroy_plan(fft_plan);
  fftw_destroy_plan(ifft_plan);
  fftw_free(tmp_fd);
  fftw_free(buffer);
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

