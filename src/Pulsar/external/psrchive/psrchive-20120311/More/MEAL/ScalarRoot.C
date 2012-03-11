/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "MEAL/ScalarRoot.h"
#include <math.h>

using namespace std;

//
//
//
string MEAL::ScalarRoot::get_name () const
{
  return "ScalarRoot";
}

//
//
//
double MEAL::ScalarRoot::function (double x) const
{
  return sqrt(x); 
}

//
//
//
double MEAL::ScalarRoot::dfdx (double x) const
{ 
  return 0.5/sqrt(x);
}
