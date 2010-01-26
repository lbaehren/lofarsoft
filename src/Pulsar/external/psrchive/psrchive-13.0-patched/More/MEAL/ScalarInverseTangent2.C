/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "MEAL/ScalarInverseTangent2.h"
#include <math.h>

using namespace std;

//! Return the name of the class
string MEAL::ScalarInverseTangent2::get_name () const
{
  return "ScalarInverseTangent2";
}

//! The function
double MEAL::ScalarInverseTangent2::function (double arg1,
						     double arg2) const
{
  return atan2 (arg1, arg2); 
}

//! The partial derivative with respect to arg1
double MEAL::ScalarInverseTangent2::partial_arg1 (double arg1,
							 double arg2) const
{
  double z = arg1/arg2;
  double datan_dz = 1.0/(1.0 + z*z);
  double dz_darg1 = 1.0/arg2;
  return datan_dz * dz_darg1;
}

//! The partial derivative with respect to arg2
double MEAL::ScalarInverseTangent2::partial_arg2 (double arg1,
							 double arg2) const
{
  double z = arg1/arg2;
  double datan_dz = 1.0/(1.0 + z*z);
  double dz_darg2 = -z/arg2;
  return datan_dz * dz_darg2;
}

