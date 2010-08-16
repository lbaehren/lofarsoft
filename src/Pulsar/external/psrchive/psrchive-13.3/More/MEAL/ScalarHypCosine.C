/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "MEAL/ScalarHypCosine.h"
#include <math.h>

using namespace std;

//! Return the name of the class
string MEAL::ScalarHypCosine::get_name () const
{
  return "ScalarHypCosine";
}

//! The function
double MEAL::ScalarHypCosine::function (double x) const
{
  return cosh(x); 
}

//! And its first derivative
double MEAL::ScalarHypCosine::dfdx (double x) const
{ 
  return sinh(x);
}
