/***************************************************************************
 *
 *   Copyright (C) 2004-2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "MEAL/Parameters.h"
#include "MEAL/Function.h"

using namespace std;

MEAL::Parameters::Parameters (Function* context, unsigned nparam)
  : 
  ParameterPolicy (context),
  params (nparam),
  fit (nparam, true),
  names (nparam),
  descriptions (nparam)
{
}


MEAL::Parameters::Parameters (const Parameters& p)
  :
  ParameterPolicy (0), 
  params (p.params),
  fit (p.fit),
  names (p.names),
  descriptions (p.descriptions)
{
}

//! Clone construtor
MEAL::Parameters* MEAL::Parameters::clone (Function* context) const 
{
  MEAL::Parameters* retval = new Parameters (context, get_nparam());
  *retval = *this;
  return retval;
}

MEAL::Parameters& 
MEAL::Parameters::operator = (const Parameters& np)
{
  if (&np == this)
    return *this;

  if (Function::very_verbose)
    cerr << "MEAL::Parameters::operator= nparam=" << get_nparam()
         << " new nparam=" << np.get_nparam() << endl;

  bool nparam_changed = np.params.size() != params.size();

  params = np.params;
  fit = np.fit;
  names = np.names;
  descriptions = np.descriptions;

  get_context()->set_evaluation_changed ();

  if (nparam_changed)
    get_context()->changed.send (Function::ParameterCount);

  return *this;
}


//! Set the value of the specified parameter
void MEAL::Parameters::set_param (unsigned index, double value)
{
  range_check (index, "MEAL::Parameters::set_param");
  
  if (Function::very_verbose) cerr << "MEAL::Parameters::set_param "
		 "(" << index << "," << value << ")" << endl;
  
  if (params[index].val == value)
    return;

  params[index].val = value;
  get_context()->set_evaluation_changed ();
}

void MEAL::Parameters::swap_param (unsigned index, double& value)
{
  double temp = get_param (index);
  set_param (index, value);
  value = temp;
}

void MEAL::Parameters::resize (unsigned nparam)
{
  if (Function::very_verbose)
    cerr << "MEAL::Parameters::resize " << nparam << endl;
  
  unsigned current = params.size();
  params.resize (nparam);
  fit.resize (nparam);
  names.resize (nparam);
  descriptions.resize (nparam);

  for (; current < nparam; current++)
    fit[current] = true;
  
  get_context()->changed.send (Function::ParameterCount);
}

//! Erase the specified parameter
void MEAL::Parameters::erase (unsigned iparam)
{
  params.erase (params.begin() + iparam);
  fit.erase (fit.begin() + iparam);
  names.erase (names.begin() + iparam);
  descriptions.erase (descriptions.begin() + iparam);
  
  get_context()->changed.send (Function::ParameterCount);
}
