/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "MEAL/ScalarExponential.h"
#include <math.h>

using namespace std;

//! Return the name of the class
string MEAL::ScalarExponential::get_name () const
{
  return "ScalarExponential";
}

//! The function
double MEAL::ScalarExponential::function (double x) const
{
  return exp(x); 
}

//! And its first derivative
double MEAL::ScalarExponential::dfdx (double x) const
{ 
  return exp(x);
}
