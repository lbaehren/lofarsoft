/***************************************************************************
 *
 *   Copyright (C) 2005-2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "MEAL/ScalarParameter.h"
#include "MEAL/OneParameter.h"

#include <assert.h>

using namespace std;

MEAL::ScalarParameter::ScalarParameter (const Estimate<double>& value)
{
  new OneParameter (this);
  set_value (value);
  set_value_name ("value");
}

//! Return the name of the class
string MEAL::ScalarParameter::get_name () const
{
  return "ScalarParameter";
}

void MEAL::ScalarParameter::set_value (const Estimate<double>& value)
{
  set_Estimate (0, value);
}

Estimate<double> MEAL::ScalarParameter::get_value () const
{
  return get_Estimate (0);
}

void MEAL::ScalarParameter::set_fit (bool flag)
{
  set_infit (0, flag);
}

bool MEAL::ScalarParameter::get_fit () const
{
  return get_infit (0);
}

string MEAL::ScalarParameter::get_value_name () const
{
  return get_param_name (0);
}

void MEAL::ScalarParameter::set_value_name (const string& name)
{
  OneParameter* one = dynamic_cast<OneParameter*>( parameter_policy.get() );
  assert (one != NULL);
  one->set_name (name);
}

//! Return the value (and gradient, if requested) of the function
void MEAL::ScalarParameter::calculate (double& result, 
                                       std::vector<double>* grad)
{
  result = get_param(0);

  if (verbose) 
    cerr << "MEAL::ScalarParameter::calculate result\n"
      "   " << result << " name=" << get_value_name() << endl;
 
  if (!grad)
    return;

  grad->resize (1);
  (*grad)[0] = 1.0;

  if (verbose)
  {
    cerr << "MEAL::ScalarParameter::calculate gradient" << endl;
    for (unsigned i=0; i<grad->size(); i++)
      cerr << "   " << i << ":" << get_fit() << "=" << (*grad)[i] << endl;
  }

}

