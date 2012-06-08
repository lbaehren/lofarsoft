/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "MEAL/ScalarCosine.h"
#include <math.h>

using namespace std;

//! Return the name of the class
string MEAL::ScalarCosine::get_name () const
{
  return "ScalarCosine";
}

//! The function
double MEAL::ScalarCosine::function (double x) const
{
  return cos(x); 
}

//! And its first derivative
double MEAL::ScalarCosine::dfdx (double x) const
{ 
  return -sin(x);
}
