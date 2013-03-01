/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "MEAL/StokesCovariance.h"

//! Default constructor
MEAL::StokesCovariance::StokesCovariance ()
{
  built = true;
  matrix_identity (xform);
}

//! Set the variances of the input Stokes parameters
void MEAL::StokesCovariance::set_variance (const Stokes<double>& var)
{
  input = var;
}

//! Set the variances of the input Stokes parameters
Stokes<double> MEAL::StokesCovariance::get_input_variance () const
{
  return input;
}

//! Set the transformation from template to observation
void MEAL::StokesCovariance::set_transformation (const Jones<double>& J)
{
  jones = J;
  xform = Mueller (J);
  built = false;
}

//! Set the transformation from template to observation
void MEAL::StokesCovariance::set_transformation (const Matrix<4,4,double>& M)
{
  jones = 0;
  xform = M;
  built = true;
}

//! Get the variances of the output Stokes parameters
Matrix<4,4,double> MEAL::StokesCovariance::get_covariance () const
{
  Matrix<4,4,double> variance;
  for (unsigned i=0; i<4; i++)
    variance[i][i] = input[i];
  return xform * variance * transpose(xform);
}

//! Set the transformation gradient component
void
MEAL::StokesCovariance::set_transformation_gradient (const Jones<double>& grad)
{
  jones_grad = grad;
  built = false;
}

//! Set the transformation gradient component
void MEAL::StokesCovariance::set_transformation_gradient 
(const Matrix<4,4,double>& grad)
{
  jones_grad = 0;
  xform_grad = grad;
  built = true;
}

//! Get the variances of the output Stokes parameters gradient component
Matrix<4,4,double> MEAL::StokesCovariance::get_covariance_gradient () const
{
  if (!built)
    const_cast<StokesCovariance*>(this)->build();

  Matrix<4,4,double> var;
  for (unsigned i=0; i<4; i++)
    var[i][i] = input[i];

  return xform_grad *var* transpose(xform) + xform *var* transpose(xform_grad);
}

void MEAL::StokesCovariance::build ()
{
  xform_grad = Mueller (jones, jones_grad);
  built = true;
}
