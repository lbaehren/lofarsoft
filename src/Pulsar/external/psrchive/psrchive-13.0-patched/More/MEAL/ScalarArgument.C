/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "MEAL/ScalarArgument.h"
#include "MEAL/NoParameters.h"

using namespace std;

//! Default constructor
MEAL::ScalarArgument::ScalarArgument ()
{
  parameter_policy = new NoParameters;
}

//! Return the name of the class
string MEAL::ScalarArgument::get_name () const
{
  return "ScalarArgument";
}

void MEAL::ScalarArgument::calculate (double& result, std::vector<double >* grad)
{
  if (grad)
    grad->resize(0);

  result = get_abscissa();
}

