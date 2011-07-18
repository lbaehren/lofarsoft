/**************************************************************************
 *  Fitting module for the CR Pipeline Python bindings.                   *
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
//$FILENAME: HFILE=mFitting.def.h
//-----------------------------------------------------------------------

// ========================================================================
//
//  Header files
//
// ========================================================================

#include "core.h"
#include "mMath.h"
#include "mFitting.h"

#ifdef PYCRTOOLS_WITH_GSL
#include <gsl/gsl_multifit.h>
#include <gsl/gsl_bspline.h>
#include <gsl/gsl_version.h>
#endif /* PYCRTOOLS_WITH_GSL */


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

//!initialize functions of the library
void hInitFitting(){
#ifdef PYCRTOOLS_WITH_GSL
  gsl_set_error_handler_off ();
#else
  ERROR("hInit: GSL-Libraries not installed. Some functions are not defined.")
#endif /* PYCRTOOLS_WITH_GSL */
}

// ================================================================================
//$SECTION: GSL Fitting
// ================================================================================

//$COPY_TO HFILE: #ifdef PYCRTOOLS_WITH_GSL
//!Makes a shared memory GSL Matrix. Note that m->block needs to be deleted explicitly afterwards ...
template <class Iter>
gsl_matrix * STL2GSLM(const Iter vec, const HInteger dim1, const HInteger dim2) {
  gsl_matrix * m = new gsl_matrix;
  m->size1 = dim1;
  m->size2 = dim2;
  m->tda = dim2;
  m->data = (double*) &(*vec);
  m->block = NULL;
  //  m->block = new gsl_block;
  //  m->block->data = m->data;
  //  m->block->size=dim1*dim2;
  m->owner = 0;
  return m;
}

template <class Iter>
gsl_vector * STL2GSL(const Iter vec, const Iter vec_end) {
  gsl_vector * m = new gsl_vector;
  m->size = vec_end-vec;
  m->stride = 1;
  m->data = (double*) &(*vec);
  m->block = NULL;
  /*  m->block = new gsl_block;
  m->block->data = m->data;
  m->block->size=m->size;
  */
  m->owner = 0; //owner=0 Means block of memory is not owned by the GSL vector and hence not freed.
  return m;
}

