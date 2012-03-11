/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "MEAL/ScalarLogarithm.h"
#include <math.h>

using namespace std;

//! Return the name of the class
string MEAL::ScalarLogarithm::get_name () const
{
  return "ScalarLogarithm";
}

//! The function
double MEAL::ScalarLogarithm::function (double x) const
{
  if (x < 0.0)
    throw Error (InvalidState, "MEAL::ScalarLogarithm::function",
		 "x < 0");
  return log(x); 
}

//! And its first derivative
double MEAL::ScalarLogarithm::dfdx (double x) const
{ 
  if (x < 0.0)
    throw Error (InvalidState, "MEAL::ScalarLogarithm::dfdx",
		 "x < 0");
  return 1/x;
}
