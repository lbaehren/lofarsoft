/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "MEAL/ScalarInverseHypTangent.h"
#include <math.h>

using namespace std;

//! Return the name of the class
string MEAL::ScalarInverseHypTangent::get_name () const
{
  return "ScalarInverseHypTangent";
}

//! The function
double MEAL::ScalarInverseHypTangent::function (double x) const
{
  if (fabs(x) > 1.0)
    throw Error (InvalidParam, "MEAL::ScalarInverseHypTangent::function",
		 "|x|=%lf > 1.0 is out of range of atanh(x)", x);

  return atanh(x); 
}

//! And its first derivative
double MEAL::ScalarInverseHypTangent::dfdx (double x) const
{ 
  return 1.0/(1.0 - x*x);
}