//$DOCSTRING: Calculate the :math:`x`-value vector for the polynomial (linear-fitting) routine, calculating essentially all powers of the input vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hLinearFitPolynomialX
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HNumber)(vecout)()("Output vector containing rows with all the requested powers of the :math:`x`-vector. Size is ``(n+1)*len(xvec)``, where ``n=len(powers)``.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(xvec)()("Input vector with :math:`x`-values for the fitting routine.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HInteger)(powers)()("Input vector with the powers to consider in the fitting. For an nth order polynomial, this is simply ``[0,1,2,...,n]``.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END ----------------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Description:
  ``n`` is the number of powers of the polynomial to fit,
  i.e. ``len(powers)``. The length of the output vector is the length
  of ``xvec*(n+1)``.

  See also:
  hLinearFit

  Example:
  [x_0,x_1,x_2,...] -> [0,x_0,x_0**2,...,x_0**n,0,x_1,x_1**2,...,x_1**n]
*/
template <class Iter, class IterI>
void HFPP_FUNC_NAME(const Iter vecout, const Iter vecout_end,
		    const Iter xvec, const Iter xvec_end,
		    const IterI powers, const IterI powers_end
		       )
{
  // Decaration of variables
  Iter itout(vecout), itx(xvec);
  IterI itn(powers);
  HInteger lenOut = vecout_end - vecout;
  HInteger lenX   = xvec_end - xvec;;
  HInteger lenPow = powers_end - powers;

  // Sanity check
  if (lenOut <= 0) {
    throw PyCR::ValueError("Illegal size of output vector.");
    return;
  }
  if (lenX <= 0) {
    throw PyCR::ValueError("Illegal size of x vector.");
    return;
  }
  if (lenPow <= 0) {
    throw PyCR::ValueError("Illegal size of power vector.");
    return;
  }

  // Implementation
  while ((itout != vecout_end) && (itx != xvec_end)) {
    switch (*itn) {
    case 0:
      *itout=1.0;
      break;
    case 1:
      *itout=*itx;
      break;
    case 2:
      *itout=(*itx) * (*itx);
      break;
    default:
      *itout=pow(*itx,(int)*itn);
      break;
    };
    ++itout; ++itn;
    if (itn==powers_end) { //We have cycled through all powers, start at the beginning and go to next x value
      itn=powers;
      ++itx;
    };
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//------------------------------------------------------------------------
// Begin k order spline
//------------------------------------------------------------------------

//$DOCSTRING: Calculate the :math:`x`-value vector for the basis Spline (BSpline) fitting routine, calculating essentially all powers of the input vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hBSplineFitXValues
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HNumber)(vecout)()("Output vector containing rows with all the requested powers of the :math:`x`-vector. Size is ``Ncoeffs * len(xvec)``.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(xvec)()("Input vector with :math:`x`-values for the fitting routine.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HInteger)(Ncoeffs)()("Number of coefficients to calculate.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_3 (HNumber)(xmin)()("Lower end of the :math:`x`-range for calculation of break points.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_4 (HNumber)(xmax)()("Upper end of the :math:`x`-range for calculation of break points.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_5 (HInteger)(k)()("Order of polynomial to fit locally, e.g. ``k=4`` is a 3rd order polynomial.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END ----------------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Description:
  Note: for ``k=2`` you get a linear fit.

  See also:
  hBSplineFit, hBSplineCalc, hBSpline, hBSplineFitXValues, hBSplineCalcAdd,  hBSplineCalcMul
*/
template <class Iter>
void HFPP_FUNC_NAME(
		    const Iter vecout, const Iter vecout_end,
		    const Iter xvec, const Iter xvec_end,
		    const HInteger Ncoeffs,
		    const HNumber xmin,
		    const HNumber xmax,
            const HInteger k
		       )
{
  Iter itout(vecout), itout_end(vecout_end-Ncoeffs+1), itx(xvec);
  gsl_bspline_workspace *bw=gsl_bspline_alloc(k, Ncoeffs+2-k);
  gsl_bspline_knots_uniform(xmin, xmax, bw);
  gsl_vector *B = STL2GSL(itout, itout+Ncoeffs);
  HInteger lenOut = vecout_end - vecout;
  HInteger lenX   = xvec_end - xvec;

  // Sanity check
  if (lenOut <= 0) {
    throw PyCR::ValueError("Illegal size of output vector.");
    return;
  }
  if (k <= 0) {
    throw PyCR::ValueError("Spline: k order must be largrer then zero.");
    return;
  }
  if (lenX <= 0) {
    throw PyCR::ValueError("Illegal size of x vector.");
    return;
  }

  // Implementation
  while ((itout < itout_end) && (itx != xvec_end)) {
    if ((*itx>=xmin) && (*itx<=xmax)) gsl_bspline_eval(*itx, B, bw);
    itout+=Ncoeffs; ++itx; B->data=&(*itout);
  };
  gsl_bspline_free(bw);
  delete B;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Calculate the :math:`y`-value vector of the results of a basis Spline fit, providing the powers of :math:`x` as input.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hBSpline
//-----------------------------------------------------------------------
//#define HFPP_WRAPPER_CLASSES HFPP_CLASS_STL HFPP_CLASS_hARRAY HFPP_CLASS_hARRAYALL
#define HFPP_FUNCDEF (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HNumber)(vecout)()("Output vector containing the :math:`y`-values for the input vector :math:`x`-values.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(xvec)()("Input vector of ``Ndata*Ncoeffs`` :math:`x`-values produced with :func:`hBSplineFitXValues`. Size is ``Ncoeffs*len(xvec)``.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HNumber)(coeffs)()("Input vector with the coefficients calculated by the fitting routine.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_3 (HInteger)(k)()("Order of polynomial to fit locally, e.g. ``k=4`` is a 3rd order polynomial.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END ----------------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  See also:
  hBSplineFit, hBSplineCalc, hBSpline, hBSplineFitXValues, hBSplineCalcAdd,  hBSplineCalcMul
*/
template <class Iter>
void HFPP_FUNC_NAME(
		    const Iter vecout, const Iter vecout_end,
		    const Iter xvec, const Iter xvec_end,
		    const Iter coeffs, const Iter coeffs_end,
            const HInteger k
		    )
{
  // Declaration of variables
  HInteger Ncoeffs(coeffs_end-coeffs);      /* nbreak = ncoeffs + 2 - k = ncoeffs - 2 since k = 4 */
  Iter itout(vecout), itx(xvec), itx_end(itx+Ncoeffs);
  Iter coeffs2(coeffs),  coeffs_end2(coeffs_end);
  HInteger lenOut    = vecout_end - vecout;
  HInteger lenX      = xvec_end - xvec;
  HInteger lenCoeffs = coeffs_end - coeffs;

  // Sanity check
  if (lenOut < 0) {
    throw PyCR::ValueError("Illegal size of output vector.");
    return;
  }
  if (lenX < 0) {
    throw PyCR::ValueError("Illegal size of x vector.");
    return;
  }
  if (lenCoeffs <= 0) {
    throw PyCR::ValueError("Illegal size of coefficients vector.");
    return;
  }
  if (k <= 0) {
    throw PyCR::ValueError("Spline: k order must be largrer then zero.");
    return;
  }

  // Implementation
  while ((itout < vecout_end) && (itx_end < xvec_end)) {
    *itout=hMulSum(itx,itx_end,coeffs2,coeffs_end2);
    ++itout; itx=itx_end; itx_end+=Ncoeffs;
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//////////////////////COPIED FROM GSL 1.14//////////////////
////////////////////////////////////////////////////////////
static inline size_t
bspline_find_interval (const double x, int *flag, gsl_bspline_workspace * w)
{
  size_t i;

  if (x < gsl_vector_get (w->knots, 0))
    {
      *flag = -1;
      return 0;
    }

  /* find i such that t_i <= x < t_{i+1} */
  for (i = w->k - 1; i < w->k + w->l - 1; i++)
    {
      const double ti = gsl_vector_get (w->knots, i);
      const double tip1 = gsl_vector_get (w->knots, i + 1);

      if (tip1 < ti)
	{
	  GSL_ERROR ("knots vector is not increasing", GSL_EINVAL);
	}

      if (ti <= x && x < tip1)
	break;

      if (ti < x && x == tip1 && tip1 == gsl_vector_get (w->knots, w->k + w->l
							 - 1))
	break;
    }

  if (i == w->k + w->l - 1)
    *flag = 1;
  else
    *flag = 0;

  return i;
}				/* bspline_find_interval() */

/*
bspline_process_interval_for_eval()
  Consumes an x location, left knot from bspline_find_interval, flag
from bspline_find_interval, and a workspace.  Checks that x lies within
the splines' knots, enforces some endpoint continuity requirements, and
avoids divide by zero errors in the underlying bspline_pppack_* functions.
*/
static inline int
bspline_process_interval_for_eval (const double x, size_t * i, const int flag,
				   gsl_bspline_workspace * w)
{
  if (flag == -1)
    {
      GSL_ERROR ("x outside of knot interval", GSL_EINVAL);
    }
  else if (flag == 1)
    {
      if (x <= gsl_vector_get (w->knots, *i) + GSL_DBL_EPSILON)
	{
	  *i -= 1;
	}
      else
	{
	  GSL_ERROR ("x outside of knot interval", GSL_EINVAL);
	}
    }

  if (gsl_vector_get (w->knots, *i) == gsl_vector_get (w->knots, *i + 1))
    {
      GSL_ERROR ("knot(i) = knot(i+1) will result in division by zero",
		 GSL_EINVAL);
    }

  return GSL_SUCCESS;
}				/* bspline_process_interval_for_eval */

/********************************************************************
 * PPPACK ROUTINES
 *
 * The routines herein deliberately avoid using the bspline workspace,
 * choosing instead to pass all work areas explicitly.  This allows
 * others to more easily adapt these routines to low memory or
 * parallel scenarios.
 ********************************************************************/

/*
bspline_pppack_bsplvb()
  calculates the value of all possibly nonzero b-splines at x of order
jout = max( jhigh , (j+1)*(index-1) ) with knot sequence t.

Parameters:
   t      - knot sequence, of length left + jout , assumed to be
            nondecreasing.  assumption t(left).lt.t(left + 1).
            division by zero  will result if t(left) = t(left+1)
   jhigh  -
   index  - integers which determine the order jout = max(jhigh,
            (j+1)*(index-1))  of the b-splines whose values at x
            are to be returned.  index  is used to avoid
            recalculations when several columns of the triangular
            array of b-spline values are needed (e.g., in  bsplpp
            or in  bsplvd ).  precisely,

            if  index = 1 ,
               the calculation starts from scratch and the entire
               triangular array of b-spline values of orders
               1,2,...,jhigh  is generated order by order , i.e.,
               column by column .

            if  index = 2 ,
               only the b-spline values of order j+1, j+2, ..., jout
               are generated, the assumption being that biatx, j,
               deltal, deltar are, on entry, as they were on exit
               at the previous call.

            in particular, if jhigh = 0, then jout = j+1, i.e.,
            just the next column of b-spline values is generated.
   x      - the point at which the b-splines are to be evaluated.
   left   - an integer chosen (usually) so that
            t(left) .le. x .le. t(left+1).
   j      - (output) a working scalar for indexing
   deltal - (output) a working area which must be of length at least jout
   deltar - (output) a working area which must be of length at least jout
   biatx  - (output) array of length jout, with  biatx(i)
            containing the value at  x  of the polynomial of order
            jout which agrees with the b-spline b(left-jout+i,jout,t)
            on the interval (t(left), t(left+1)) .

Method:
   the recurrence relation

                      x - t(i)              t(i+j+1) - x
      b(i,j+1)(x) = -----------b(i,j)(x) + ---------------b(i+1,j)(x)
                    t(i+j)-t(i)            t(i+j+1)-t(i+1)

   is used (repeatedly) to generate the (j+1)-vector  b(left-j,j+1)(x),
   ...,b(left,j+1)(x)  from the j-vector  b(left-j+1,j)(x),...,
   b(left,j)(x), storing the new values in  biatx  over the old. the
   facts that

      b(i,1) = 1  if  t(i) .le. x .lt. t(i+1)

   and that

      b(i,j)(x) = 0  unless  t(i) .le. x .lt. t(i+j)

   are used. the particular organization of the calculations follows
   algorithm (8) in chapter x of [1].

Notes:

   (1) This is a direct translation of PPPACK's bsplvb routine with
       j, deltal, deltar rewritten as input parameters and
       utilizing zero-based indexing.

   (2) This routine contains no error checking.  Please use routines
       like gsl_bspline_eval().
*/

static void
bspline_pppack_bsplvb (const gsl_vector * t,
		       const size_t jhigh,
		       const size_t index,
		       const double x,
		       const size_t left,
		       size_t * j,
		       gsl_vector * deltal,
		       gsl_vector * deltar, gsl_vector * biatx)
{
  size_t i;			/* looping */
  double saved;
  double term;

  if (index == 1)
    {
      *j = 0;
      gsl_vector_set (biatx, 0, 1.0);
    }

  for ( /* NOP */ ; *j < jhigh - 1; *j += 1)
    {
      gsl_vector_set (deltar, *j, gsl_vector_get (t, left + *j + 1) - x);
      gsl_vector_set (deltal, *j, x - gsl_vector_get (t, left - *j));

      saved = 0.0;

      for (i = 0; i <= *j; i++)
	{
	  term = gsl_vector_get (biatx, i) / (gsl_vector_get (deltar, i)
					      + gsl_vector_get (deltal,
								*j - i));

	  gsl_vector_set (biatx, i,
			  saved + gsl_vector_get (deltar, i) * term);

	  saved = gsl_vector_get (deltal, *j - i) * term;
	}

      gsl_vector_set (biatx, *j + 1, saved);
    }

  return;
}				/* gsl_bspline_pppack_bsplvb */

int
gsl_bspline_eval_nonzero (const double x, gsl_vector * Bk, size_t * istart,
			  size_t * iend, gsl_bspline_workspace * w)
{
  if (Bk->size != w->k)
    {
      GSL_ERROR ("Bk vector length does not match order k", GSL_EBADLEN);
    }
  else
    {
      size_t i;			/* spline index */
      size_t j;			/* looping */
      int flag = 0;		/* interval search flag */
      int error = 0;		/* error flag */

      i = bspline_find_interval (x, &flag, w);
      error = bspline_process_interval_for_eval (x, &i, flag, w);
      if (error)
	{
	  return error;
	}

      *istart = i - w->k + 1;
      *iend = i;

      bspline_pppack_bsplvb (w->knots, w->k, 1, x, *iend, &j, w->deltal,
			     w->deltar, Bk);

      return GSL_SUCCESS;
    }
}				/* gsl_bspline_eval_nonzero() */

/*
gsl_bspline_deriv_eval()
  Evaluate d^j/dx^j B_i(x) for all i, 0 <= j <= nderiv.
This is a wrapper function for gsl_bspline_deriv_eval_nonzero()
which formats the output in a nice way.

Inputs: x      - point for evaluation
        nderiv - number of derivatives to compute, inclusive.
        dB     - (output) where to store d^j/dx^j B_i(x)
                 values. the size of this matrix is
                 (n = nbreak + k - 2 = l + k - 1 = w->n)
                 by (nderiv + 1)
        w      - bspline derivative workspace

Return: success or error

Notes: 1) The w->knots vector must be initialized prior to calling
          this function (see gsl_bspline_knots())

       2) based on PPPACK's bsplvd
*/
//////////////////////END COPIED FROM GSL 1.14//////////////////
////////////////////////////////////////////////////////////////


//========================================================================
//$ITERATE MFUNC Mul,Add,Assign
//========================================================================

//$DOCSTRING: Calculate the :math:`y`-values of the results of a basis Spline fit and $MFUNC to the output vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hBSplineCalc{$MFUNC}
//-----------------------------------------------------------------------
//#define HFPP_WRAPPER_CLASSES HFPP_CLASS_STL HFPP_CLASS_hARRAY HFPP_CLASS_hARRAYALL
#define HFPP_FUNCDEF (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HNumber)(vecout)()("Output vector containing the :math:`y`-values for the input vector :math:`x`-values.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(xvec)()("Input vector with :math:`x`-values.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HNumber)(coeffs)()("Input vector with the number of coefficients calculated by the fitting routine.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_3 (HNumber)(xmin)()("Lower limit of interval where spline is defined.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_4 (HNumber)(xmax)()("Upper limit of interval where spline is defined.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_5 (HInteger)(k)()("Order of polynomial to fit locally, e.g. ``k=4`` is 3rd order polynomial.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END ----------------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  See also:
  hBSplineFit, hBSpline, hBSplineFitXValues, hBSplineCalc,  hBSplineCalcAdd,  hBSplineCalcMul
*/
template <class Iter>
void HFPP_FUNC_NAME(
		    const Iter vecout, const Iter vecout_end,
		    const Iter xvec, const Iter xvec_end,
		    const Iter coeffs, const Iter coeffs_end,
            const HNumber xmin,
            const HNumber xmax,
            const HInteger k
		    )
{
  // Declaration of variables
  HInteger Ncoeffs(coeffs_end-coeffs);      /* nbreak = ncoeffs + 2 - k = ncoeffs - 2 since k = 4 */
  Iter itout(vecout), itx(xvec);

  if (Ncoeffs <= 0) {
    throw PyCR::ValueError("Illegal size of coefficients vector.");
    return;
  }
  if (k <= 0) {
    throw PyCR::ValueError("Spline: k order must be larger then zero.");
    return;
  }

  gsl_bspline_workspace *bw=gsl_bspline_alloc(k, Ncoeffs+2-k);
  gsl_bspline_knots_uniform(xmin, xmax, bw);
  //  gsl_vector* B = gsl_vector_alloc (Ncoeffs);
  gsl_vector* B = gsl_vector_alloc (k);

  //  Iter Bstart(B->data),Bend((B->data)+Ncoeffs);
  Iter Bstart(B->data),Bend((B->data)+k);
  size_t istart,iend;

  // Implementation
  while ((itout < vecout_end) && (itx < xvec_end)) {
    //    if ((*itx>=xmin) && (*itx<=xmax)) gsl_bspline_eval(*itx, B, bw);
    //    *itout HFPP_OPERATOR_INPLACE_$MFUNC hMulSum(Bstart,Bend,coeffs,coeffs_end);
    if ((*itx>=xmin) && (*itx<=xmax)) {
      gsl_bspline_eval_nonzero(*itx, B, &istart, &iend, bw);
      if (((HInteger)istart<Ncoeffs) && ((HInteger)iend < Ncoeffs))
        ++iend;
        *itout HFPP_OPERATOR_INPLACE_$MFUNC hMulSum(Bstart,Bend,coeffs+istart,coeffs+iend);
    };
    ++itx;++itout;
  };
  gsl_bspline_free(bw);
  gsl_vector_free (B);
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$ENDITERATE


//$DOCSTRING: Calculate the :math:`y`-values of the results of a basis Spline fit and assign to the output vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hBSplineCalc
//-----------------------------------------------------------------------
//#define HFPP_WRAPPER_CLASSES HFPP_CLASS_STL HFPP_CLASS_hARRAY HFPP_CLASS_hARRAYALL
#define HFPP_FUNCDEF (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HNumber)(vecout)()("Output vector containing the :math:`y`-values for the input vector :math:`x`-values.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(xvec)()("Input vector with :math:`x`-values.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HNumber)(coeffs)()("Input vector with the number of coefficients calculated by the fitting routine.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_3 (HNumber)(xmin)()("Lower limit of interval where spline is defined.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_4 (HNumber)(xmax)()("Upper limit of interval where spline is defined.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_5 (HInteger)(k)()("Order of polynomial to fit locally, e.g. ``k=4`` is a 3rd order polynomial.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END ----------------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  See also:
  hBSplineFit, hBSpline, hBSplineFitXValues, hBSplineCalc,  hBSplineCalcAdd,  hBSplineCalcMul
*/
template <class Iter>
void HFPP_FUNC_NAME(const Iter vecout, const Iter vecout_end,
		    const Iter xvec, const Iter xvec_end,
		    const Iter coeffs, const Iter coeffs_end,
                    const HNumber xmin,
                    const HNumber xmax,
                    const HInteger k
		    )
{
  hBSplineCalcAssign(vecout, vecout_end,
                     xvec, xvec_end,
                     coeffs, coeffs_end,
                     xmin, xmax,
                     k);
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Do a basis spline fit to a data set (without weights) and return the coefficients and covariance matrix in two vectors.
// Returns as function value a pointer to the bspline workspace that is needed by other functions.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hBSplineFit
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF (HInteger)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HNumber)(vecout)()("Output vector containing the bets fit coefficients :math:`C_n` of the polynomial :math:`\\sum_n C_n x^n`. The first element is :math:`C_0`, i.e. the constant. Its length determines the order of the polynomial.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(veccov)()("Output vector of length :math:`n^2` containing the covariance matrix of the fit.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HNumber)(xmatrix)()("Output vector containing the :math:`x`-values of the data, where each value is actually one row of ``ncoeff`` values some power of :math:`x`. Calculated with :func:`hBSplineFitXValues`.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_3 (HNumber)(xvec)()("Vector containing the :math:`x` values of the data.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_4 (HNumber)(yvec)()("Vector containing the :math:`y` values of the data.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_5 (HNumber)(xmin)()("Lower limit of interval where spline is defined.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_6 (HNumber)(xmax)()("Upper limit of interval where spline is defined.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_7 (HInteger)(k)()("Order of polynomial to fit locally, e.g. ``k=4`` is a 3rd order polynomial.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END ----------------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Description:
  Returns the :math:`\\chi^2` value of the fit.

  See also:
  hBSplineFit, hBSpline, hBSplineFitXValues, hBSplineCalcAssign,  hBSplineCalcAdd,  hBSplineCalcMul
*/
template <class Iter>
HNumber HFPP_FUNC_NAME(
		    const Iter vecout, const Iter vecout_end,
		    const Iter veccov, const Iter veccov_end,
		    const Iter xmatrix, const Iter xmatrix_end,
		    const Iter xvec, const Iter xvec_end,
		    const Iter yvec, const Iter yvec_end,
            const HNumber xmin,
            const HNumber xmax,
            const HInteger k
		    )
{
  HInteger Ndata(yvec_end-yvec);  // Number of data points to fit
  HInteger Ncoeffs(vecout_end-vecout);
  HInteger lenXMatrix = xmatrix_end - xmatrix;
  HInteger lenCov = veccov_end - veccov;
  HInteger Nbreak(Ncoeffs+2-k);      /* nbreak = ncoeffs + 2 - k = ncoeffs - 2 since k = 4 */
  HInteger error;
  double chisq;
  gsl_matrix *X, *cov;
  gsl_vector *y, *c;

  if (k <= 0) {
    throw PyCR::ValueError("Spline: k order must be largrer then zero.");
    return -1.0;
  }

  if (lenXMatrix != (Ndata*Ncoeffs)) {
    throw PyCR::ValueError("vector sizes do not match: Vector lengths should be l(xMatrix) = l(coeffs)*l(data).");
    ERROR(BOOST_PP_STRINGIZE(HFPP_FUNC_NAME) << ": X input matrix vector has wrong size! len(xmatrix)=" << (xmatrix_end-xmatrix)  <<  ", should be = " << Ndata*Ncoeffs << ". Ndata=" << Ndata << ", Ncoeffs=" << Ncoeffs);
    return -1.0;
  };
  if (lenCov != (Ncoeffs*Ncoeffs)) {
    throw PyCR::ValueError("vector sizes do not match: Vector lengths should be l(cov) = l(coeffs)*l(coeffs).");
    ERROR(BOOST_PP_STRINGIZE(HFPP_FUNC_NAME) << ": Covariance matrix vector has wrong size! len(veccov)=" << (veccov_end-veccov)  <<  ", should be = " << Ncoeffs*Ncoeffs << ". Ncoeffs=" << Ncoeffs);
    return -1.0;
  };

  gsl_bspline_workspace *bw=gsl_bspline_alloc(k, Nbreak);

  X = STL2GSLM(xmatrix, Ndata, Ncoeffs);
  y = STL2GSL(yvec, yvec+Ndata);

  c = STL2GSL(vecout, vecout_end);
  cov = STL2GSLM(veccov, Ncoeffs, Ncoeffs);

  //If one can find out how much memory is needed, "work" could be provided as a scratch input vector to this function ...
  gsl_multifit_linear_workspace * work = gsl_multifit_linear_alloc (Ndata, Ncoeffs);
  gsl_bspline_knots_uniform(xmin, xmax, bw);
  hBSplineFitXValues(xmatrix,xmatrix_end,xvec,xvec_end,Ncoeffs,xmin,xmax,k);
  error=gsl_multifit_linear(X, y, c, cov, &chisq, work);
  gsl_multifit_linear_free (work);

  gsl_bspline_free(bw);

  delete X;
  delete cov;
  delete y;
  delete c;

  if (error!=0) ERROR(BOOST_PP_STRINGIZE(HFPP_FUNC_NAME) << ": GSL Fitting Routine returned error code " << error);

  return chisq;
  //  return (HInteger) bw;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//------------------------------------------------------------------------
// End k order spline
//------------------------------------------------------------------------


//$DOCSTRING: Calculates a polynomial from a vector of :math:`x`-values, coefficients, and powers writing it to the output vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hPolynomial
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HNumber)(vecout)()("Output vector containing the polynomial :math:`y`-values.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(xvec)()("Input vector with :math:`x`-values.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HNumber)(coeff)()("Coefficients for each power in powers.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_3 (HInteger)(powers)()("Input vector with the powers to take into account. For an n-th order polynomial, this is simply ``[0,1,2,...,n]``.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END ----------------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Usage:
  xvec=[x_0,x_1,x_2,...],powers=[0,1,2,....],coeff=[C_0,C_1,C_2,...]:
  vecout.polynmomial(xvec,coeff,powers) -> [sum(C_0,C_1*x_0,C_2*x_0**2,...),sum(C_0,C_1*x_1,C_2*x_1**2,...,C_n*x_1**n),...]

  See also:
  hLinearFit, hLinearFitPolynomialX, hPolynomialAdd, hPolynomialMul
*/
template <class Iter, class IterI>
void HFPP_FUNC_NAME(
		    const Iter vecout, const Iter vecout_end,
		    const Iter xvec, const Iter xvec_end,
		    const Iter coeff, const Iter coeff_end,
		    const IterI powers, const IterI powers_end
		       )
{
  // Declaration of variables
  Iter itout(vecout), itx(xvec);
  IterI itn(powers); Iter itc(coeff);
  HInteger lenOut   = vecout_end - vecout;
  HInteger lenX     = xvec_end - xvec;
  HInteger lenCoeff = coeff_end - coeff;
  HInteger lenPow   = powers_end - powers;
  HNumber sum(0.0);

  // Sanity check
  if (lenOut <= 0) {
    throw PyCR::ValueError("Illegal size of output vector.");
    return;
  }
  if (lenX <= 0) {
    throw PyCR::ValueError("Illegal size of x vector.");
    return;
  }
  if (lenCoeff <= 0) {
    throw PyCR::ValueError("Illegal size of coefficients vector.");
    return;
  }
  if (lenPow <= 0) {
    throw PyCR::ValueError("Illegal size of power vector.");
    return;
  }
  // Implementation
  while ((itout != vecout_end) && (itx != xvec_end)) {
    switch (*itn) {
    case 0:
      sum+=*itc;
      break;
    case 1:
      sum+=*itc * *itx;
      break;
    case 2:
      sum+=*itc * (*itx) * (*itx);
      break;
    default:
      sum+=*itc * pow(*itx,(int)*itn);
      break;
    };
    ++itn; ++itc;
    if ((itn==powers_end ) || (itc==coeff_end)) { //We have cycled through all powers, start at the beginning and go to next x value
      itn=powers; itc=coeff;
      *itout=sum; sum=0.0;
      ++itx; ++itout;
    };
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Calculates a polynomial from a vector of :math:`x`-values, coefficients, and powers multiplies it to the output vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hPolynomialMul
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HNumber)(vecout)()("Output vector containing the polynomial :math:`y`-values.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(xvec)()("Input vector with :math:`x`-values.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HNumber)(coeff)()("Coefficients for each power in powers.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_3 (HInteger)(powers)()("Input vector with the powers to take into account. For an n-th order polynomial, this is simply ``[0,1,2,...,n]``.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END ----------------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Description:
  Same as :func:`hPolynomialMul`, but multiply result to the output (i.e., use
  ``*=``). So, for a first iteration vecout has to be filled with 1.

  Usage:
  vecout.polynmomialmul(xvec,coeff,powers) -> vecout_0 *= [sum(C_0,C_1*x_0,C_2*x_0**2,...,C_n*x_0**n),sum(C_0,C_1*x_1,C_2*x_1**2,...,C_n*x_1**n),...]
  xvec=[x_0,x_1,x_2,...],powers=[0,1,2,....],coeff=[C_0,C_1,C_2,...]

  See also:
  hLinearFit, hLinearFitPolynomialX, hPolynomialAdd, hPolynomialMul, hPolynomial
*/
template <class Iter, class IterI>
void HFPP_FUNC_NAME(
		    const Iter vecout, const Iter vecout_end,
		    const Iter xvec, const Iter xvec_end,
		    const Iter coeff, const Iter coeff_end,
		    const IterI powers, const IterI powers_end
		       )
{
  // Declaration of variables
  Iter itout(vecout), itx(xvec);
  IterI itn(powers); Iter itc(coeff);
  HInteger lenOut   = vecout_end - vecout;
  HInteger lenX     = xvec_end - xvec;
  HInteger lenCoeff = coeff_end - coeff;
  HInteger lenPow   = powers_end - powers;
  HNumber sum(0.0);

  // Sanity check
  if (lenOut <= 0) {
    throw PyCR::ValueError("Illegal size of output vector.");
    return;
  }
  if (lenX <= 0) {
    throw PyCR::ValueError("Illegal size of x vector.");
    return;
  }
  if (lenCoeff <= 0) {
    throw PyCR::ValueError("Illegal size of coefficients vector.");
    return;
  }
  if (lenPow <= 0) {
    throw PyCR::ValueError("Illegal size of power vector.");
    return;
  }
  // Implementation
  while ((itout != vecout_end) && (itx != xvec_end)) {
    switch (*itn) {
    case 0:
      sum+=*itc;
      break;
    case 1:
      sum+=*itc * *itx;
      break;
    case 2:
      sum+=*itc * (*itx) * (*itx);
      break;
    default:
      sum+=*itc * pow(*itx,(int)*itn);
      break;
    };
    ++itn; ++itc;
    if ((itn==powers_end ) || (itc==coeff_end)) { //We have cycled through all powers, start at the beginning and go to next x value
      itn=powers; itc=coeff;
      *itout*=sum; sum=0.0;
      ++itx; ++itout;
    };
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Calculates a polynomial from a vector of xvalues, coefficients, and powers and adds it to the output vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hPolynomialAdd
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HNumber)(vecout)()("Output vector containing the polynomial y-values.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(xvec)()("Input vector with x-values.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HNumber)(coeff)()("Coefficients for each power in powers.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_3 (HInteger)(powers)()("Input vector with the powers to take into account. For an n-th order polynomial, this is simply [0,1,2,...,n].")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END ----------------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Description:
  Same as :func:`hPolynomialMul`, but add result to the output (i.e., use
  ``+=``). So, for a first iteration vecout has to be filled with 1.

  Usage:
  vecout.polynmomialadd(xvec,coeff,powers) -> vecout += [sum(C_0,C_1*x_0,C_2*x_0**2,...),sum(C_0,C_1*x_1,C_2*x_1**2,...,C_n*x_1**n),...]
  xvec=[x_0,x_1,x_2,...],powers=[0,1,2,....],coeff=[C_0,C_1,C_2,...]

  See also:
  hLinearFit, hLinearFitPolynomialX, hPolynomialAdd, hPolynomialMul, hPolynomial
*/
template <class Iter, class IterI>
void HFPP_FUNC_NAME(
		    const Iter vecout, const Iter vecout_end,
		    const Iter xvec, const Iter xvec_end,
		    const Iter coeff, const Iter coeff_end,
		    const IterI powers, const IterI powers_end
		       )
{
  // Declaration of variables
  Iter itout(vecout), itx(xvec);
  IterI itn(powers); Iter itc(coeff);
  HInteger lenOut   = vecout_end - vecout;
  HInteger lenX     = xvec_end - xvec;
  HInteger lenCoeff = coeff_end - coeff;
  HInteger lenPow   = powers_end - powers;
  HNumber sum(0.0);

  // Sanity check
  if (lenOut <= 0) {
    throw PyCR::ValueError("Illegal size of output vector.");
    return;
  }
  if (lenX <= 0) {
    throw PyCR::ValueError("Illegal size of x vector.");
    return;
  }
  if (lenCoeff <= 0) {
    throw PyCR::ValueError("Illegal size of coefficients vector.");
    return;
  }
  if (lenPow <= 0) {
    throw PyCR::ValueError("Illegal size of power vector.");
    return;
  }
  // Implementation
  while ((itout != vecout_end) && (itx != xvec_end)) {
    switch (*itn) {
    case 0:
      sum+=*itc;
      break;
    case 1:
      sum+=*itc * *itx;
      break;
    case 2:
      sum+=*itc * (*itx) * (*itx);
      break;
    default:
      sum+=*itc * pow(*itx,(int)*itn);
      break;
    };
    ++itn; ++itc;
    if ((itn==powers_end ) || (itc==coeff_end)) { //We have cycled through all powers, start at the beginning and go to next x value
      itn=powers; itc=coeff;
      *itout += sum; sum=0.0;
      ++itx; ++itout;
    };
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Calculates a weight factor from an error for a fitting routine, i.e. ``w=1/Error**2``.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hErrorsToWeights
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HNumber)(vecout)()("Output vector containing the weights.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(vecin)()("Input vector with errorvalues.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END ----------------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  See also:
  hLinearFit, hLinearFitPolynomialX, hPolynomial

  Example:
  [x_0,x_1,x_2,...] -> [1/x_0**2,1/x_1**2,...]
*/
template <class Iter>
void HFPP_FUNC_NAME(
		    const Iter vecout, const Iter vecout_end,
		    const Iter vecin, const Iter vecin_end
		    )
{
  // Declaration of variables
  Iter itout(vecout), itin(vecin);
  HInteger lenOut = vecout_end - vecout;
  HInteger lenIn  = vecin_end - vecin;

  // Sanity check
  if (lenOut <= 0) {
    throw PyCR::ValueError("Illegal size of output vector.");
    return;
  }
  if (lenIn <= 0) {
    throw PyCR::ValueError("Illegal size of input vector.");
    return;
  }
  if (lenIn != lenOut) {
    throw PyCR::ValueError("Output vector has different size than input vector.");
    return;
  }

  // Implementation
  try {
    while ((itout != vecout_end) && (itin != vecin_end)) {
      *itout = 1/((*itin) * (*itin));
      ++itout; ++itin;
    };
  }
  catch (HNumber i) { // IterValueType does not seem to work???
    if (abs(i) < A_LOW_NUMBER) {
      throw PyCR::ZeroDivisionError("Division by zero");
    }
  }
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Do a linear fit (e.g. to an n-th order polynomial) to a data set provided a vector of weights and return the coefficients and covariance matrix in two vectors.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hLinearFit
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF (HNumber)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HNumber)(vecout)()("Output vector containing the bets fit coefficients :math:`C_n` of the polynomial :math:`\sum_n C_n x^n`). The first element is :math:`C_0`, i.e. the constant. Its length determines the order of the polynomial.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(veccov)()("Output vector of length :math:`n^2` containing the covariance matrix of the fit.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HNumber)(xvec)()("Vector containing the :math:`x`-values of the data, where each value is actually one row of :math:`n+1` values of :math:`x^n` (e.g., if :math:`x`-values are ``[2,3] => xvec=[0,2,4,0,3,9]`` for a quadratic polynomical (:math:`n=2`)).")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_3 (HNumber)(yvec)()("Vector containing the :math:`y`-values of the data.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_4 (HNumber)(wvec)()("Vector containing the weights of the data (which are ``1/Error^2``).")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_5 (HInteger)(ndata)()("Number of data points to take into account (``ndata=-1`` -> take all elements in ``yvec``; if ``ndata > 0`` only take the first ``ndata``). ")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END ----------------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Description:
  Returns as function value the :math:`\\chi^2` value of the fit.

  See also:
  hLinearFitPolynomialX, hPolynomial
*/
template <class Iter>
HNumber HFPP_FUNC_NAME(
		    const Iter vecout, const Iter vecout_end,
		    const Iter veccov, const Iter veccov_end,
		    const Iter xvec, const Iter xvec_end,
		    const Iter yvec, const Iter yvec_end,
		    const Iter wvec, const Iter wvec_end,
		    const HInteger ndata
		    )
{
  HInteger Ndata(yvec_end-yvec);  // Number of data points to fit
  HInteger Ncoeff(vecout_end-vecout);
  HInteger lenX = xvec_end-xvec;
  HInteger lenW = wvec_end-wvec;
  HInteger lenCov = veccov_end - veccov;
  HInteger error;
  double chisq;
  gsl_matrix *X, *cov;
  gsl_vector *y, *w, *c;
  if (lenW != Ndata) {
    throw PyCR::ValueError("Weight vector has different size than data vector.");
    ERROR(BOOST_PP_STRINGIZE(HFPP_FUNC_NAME) << ": weights vector has wrong size! len(wvec)=" << (wvec_end-wvec)  <<  ", should be = " << Ndata << ". Ndata=" << Ndata);
    return -1.0;
  };
  if (lenX != (Ndata*Ncoeff)) {
    throw PyCR::ValueError("X vector size does not match: Vector length should be l(x) = l(coeff)*l(data).");
    ERROR(BOOST_PP_STRINGIZE(HFPP_FUNC_NAME) << ": X input matrix vector has wrong size! len(xvec)=" << (xvec_end-xvec)  <<  ", should be = " << Ndata*Ncoeff << ". Ndata=" << Ndata << ", Ncoeff=" << Ncoeff);
    return -1.0;
  };
  if (lenCov != (Ncoeff*Ncoeff)) {
    throw PyCR::ValueError("vector sizes do not match: Vector lengths should be l(cov) = l(coeff)*l(coeff).");
    ERROR(BOOST_PP_STRINGIZE(HFPP_FUNC_NAME) << ": Covariance matrix vector has wrong size! len(veccov)=" << (veccov_end-veccov)  <<  ", should be = " << Ncoeff*Ncoeff << ". Ncoeff=" << Ncoeff);
    return -1.0;
  };

  if (ndata>0) Ndata=min(ndata,Ndata);

  X = STL2GSLM(xvec, Ndata, Ncoeff);
  y = STL2GSL(yvec, yvec+Ndata);
  w = STL2GSL(wvec, wvec+Ndata);

  c = STL2GSL(vecout, vecout_end);
  cov = STL2GSLM(veccov, Ncoeff, Ncoeff);

  //If one can find out how much memory is needed, "work" could be provided as a scratch input vector to this function ...
  gsl_multifit_linear_workspace * work = gsl_multifit_linear_alloc (Ndata, Ncoeff);
  error=gsl_multifit_wlinear (X, w, y, c, cov,&chisq, work);
  gsl_multifit_linear_free (work);
  if (error!=0) ERROR(BOOST_PP_STRINGIZE(HFPP_FUNC_NAME) << ": GSL Fitting Routine returned error code " << error);

  //Make sure to delete all GSL vector and matrix structures - this will not
  delete X;
  delete cov;
  delete y;
  delete w;
  delete c;
  return chisq;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Do a linear fit (e.g. to an n-th order polynomial) to a data set (without weights) and return the coefficients and covariance matrix in two vectors.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hLinearFit
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF (HNumber)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HNumber)(vecout)()("Output vector containing the bets fit coefficients :math:`C_n` of the polynomial :math:`\\sum_n C_n x^n`. The first element is :math:`C_0`, i.e. the constant. Its length determines the order of the polynomial.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(veccov)()("Output vector of length :math:`n^2` containing the covariance matrix of the fit.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HNumber)(xvec)()("Vector containing the :math:`x`-values of the data, where each value is actually one row of :math:`n+1` values of :math:`x^n` (e.g., if :math:`x`-values are ``[2,3] => xvec=[0,2,4,0,3,9]`` for a quadratic polynomical (:math:`n=2`)).")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_3 (HNumber)(yvec)()("Vector containing the :math:`y`-values of the data.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_4 (HInteger)(ndata)()("Number of data points to take into account (``ndata=-1`` -> take all elements in ``yvec``; if ``ndata > 0`` only take the first ndata). ")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END ----------------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

  Description:
  Returns as function value the :math:`\\chi^2` value of the fit.

  See also:
  hLinearFitPolynomialX, hPolynomial
*/
template <class Iter>
HNumber HFPP_FUNC_NAME(
		    const Iter vecout, const Iter vecout_end,
		    const Iter veccov, const Iter veccov_end,
		    const Iter xvec, const Iter xvec_end,
		    const Iter yvec, const Iter yvec_end,
		    const HInteger ndata
		    )
{
  HInteger lenX = xvec_end - xvec;
  HInteger lenCov = veccov_end - veccov;
  HInteger Ndata(yvec_end-yvec);  // Number of data points to fit
  HInteger Ncoeff(vecout_end-vecout);
  HInteger error;
  double chisq;
  gsl_matrix *X, *cov;
  gsl_vector *y, *c;

  if (lenX != (Ndata*Ncoeff)) {
    throw PyCR::ValueError("vector sizes do not match: Vector lengths should be l(x) = l(coeffs)*l(data).");
    ERROR(BOOST_PP_STRINGIZE(HFPP_FUNC_NAME) << ": X input matrix vector has wrong size! len(xvec)=" << (xvec_end-xvec)  <<  ", should be = " << Ndata*Ncoeff << ". Ndata=" << Ndata << ", Ncoeff=" << Ncoeff);
    return -1.0;
  };
  if (lenCov != (Ncoeff*Ncoeff)) {
    throw PyCR::ValueError("vector sizes do not match: Vector lengths should be l(cov) = l(coeffs)*l(coeffs).");
    ERROR(BOOST_PP_STRINGIZE(HFPP_FUNC_NAME) << ": Covariance matrix vector has wrong size! len(veccov)=" << (veccov_end-veccov)  <<  ", should be = " << Ncoeff*Ncoeff << ". Ncoeff=" << Ncoeff);
    return -1.0;
  };

  if (ndata>0) Ndata=min(ndata,Ndata);

  X = STL2GSLM(xvec, Ndata, Ncoeff);
  y = STL2GSL(yvec, yvec+Ndata);

  c = STL2GSL(vecout, vecout_end);
  cov = STL2GSLM(veccov, Ncoeff, Ncoeff);

  //If one can find out how much memory is needed, "work" could be provided as a scratch input vector to this function ...
  gsl_multifit_linear_workspace * work = gsl_multifit_linear_alloc (Ndata, Ncoeff);
  error=gsl_multifit_linear (X, y, c, cov,&chisq, work);

  gsl_multifit_linear_free (work);

  delete X;
  delete cov;
  delete y;
  delete c;

  if (error!=0) ERROR(BOOST_PP_STRINGIZE(HFPP_FUNC_NAME) << ": GSL Fitting Routine returned error code " << error);

  return chisq;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$COPY_TO HFILE: #endif
