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


#ifdef HAVE_GSL
#include <gsl/gsl_multifit.h>
#include <gsl/gsl_bspline.h>
#endif


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
#ifdef HAVE_GSL
  gsl_set_error_handler_off ();
#else
  ERROR("hInit: GSL-Libraries not installed. Some functions are not defined.")
#endif
}


// ================================================================================
//$SECTION: GSL Fitting
// ================================================================================

//$COPY_TO HFILE: #ifdef HAVE_GSL
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



//$DOCSTRING: Calculate the XValues matrix vector for the polynomial (linear-fitting) routine, calculating essentially all powers of the input vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hLinearFitPolynomialX
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HNumber)(vecout)()("Output vector containing rows with all the requested powers of the x vector. Size is (n+1)*len(xvec), where n=len(powers).")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(xvec)()("Input vector with x values for the fitting routine.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HInteger)(powers)()("Input vector with the powers to consider in the fitting. For an n-th order polynomial, this is simply [0,1,2,...,n].")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END ----------------------------------------------------------
/*!
  [x_0,x_1,x_2,...] -> [0,x_0,x_0**2,...,x_0**n,0,x_1,x_1**2,...,x_1**n]

  n is the number of powers of the polynomial to fit,
  i.e. len(powers). The length of the output vector is the length of
  xvec*(n+1).

  \brief $DOCSTRING
  $PARDOCSTRING

See also: LinearFit
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

//$DOCSTRING: Calculate the X-Values matrix vector for the Basis Spline (BSpline) fitting routine, calculating essentially all powers of the input vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hBSplineFitXValues
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HNumber)(vecout)()("Output vector containing rows with all the requested powers of the x vector. Size is Ncoeffs*len(xvec).")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(xvec)()("Input vector with x values for the fitting routine.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HNumber)(xmin)()("Lower end of the x-range for calculation of break points.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_3 (HNumber)(xmax)()("Upper end of the x-range for calculation of break points.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//#define HFPP_PARDEF_2 (HInteger)(bwipointer)()("Pointer to the BSpline workspace returned as Integer number.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_4 (HInteger)(Ncoeffs)()("Number of coefficients to calculate.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END ----------------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

See also: BSplineFit, hBSpline, hBSplineFitXValues
*/
template <class Iter>
void HFPP_FUNC_NAME(
		    const Iter vecout, const Iter vecout_end,
		    const Iter xvec, const Iter xvec_end,
		    const HNumber xmin,
		    const HNumber xmax,
		    //		    const HInteger bwipointer
		    const HInteger Ncoeffs
		       )
{
  Iter itout(vecout), itout_end(vecout_end-Ncoeffs+1), itx(xvec);
  gsl_bspline_workspace *bw=gsl_bspline_alloc(4, Ncoeffs-2);
  gsl_bspline_knots_uniform(xmin, xmax, bw);
  gsl_vector *B = STL2GSL(itout, itout+Ncoeffs);
  HInteger lenOut = vecout_end - vecout;
  HInteger lenX   = xvec_end - xvec;

  // Sanity check
  if (lenOut <= 0) {
    throw PyCR::ValueError("Illegal size of output vector.");
    return;
  }
  if (lenX <= 0) {
    throw PyCR::ValueError("Illegal size of x vector.");
    return;
  }

  // Implementation
  while ((itout < itout_end) && (itx != xvec_end)) {
    gsl_bspline_eval(*itx, B, bw);
    itout+=Ncoeffs; ++itx; B->data=&(*itout);
  };
  gsl_bspline_free(bw);
  delete B;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Calculate the y-values of the results of a Basis Spline fit
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hBSpline
//-----------------------------------------------------------------------
#define HFPP_WRAPPER_CLASSES HFPP_CLASS_STL HFPP_CLASS_hARRAY HFPP_CLASS_hARRAYALL
#define HFPP_FUNCDEF (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HNumber)(vecout)()("Output vector containing the y-values for the input vector x-values.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(xvec)()("Input vector of Ndata*Ncoeffs X-values produced with hBSplineFitXValues. Size is Ncoeffs*len(xvec).")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HNumber)(coeffs)()("Input vector with the coefficients calculated by the fitting routine.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END ----------------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

See also:  BSplineFit, hBSpline, hBSplineFitXValues
*/
template <class Iter>
void HFPP_FUNC_NAME(
		    const Iter vecout, const Iter vecout_end,
		    const Iter xvec, const Iter xvec_end,
		    const Iter coeffs, const Iter coeffs_end
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
  if (lenOut <= 0) {
    throw PyCR::ValueError("Illegal size of output vector.");
    return;
  }
  if (lenX <= 0) {
    throw PyCR::ValueError("Illegal size of x vector.");
    return;
  }
  if (lenCoeffs <= 0) {
    throw PyCR::ValueError("Illegal size of coefficients vector.");
    return;
  }
  if (lenCoeffs*lenOut != lenX) {
    throw PyCR::ValueError("vector sizes do not match: Vector lengths should be l(x) = l(coeffs)*l(out).");
    return;
  }

  // Implementation
  while ((itout < vecout_end) && (itx_end < xvec_end)) {
    *itout=hMulSum(itx,itx_end,coeffs2,coeffs_end2);
    ++itout; itx=itx_end; itx_end+=Ncoeffs;
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Calculates a polynomial and adds it to the output vector.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hPolynomial
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HNumber)(vecout)()("Output vector containing the polynomial y-values.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(xvec)()("Input vector with x-values.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HNumber)(coeff)()("Coefficients for each  power in powers.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_3 (HInteger)(powers)()("Input vector with the powers to take into account. For an n-th order polynomial, this is simply [0,1,2,...,n].")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END ----------------------------------------------------------
/*!
  xvec=[x_0,x_1,x_2,...]
  coeff=[C_0,C_1,C_2,...] -> [sum(C_0,C_1*x_0,C_2*x_0**2),...,sum(C_0,C_1*x_1,C_2*x_1**2,...,C_n*x_1**n)]

  \brief $DOCSTRING
  $PARDOCSTRING

  Note that the polynomical is added to the output vector, so you need
  to initialize the output vector to zero!

See also: LinearFit, hLinearFitPolynomialX
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

  // Sanity check
  if (lenOut <= 0) {
    throw PyCR::ValueError("Illegal size of output vector.");
    return;
  }
  if (lenX <= 0) {
    throw PyCR::ValueError("Illegal size of x vector.");
    return;
  }
  if (lenOut != lenX) {
    throw PyCR::ValueError("Output vector has different size than x vector.");
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
  if (lenPow != lenCoeff) {
    throw PyCR::ValueError("Power vector has different size than coefficients vector.");
    return;
  }

  // Implementation
  while ((itout != vecout_end) && (itx != xvec_end)) {
    switch (*itn) {
    case 0:
      *itout+=*itc;
      break;
    case 1:
      *itout+=*itc * *itx;
      break;
    case 2:
      *itout+=*itc * (*itx) * (*itx);
      break;
    default:
      *itout+=*itc * pow(*itx,(int)*itn);
      break;
    };
    ++itn; ++itc;
    if ((itn==powers_end )|| (itc==coeff_end)) { //We have cycled through all powers, start at the beginning and go to next x value
      itn=powers; itc=coeff;
      ++itx; ++itout;
    };
  };
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Calculates a weight factor from an error for a fitting routine, i.e. w=1/Error**2.
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
[x_0,x_1,x_2,...] -> [1/x_0**2,1/x_1**2,...]

See also: hLinearFit, hLinearFitPolynomialX, hPolynomial
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


//$DOCSTRING: Do a linear fit (e.g. to an n-th order polynomial) to a data set provided a vector of weights and return the coefficients and covariance matrix in two vectors. Returns as function value the chi-square value of the fit.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hLinearFit
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF (HNumber)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HNumber)(vecout)()("Output vector containing the bets fit coefficients C_n of the polynomial Sum_n(C_n*x**n). The first element is C_0, i.e. the constant. Its length determines the order of the polynomial.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(veccov)()("Output vector of length n*n containing the covariance matrix of the fit.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HNumber)(xvec)()("Vector containing the x values of the data, where each value is actually one row of (n+1) values of x**n (e.g., if x values are [2,3] => xvec=[0,2,4,0,3,9] for a quadratic polynomical (n=2)).")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_3 (HNumber)(yvec)()("Vector containing the y values of the data.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_4 (HNumber)(wvec)()("Vector containing the weights of the data (which are 1/Error^2).")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_5 (HInteger)(ndata)()("Number of data points to take into account (ndata=-1 -> take all elements in yvec, if ndata>0 only take the first ndata). ")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END ----------------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

See also: hLinearFitPolynomialX,hPolynomial
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


//$DOCSTRING: Do a linear fit (e.g. to an n-th order polynomial) to a data set (without weights) and return the coefficients and covariance matrix in two vectors. Returns as function value the chi-square value of the fit.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hLinearFit
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF (HNumber)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HNumber)(vecout)()("Output vector containing the bets fit coefficients C_n of the polynomial Sum_n(C_n*x**n). The first element is C_0, i.e. the constant. Its length determines the order of the polynomial.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(veccov)()("Output vector of length n*n containing the covariance matrix of the fit.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HNumber)(xvec)()("Vector containing the x values of the data, where each value is actually one row of (n+1) values of x**n (e.g., if x values are [2,3] => xvec=[0,2,4,0,3,9] for a quadratic polynomical (n=2)).")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_3 (HNumber)(yvec)()("Vector containing the y values of the data.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_4 (HInteger)(ndata)()("Number of data points to take into account (ndata=-1 -> take all elements in yvec, if ndata>0 only take the first ndata). ")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END ----------------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

See also: hLinearFitPolynomialX,hPolynomial
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


//$DOCSTRING: Do a basis spline fit to a data set (without weights) and return the coefficients and covariance matrix in two vectors. Returns the chi-square value of the fit.
// Returns as function value a pointer to the bspline workspace that is needed by other functions.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hBSplineFit
//-----------------------------------------------------------------------
#define HFPP_FUNCDEF (HInteger)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (HNumber)(vecout)()("Output vector containing the bets fit coefficients C_n of the polynomial Sum_n(C_n*x**n). The first element is C_0, i.e. the constant. Its length determines the order of the polynomial.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HNumber)(veccov)()("Output vector of length n*n containing the covariance matrix of the fit.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HNumber)(xmatrix)()("Output vector containing the x values of the data, where each value is actually one row of ncoeff values some power of x. Calculated with hBSplineFitXValues.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_3 (HNumber)(xvec)()("Vector containing the x values of the data.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_4 (HNumber)(yvec)()("Vector containing the y values of the data.")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//#define HFPP_PARDEF_2 (HInteger)(bwipointer)()("Pointer to the BSpline workspace returned as Integer number.")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END ----------------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING

See also:  BSplineFit, hBSpline, hBSplineFitXValues
*/
template <class Iter>
HNumber HFPP_FUNC_NAME(
		    const Iter vecout, const Iter vecout_end,
		    const Iter veccov, const Iter veccov_end,
		    const Iter xmatrix, const Iter xmatrix_end,
		    const Iter xvec, const Iter xvec_end,
		    const Iter yvec, const Iter yvec_end
		    )
{
  HInteger Ndata(yvec_end-yvec);  // Number of data points to fit
  HInteger Ncoeffs(vecout_end-vecout);
  HInteger lenXMatrix = xmatrix_end - xmatrix;
  HInteger lenCov = veccov_end - veccov;
  HInteger Nbreak(Ncoeffs-2);      /* nbreak = ncoeffs + 2 - k = ncoeffs - 2 since k = 4 */
  HInteger error;
  double chisq;
  gsl_matrix *X, *cov;
  gsl_vector *y, *c;


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

  gsl_bspline_workspace *bw=gsl_bspline_alloc(4, Nbreak);

  X = STL2GSLM(xmatrix, Ndata, Ncoeffs);
  y = STL2GSL(yvec, yvec+Ndata);

  c = STL2GSL(vecout, vecout_end);
  cov = STL2GSLM(veccov, Ncoeffs, Ncoeffs);

  //If one can find out how much memory is needed, "work" could be provided as a scratch input vector to this function ...
  gsl_multifit_linear_workspace * work = gsl_multifit_linear_alloc (Ndata, Ncoeffs);
  gsl_bspline_knots_uniform(*xvec, *(xvec_end-1), bw);
  //  hBSplineFitXValues(xmatrix,xmatrix_end,xvec,xvec_end,(HInteger)bw);
  hBSplineFitXValues(xmatrix,xmatrix_end,xvec,xvec_end,*xvec,*(xvec_end-1),Ncoeffs);
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


//$COPY_TO HFILE: #endif
