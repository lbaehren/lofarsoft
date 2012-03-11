/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "MEAL/Complex2Constant.h"
#include "MEAL/Constant.h"
#include "MEAL/NotCached.h"

//! Default constructor
MEAL::Complex2Constant::Complex2Constant (const Jones<double>& jones)
{
  parameter_policy = new Constant;
  evaluation_policy = new NotCached<Complex2> (this);

  value = jones;
}

//! Assignment operator
const MEAL::Complex2Constant&
MEAL::Complex2Constant::operator = (const Complex2Constant& scalar)
{
  throw Error (InvalidState, "MEAL::Complex2Constant::operator =",
	       "cannot assign new value to constant");
}

//! Return the name of the class
std::string MEAL::Complex2Constant::get_name () const
{
  return "Complex2Constant";
}

void MEAL::Complex2Constant::calculate (Jones<double>& result,
                                        std::vector<Jones<double> >* grad)
{
  if (grad)
    grad->resize(0);

  result = value;
}
