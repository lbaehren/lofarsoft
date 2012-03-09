/***************************************************************************
 *
 *   Copyright (C) 2010 by Paul Demorest
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "SplineFit.h"

#include <algorithm>
#include <math.h>
#include <gsl/gsl_bspline.h>
#include <gsl/gsl_multifit.h>
#include <gsl/gsl_blas.h>

using namespace std;

SplineFit::SplineFit ()
{
  order = 3; // default = cubic
  chi2 = 0.0;
  ndof = 0;
  calculated = false;
  bwork = NULL;
  coeffs = NULL;
  cov = NULL;
}

SplineFit::~SplineFit ()
{
  reset();
}

void SplineFit::reset ()
{
  x.resize(0);
  y.resize(0);
  bp.resize(0);
  chi2 = 0.0;
  ndof = 0;
  calculated = false;
  free_workspaces();
}

void SplineFit::free_workspaces()
{
  if (bwork!=NULL) gsl_bspline_free(bwork);
  if (coeffs!=NULL) gsl_vector_free(coeffs);
  if (cov!=NULL) gsl_matrix_free(cov);
  bwork = NULL;
  coeffs = NULL;
  cov = NULL;
}

bool SplineFit::check_range(double x_in)
{
  // Needs to be in range
  double xmin = *min_element(bp.begin(),bp.end());
  double xmax = *max_element(bp.begin(),bp.end());
  if (x_in < xmin)
    return false;
  else if (x_in > xmax) 
    return false;
  else
    return true;
}

void SplineFit::add_data(double x_in, Estimate<double> y_in)
{
  x.push_back(x_in);
  y.push_back(y_in);
  calculated=false;
}

void SplineFit::set_uniform_breaks(int nint)
{
  calculated = false;
  bp.resize(nint+1);
  double x0 = *min_element(x.begin(),x.end());
  double x1 = *max_element(x.begin(),x.end());
  bp[0] = x0;
  bp[nint] = x1;
  double step = (x1 - x0) / (double)nint;
  for (int i=1; i<nint; i++) {
    bp[i] = x0 + step*(double)i;
  }
}

double SplineFit::evaluate(double x_in)
{
  if (!calculated)
    throw Error (InvalidState, "SplineFit::evaluate",
        "evaluate() called before compute()");

  if (!check_range(x_in))
    throw Error (InvalidParam, "SplineFit::evaluate",
        "input x outside of fit range");

  unsigned nc = coeffs->size;
  gsl_vector *c = gsl_vector_alloc(nc);
  gsl_bspline_eval(x_in, c, bwork);
  double result;
  gsl_blas_ddot(c, coeffs, &result);
  gsl_vector_free(c);
  return result;

#if 0 
  // To calculate variance
  double result_var;
  gsl_vector *c2 = gsl_vector_alloc(nc);
  gsl_blas_dsymv(CblasUpper, 1.0, cov, c, 0.0, c2);
  gsl_blas_ddot(c, c2, &result_var);
  gsl_vector_free(c2);
#endif
}

double SplineFit::evaluate_deriv(double x_in)
{
  if (!calculated)
    throw Error (InvalidState, "SplineFit::evaluate_deriv",
        "evaluate_deriv() called before compute()");

  if (!check_range(x_in))
    throw Error (InvalidParam, "SplineFit::evaluate_deriv",
        "input x outside of fit range");

  throw Error(InvalidState, "SplineFit::evaluate_deriv",
      "Not implemented yet.");

  // TODO
}

double SplineFit::get_rchi2()
{
  if (!calculated)
    throw Error (InvalidState, "SplineFit::get_rchi2",
        "get_rchi2() called before compute()");
  return chi2 / (double)ndof;
}

void SplineFit::interval_check(bool fix) 
{
  // Determine if data points are appropriately distributed
  // to use with the current knot vector.  If fix is true,
  // remove knots as appropriate to fix the siutation.
  unsigned np = x.size();
  unsigned nb = bp.size() - 1;
  std::vector<double> counts;
  counts.resize(nb);
  for (unsigned i=0; i<np; i++) {
    // Yes, this is not efficient..
    for (unsigned j=0; j<nb; j++) {
      if (x[i]>bp[j] && x[i]<bp[j+1]) {
        counts[j]++;
        break;
      }
    }
  }

  if (fix) {

    bool skip=false;
    int s0=0, s1=0;
    for (unsigned i=0; i<nb; i++) {
      if (skip==false && counts[i]==0) {
        // Start of a new skipped range
        skip = true;
        s0 = i;
        s1 = i+1;
      } else if (skip==true && counts[i]==0) {
        // Extend current skipped range
        s1 = i+1;
      } else if (skip==true && counts[i]!=0) {
        // Close current skipped range:
        //   Erase range of missing breakpoints
        bp.erase(bp.begin()+s0,bp.begin()+s1+1);
        counts.erase(counts.begin()+s0,counts.begin()+s1+1);
        //   Update loop indices
        nb -= (s1-s0+1);
        i -= (s1-s0+1);
        skip = false;
      }
    }

  }

}

// Make a new gsl vector filled with values from std::vector input
// Remember to free these when we're done ;)
static gsl_vector *vector_convert(std::vector<double> v)
{
  gsl_vector *res = gsl_vector_alloc(v.size());
  for (unsigned i=0; i<v.size(); i++) gsl_vector_set(res, i, v[i]);
  return res;
}

void SplineFit::compute()
{

  // Check that intervals are set up ok
  interval_check(true);
  
  // If old results are hanging around, free them
  // then allocate new ones
  free_workspaces();
  bwork = gsl_bspline_alloc(order+1, bp.size());

  // Set up knots
  gsl_vector *bpv = vector_convert(bp);
  gsl_bspline_knots(bpv, bwork);
  unsigned nc = gsl_bspline_ncoeffs(bwork);
  unsigned np = x.size();

  // Set up fit inputs
  gsl_vector *xv = vector_convert(x);
  gsl_vector *yv = gsl_vector_alloc(np);
  gsl_vector *wv = gsl_vector_alloc(np);
  for (unsigned i=0; i<np; i++) {
    gsl_vector_set(yv, i, y[i].get_value());
    gsl_vector_set(wv, i, 1.0/y[i].get_variance());
  }
  gsl_matrix *A = gsl_matrix_alloc(np, nc);
  gsl_vector *bv = gsl_vector_alloc(nc);
  for (unsigned ip=0; ip<np; ip++) {
    gsl_bspline_eval(gsl_vector_get(xv,ip), bv, bwork);
    for (unsigned ic=0; ic<nc; ic++) {
      gsl_matrix_set(A, ip, ic, gsl_vector_get(bv, ic));
    }
  }

  // Do fit
  coeffs = gsl_vector_alloc(nc);
  cov = gsl_matrix_alloc(nc, nc);
  gsl_multifit_linear_workspace *fwork = gsl_multifit_linear_alloc(np, nc);
  gsl_multifit_wlinear(A, wv, yv, coeffs, cov, &chi2, fwork);
  ndof = np - nc;

  // Free up
  gsl_vector_free(bpv);
  gsl_vector_free(xv);
  gsl_vector_free(yv);
  gsl_vector_free(wv);
  gsl_vector_free(bv);
  gsl_matrix_free(A);
  gsl_multifit_linear_free(fwork);

  calculated = true;
}

