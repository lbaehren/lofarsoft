/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "MEAL/ScalarInverseTangent.h"
#include <math.h>

using namespace std;

//! Return the name of the class
string MEAL::ScalarInverseTangent::get_name () const
{
  return "ScalarInverseTangent";
}

//! The function
double MEAL::ScalarInverseTangent::function (double x) const
{
  return atan (x); 
}

//! The partial derivative with respect to arg1
double MEAL::ScalarInverseTangent::dfdx (double x) const
{
  return 1.0/(1+x*x);
}
