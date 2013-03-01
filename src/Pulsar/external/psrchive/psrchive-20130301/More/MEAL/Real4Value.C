/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "MEAL/Real4Value.h"
#include "MEAL/NoParameters.h"
#include "MEAL/NotCached.h"

MEAL::Real4Value::Real4Value ()
{
  parameter_policy = new NoParameters;
  evaluation_policy = new NotCached<Real4> (this);
}

//! Return the name of the class
std::string MEAL::Real4Value::get_name () const
{
  return "Real4Value";
}

void MEAL::Real4Value::set_value (const Matrix<4,4,double>& _value)
{
  if (value == _value)
    return;

  value = _value;
  set_evaluation_changed();
}

Matrix<4,4,double> MEAL::Real4Value::get_value () const
{
  return value;
}

//! Return the value (and gradient, if requested) of the function
void MEAL::Real4Value::calculate (Matrix<4,4,double>& result, 
				  std::vector< Matrix<4,4,double> >* grad)
{
  result = value;

  if (grad)
    grad->resize (0);
}
