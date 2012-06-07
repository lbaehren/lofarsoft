/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "MEAL/ScalarSine.h"
#include <math.h>

using namespace std;

//! Return the name of the class
string MEAL::ScalarSine::get_name () const
{
  return "ScalarSine";
}

//! The function
double MEAL::ScalarSine::function (double x) const
{
  return sin(x); 
}

//! And its first derivative
double MEAL::ScalarSine::dfdx (double x) const
{ 
  return cos(x);
}
