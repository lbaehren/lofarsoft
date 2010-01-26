/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/TotalCovariance.h"
#include "MEAL/StokesCovariance.h"

#include <iostream>
using namespace std;

Calibration::TotalCovariance::TotalCovariance ()
{
  observation_covariance_set = false;
}

//! Set the uncertainty of the observation
void Calibration::TotalCovariance::set_covariance 
(const Matrix<4,4,double>& covar)
{
  observation_covariance = covar;
  observation_covariance_set = true;
  built = false;
}

//! Set the optimizing transformation
void Calibration::TotalCovariance::set_optimizing_transformation 
(const Matrix<4,4,double>& opt)
{
  optimizer = opt;
  built = false;
}


//! Given a coherency matrix, return the difference
double Calibration::TotalCovariance::get_weighted_norm
(const Jones<double>& matrix) const
{
  if (!built)
    const_cast<TotalCovariance*>(this)->build();

  Stokes< complex<double> > stokes = complex_coherency( matrix );
  return (stokes * inv_covar * conj(stokes)).real();
}


//! Given a coherency matrix, return the weighted conjugate matrix
Jones<double> Calibration::TotalCovariance::get_weighted_conjugate
(const Jones<double>& matrix) const try
{
  if (!built)
    const_cast<TotalCovariance*>(this)->build();

  Stokes< complex<double> > stokes = complex_coherency( matrix );
  Stokes< complex<double> > result = inv_covar * conj(stokes);

  return convert (result);
}
catch (Error& error)
{
  throw error += "Calibration::TotalCovariance::get_weighted_conjugate";
}


void Calibration::TotalCovariance::build ()
{
  MEAL::StokesCovariance compute;
  compute.set_transformation (optimizer * Mueller(transformation->evaluate()));

  compute.set_variance( real(template_variance) );
  Matrix<4,4,double> covar = compute.get_covariance();

  if (observation_covariance_set)
  {
    covar += observation_covariance;
  }
  else
  {
    compute.set_variance( real(observation_variance) );
    compute.set_transformation (optimizer);
    covar += compute.get_covariance();
  }

  inv_covar = inv (covar);
  built = true;
}
