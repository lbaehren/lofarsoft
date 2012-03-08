/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "MEAL/Null.h"
#include "tostring.h"

using namespace std;

void MEAL::Null::init ()
{
  for (unsigned i=0; i<get_nparam(); i++)
    parameters.set_name (i, "c_" + tostring(i));
}

MEAL::Null::Null (unsigned ncoef)
  : parameters (this, ncoef)
{
  init ();
}

//! Copy constructor
MEAL::Null::Null (const Null& copy)
  : parameters (this)
{
  operator = (copy);
}

//! Assignment operator
MEAL::Null& MEAL::Null::operator = (const Null& copy)
{
  if (&copy != this)
    parameters = copy.parameters;

  return *this;
}

//! Return the name of the class
string MEAL::Null::get_name () const
{
  return "Null";
}

void MEAL::Null::resize (unsigned nparam)
{
  parameters.resize (nparam);
}

