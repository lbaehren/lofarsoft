/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "MEAL/ScalarHypTangent.h"
#include <math.h>

using namespace std;

//! Return the name of the class
string MEAL::ScalarHypTangent::get_name () const
{
  return "ScalarHypTangent";
}

//! The function
double MEAL::ScalarHypTangent::function (double x) const
{
  return tanh(x); 
}

//! And its first derivative
double MEAL::ScalarHypTangent::dfdx (double x) const
{ 
  double coshx = cosh(x);
  return 1.0/(coshx*coshx);
}
