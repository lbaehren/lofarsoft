//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2010 by Paul Demorest
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifndef __Pulsar_SplineFit_h
#define __Pulsar_SplineFit_h

#include "Estimate.h"
#include "Reference.h"

#include <gsl/gsl_bspline.h>
#include <gsl/gsl_matrix.h>

//! Spline fitting for smoothing and/or interpolation, using 
//! GSL's bspline routines.
class SplineFit : public Reference::Able {

public:

  //! Default constructor
  SplineFit ();

  //! Destructor
  virtual ~SplineFit ();

  //! Clear all current data, results
  void reset();

  //! Set the degree of the fit
  void set_order(int n) { order=n; calculated=false; }

  //! Get the current degree
  int get_order() { return order; }

  //! Set uniform breakpoints to span the data
  void set_uniform_breaks(int nint);

  //! Add a data point
  void add_data(double x, Estimate<double> y);

  //! Compute the fit using current data
  void compute();

  //! Evaluate the fit solution at the given x
  double evaluate(double x);

  //! Evaluate the fit solution's derivative at the given x
  double evaluate_deriv(double x);

  //! Get the reduced chi2 of the fit
  double get_rchi2();

protected:

  //! The x values for the fit
  std::vector<double> x;

  //! The y values/errors for the fit
  std::vector< Estimate<double> > y;

  //! The spline breakpoints
  std::vector<double> bp;

  //! Spline order (0=const, 3=cubic, etc)
  int order;

  //! The fit chi2
  double chi2;

  //! Fit NDOF
  int ndof;

  //! Has the fit been calculated?
  bool calculated;

  //! The fitted coeffs
  gsl_vector *coeffs;

  //! The fit cov matrix
  gsl_matrix *cov;

  //! Check if a requested x val is in the fit range
  bool check_range(double x);

  //! Check if spline intervals and data make sense
  void interval_check(bool fix=false);

  //! Free spline workspaces
  void free_workspaces();

  //! bspline temp space
  gsl_bspline_workspace *bwork;

private:

};

#endif
