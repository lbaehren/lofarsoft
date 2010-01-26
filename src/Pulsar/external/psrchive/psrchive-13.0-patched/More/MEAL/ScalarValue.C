/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "MEAL/ScalarValue.h"
#include "MEAL/NoParameters.h"
#include "MEAL/NotCached.h"

MEAL::ScalarValue::ScalarValue (double _value)
{
  parameter_policy = new NoParameters;
  evaluation_policy = new NotCached<Scalar> (this);

  value = _value;
}

//! Return the name of the class
std::string MEAL::ScalarValue::get_name () const
{
  return "ScalarValue";
}

void MEAL::ScalarValue::set_value (double _value)
{
  if (value == _value)
    return;

  value = _value;
  set_evaluation_changed();
}

double MEAL::ScalarValue::get_value () const
{
  return value;
}

//! Return the value (and gradient, if requested) of the function
void MEAL::ScalarValue::calculate (double& result, std::vector<double>* grad)
{
  result = value;

  if (grad)
    grad->resize (0);
}
