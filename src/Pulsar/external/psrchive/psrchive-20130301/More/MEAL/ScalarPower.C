/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "MEAL/ScalarPower.h"
#include <math.h>

using namespace std;

//
//
//
string MEAL::ScalarPower::get_name () const
{
  return "ScalarPower";
}

//
//
//
double MEAL::ScalarPower::function (double x, double y) const
{
  return pow (x, y);
}

//
//
//
double MEAL::ScalarPower::partial_arg1 (double x, double y) const
{
  // d/dx x^y = y x^(y-1)
  return y * pow (x, y-1.0);
}

//
//
//
double MEAL::ScalarPower::partial_arg2 (double x, double y) const
{
  // d/dy x^y = ln(x) x^y
  return log(x) * pow (x, y);
}
