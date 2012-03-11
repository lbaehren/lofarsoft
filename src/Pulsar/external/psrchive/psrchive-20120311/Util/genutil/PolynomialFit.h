//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2008 by Paul Demorest
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifndef __Pulsar_PolynomialFit_h
#define __Pulsar_PolynomialFit_h

#include "Estimate.h"
#include "Reference.h"


//! A simple 1-D polynomial fit, using GSL routines
class PolynomialFit : public Reference::Able {

public:

  //! Default constructor
  PolynomialFit ();

  //! Destructor
  virtual ~PolynomialFit ();

  //! Clear all current data, results
  void reset();

  //! Set the degree of the fit
  void set_degree(int n) { ndeg=n; calcd=false; }

  //! Get the current degree
  int get_degree() { return ndeg; }

  //! Add a data point
  void add_data(double x, Estimate<double> y);

  //! Compute the fit using current data
  void compute();

  //! Evaluate the fit solution at the given x
  double evaluate(double x);

  //! Evaluate the fit solution's derivative at the given x
  double evaluate_deriv(double x);

  //! Integrate the fit solution between x0 and x1
  double evaluate_integ(double x0, double x1);

  //! Integrate x^n * y(x) over the specified range
  double evaluate_moment(double x0, double x1, int n);

  //! Get the reduced chi2 of the fit
  double get_rchi2();

  //! Get the nth degree fitted polynomial coeff and error
  Estimate<double> get_coeff(int n);

protected:

  //! The x values for the fit
  std::vector<double> x;

  //! The y values/errors for the fit
  std::vector< Estimate<double> > y;

  //! Number of polynomial degrees (1=linear, 2=quadratic, etc)
  int ndeg;

  //! Avg x value
  double x_avg;

  //! The fit chi2
  double chi2;

  //! Has the fit been calculated?
  bool calcd;

  //! The fitted coeffs
  std::vector< Estimate<double> > coeffs;

  //! Check if a requested x val is in the fit range
  bool check_range(double x);

private:

};

#endif
