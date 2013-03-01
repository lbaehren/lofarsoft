/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "MEAL/ScalarAbsolute.h"
#include <math.h>

using namespace std;

//! Return the name of the class
string MEAL::ScalarAbsolute::get_name () const
{
  return "ScalarAbsolute";
}

//! The function
double MEAL::ScalarAbsolute::function (double x) const
{
  return fabs(x); 
}

//! And its first derivative
double MEAL::ScalarAbsolute::dfdx (double x) const
{ 
  if (x < 0)
    return -1;
  else
    return 1;
}
