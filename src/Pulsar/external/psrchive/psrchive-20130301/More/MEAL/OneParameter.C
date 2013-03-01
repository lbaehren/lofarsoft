/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "MEAL/OneParameter.h"
#include "MEAL/Function.h"

using namespace std;

MEAL::OneParameter::OneParameter (Function* context)
  : ParameterPolicy(context), fit(true)
{
}


MEAL::OneParameter::OneParameter (const OneParameter& p)
  : ParameterPolicy (0), param(p.param), fit(p.fit),
    name(p.name), description(p.description)
{
}

//! Clone construtor
MEAL::OneParameter* MEAL::OneParameter::clone (Function* context) const
{
  MEAL::OneParameter* retval = new OneParameter (context);
  *retval = *this;
  return retval;
}

MEAL::OneParameter& 
MEAL::OneParameter::operator = (const OneParameter& np)
{
  if (&np == this)
    return *this;

  param = np.param;
  fit = np.fit;
  name = np.name;
  description = np.description;

  if (get_context())
    get_context()->set_evaluation_changed ();

  return *this;
}


//! Set the value of the specified parameter
void MEAL::OneParameter::set_param (double value)
{
  if (param.val == value)
    return;

  param.val = value;

  if (get_context())
  {
    if (Function::very_verbose)
      cerr << "MEAL::OneParameter::set_param set_evaluation_changed" << endl;
    get_context()->set_evaluation_changed ();
  }
}

