/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "MEAL/ScalarConstant.h"
#include "MEAL/Constant.h"
#include "MEAL/NotCached.h"

using namespace std;

//! Default constructor
MEAL::ScalarConstant::ScalarConstant (double _value)
{
  parameter_policy = new Constant;
  evaluation_policy = new NotCached<Scalar> (this);

  value = _value;
}

//! Assignment operator
const MEAL::ScalarConstant&
MEAL::ScalarConstant::operator = (const ScalarConstant& scalar)
{
  throw Error (InvalidState, "MEAL::ScalarConstant::operator =",
	       "cannot assign new value to constant");
}

//! Return the name of the class
string MEAL::ScalarConstant::get_name () const
{
  return "ScalarConstant";
}

void MEAL::ScalarConstant::calculate (double& retval,
				      std::vector<double >* grad)
{
  if (grad)
    grad->resize(0);

  retval = value;
}
