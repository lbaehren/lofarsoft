/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "MEAL/Complex2Value.h"
#include "MEAL/NoParameters.h"
#include "MEAL/NotCached.h"

MEAL::Complex2Value::Complex2Value (const Jones<double>& _value)
{
  parameter_policy = new NoParameters;
  evaluation_policy = new NotCached<Complex2> (this);

  value = _value;
}

//! Return the name of the class
std::string MEAL::Complex2Value::get_name () const
{
  return "Complex2Value";
}

void MEAL::Complex2Value::set_value (const Jones<double>& _value)
{
  if (value == _value)
    return;

  value = _value;
  set_evaluation_changed();
}

Jones<double> MEAL::Complex2Value::get_value () const
{
  return value;
}

//! Return the value (and gradient, if requested) of the function
void MEAL::Complex2Value::calculate (Jones<double>& result, 
				     std::vector< Jones<double> >* grad)
{
  result = value;

  if (grad)
    grad->resize (0);
}
