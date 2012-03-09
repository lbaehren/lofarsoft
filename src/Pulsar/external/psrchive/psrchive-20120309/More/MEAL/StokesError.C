/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "MEAL/StokesError.h"

//! Default constructor
MEAL::StokesError::StokesError ()
{
  built = true;
}

//! Set the variances of the input Stokes parameters
void MEAL::StokesError::set_variance (const Stokes<double>& var)
{
  input = var;
}

//! Set the variances of the input Stokes parameters
Stokes<double> MEAL::StokesError::get_input_variance () const
{
  return input;
}

//! Set the transformation from template to observation
void MEAL::StokesError::set_transformation (const Jones<double>& J)
{
  jones = J;
  xform = Mueller (J);
  built = false;
}

//! Get the variances of the output Stokes parameters
Stokes<double> MEAL::StokesError::get_variance () const
{
  Matrix<4,4,double> squared;

  for (unsigned i=0; i<4; i++)
    for (unsigned j=0; j<4; j++)
      squared[i][j] = xform[i][j] * xform[i][j];

  return squared * input;
}

//! Set the transformation gradient component
void MEAL::StokesError::set_transformation_gradient (const Jones<double>& grad)
{
  jones_grad = grad;
  built = false;
}

//! Get the variances of the output Stokes parameters gradient component
Stokes<double> MEAL::StokesError::get_variance_gradient () const
{
  if (!built)
    const_cast<StokesError*>(this)->build();

  return del * input;
}

void MEAL::StokesError::build ()
{
  Matrix<4,4,double> xform_grad = Mueller (jones, jones_grad);

  for (unsigned i=0; i<4; i++)
    for (unsigned j=0; j<4; j++)
      del[i][j] = 2.0 * xform[i][j] * xform_grad[i][j];

  built = true;
}
