/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "MEAL/ScalarTangent.h"
#include <math.h>

using namespace std;

//! Return the name of the class
string MEAL::ScalarTangent::get_name () const
{
  return "ScalarTangent";
}

//! The function
double MEAL::ScalarTangent::function (double x) const
{
  return tan(x); 
}

//! And its first derivative
double MEAL::ScalarTangent::dfdx (double x) const
{
  double cosx = cos(x);
  return 1.0/(cosx*cosx);
}
