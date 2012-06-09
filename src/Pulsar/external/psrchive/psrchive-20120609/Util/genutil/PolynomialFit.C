/***************************************************************************
 *
 *   Copyright (C) 2008 by Paul Demorest
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "PolynomialFit.h"

#include <algorithm>
#include <math.h>
#include <gsl/gsl_multifit.h>

using namespace std;

PolynomialFit::PolynomialFit ()
{
  ndeg = 1; // Default = linear
  chi2 = 0.0;
  calcd = false;
}

PolynomialFit::~PolynomialFit ()
{
}

void PolynomialFit::reset ()
{
  x.resize(0);
  y.resize(0);
  chi2 = 0.0;
  calcd = false;
}

bool PolynomialFit::check_range(double x_in)
{
  // Give ourselves the range of the fit plus an extra 10% on each
  // side.
  double xmin = *min_element(x.begin(),x.end());
  double xmax = *max_element(x.begin(),x.end());
  double xmid = 0.5 * (xmax + xmin);
  double span = 1.2 * (xmax - xmin);
  if (x_in > xmid-0.5*span && x_in < xmid+0.5*span)
    return true;
  else
    return false;
}

void PolynomialFit::add_data(double x_in, Estimate<double> y_in)
{
  x.push_back(x_in);
  y.push_back(y_in);
  calcd=false;
}

double PolynomialFit::evaluate(double x_in)
{
  if (!calcd)
    throw Error (InvalidState, "PolynomialFit::evaluate",
        "evaluate() called before compute()");

  if (!check_range(x_in))
    throw Error (InvalidParam, "PolynomicalFit::evaluate",
        "input x outside of fit range");

  double result = 0.0, yy=1.0;
  x_in -= x_avg; // Get x offset

  for (unsigned i=0; i<=ndeg; i++) {
    result += coeffs[i].get_value() * yy;
    yy *= x_in;
  }

  return result;
}

double PolynomialFit::evaluate_deriv(double x_in)
{
  if (!calcd)
    throw Error (InvalidState, "PolynomialFit::evaluate_deriv",
        "evaluate_deriv() called before compute()");

  if (!check_range(x_in))
    throw Error (InvalidParam, "PolynomicalFit::evaluate_deriv",
        "input x outside of fit range");

  double result=0.0, yy=1.0;
  x_in -= x_avg;

  for (unsigned i=1; i<=ndeg; i++) {
    result += coeffs[i].get_value() * (double)i * yy;
    yy *= x_avg;
  }

  return result;
}

double PolynomialFit::evaluate_integ(double x0, double x1)
{
  if (!calcd)
    throw Error (InvalidState, "PolynomialFit::evaluate_integ",
        "evaluate_integ() called before compute()");

  return evaluate_moment(x0, x1, 0);
}

double PolynomialFit::evaluate_moment(double x0, double x1, int n)
{
  if (!calcd)
    throw Error (InvalidState, "PolynomialFit::evaluate_moment",
        "evaluate_moment() called before compute()");

  if (n>1) 
    throw Error (InvalidParam, "PolynomialFit::evaulate_moment",
        "only 0th and 1st moments implemented now");

  x0 -= x_avg;
  x1 -= x_avg;
  double xx0=pow(x0,n+1), xx1=pow(x1,n+1);
  double result = 0.0;
  for (unsigned i=0; i<=ndeg; i++) {
    // This may get numerically bad for very high order polynomials.
    result += coeffs[i].get_value() * (xx1 - xx0) / (double)(i+n+1);
    xx0 *= x0;
    xx1 *= x1;
  }

  if (n==0)
    return result;

  result += x_avg * evaluate_integ(x0+x_avg, x1+x_avg);

  return result;

}

double PolynomialFit::get_rchi2()
{
  if (!calcd)
    throw Error (InvalidState, "PolynomialFit::get_rchi2",
        "get_rchi2() called before compute()");
  return chi2 / ((double)x.size() - (double)ndeg - 1.0);
}

Estimate<double> PolynomialFit::get_coeff(int n)
{
  if (!calcd)
    throw Error (InvalidState, "PolynomialFit::get_coeff",
        "get_coeff() called before compute()");
  return coeffs[n];
}

void PolynomialFit::compute()
{

  const unsigned n = x.size();
  const unsigned np = ndeg+1;

  // Check that we have enough data points
  // Allow n==np case or not?
  if (n<np)
    throw Error (InvalidState, "PolynomialFit::compute",
        "Not enough data points (%d) for the requested degree (%d)",
        n, ndeg);

  gsl_matrix *XX, *cov;
  gsl_vector *yy, *ww, *cc;

  XX = gsl_matrix_alloc(n, np);
  yy = gsl_vector_alloc(n);
  ww = gsl_vector_alloc(n);

  cc = gsl_vector_alloc(np);
  cov = gsl_matrix_alloc(np,np);

  // Fill in data point vectors.
  for (unsigned i=0; i<n; i++) {
    gsl_vector_set(yy, i,  y[i].get_value());
    gsl_vector_set(ww, i,  1.0/y[i].get_variance());
  }

  // Do fit with wrt weighted avg x to avoid (blatant) numerical problems.
  x_avg = 0.0;
  double wsum=0.0;
  for (unsigned i=0; i<n; i++) {
    x_avg += x[i] * gsl_vector_get(ww,i);
    wsum += gsl_vector_get(ww,i);
  }
  x_avg /= wsum;

  // Fill in design matrix.
  for (unsigned i=0; i<n; i++) {
    double pp = 1.0; 
    double xtmp = x[i] - x_avg;
    for (unsigned ip=0; ip<np; ip++) {
      gsl_matrix_set(XX, i, ip, pp);
      pp *= xtmp;
    }
  }

  // Call fit routines.
  // Should we use _svd version of the call here?
  // Both versions use SVD internally, but _svd gives more explicit
  // control over tolerances, and returns effective rank of the fit.
  // Also: check return value? 
  //       check that chi2 return is what I think it is.
  gsl_multifit_linear_workspace *work = gsl_multifit_linear_alloc(n, np);
  gsl_multifit_wlinear(XX, ww, yy, cc, cov, &chi2, work);
  gsl_multifit_linear_free(work);

  // Fill results into class members
  coeffs.resize(np);
  for (unsigned ip=0; ip<np; ip++) 
    coeffs[ip] = Estimate<double>(gsl_vector_get(cc, ip), 
        gsl_matrix_get(cov,ip,ip));

  // Set calculated flag
  calcd = true;

}

