/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "MEAL/UnivariatePolicy.h"
#include "MEAL/Axis.h"
#include "MEAL/Function.h"

#include <iostream>

using namespace std;

MEAL::UnivariatePolicy::UnivariatePolicy (Function* context)
 : ArgumentPolicy (context)
{
  abscissa = 0.0;
}

//! Destructor
MEAL::UnivariatePolicy::~UnivariatePolicy ()
{
}

//! Copy constructor
MEAL::UnivariatePolicy::UnivariatePolicy (const UnivariatePolicy& u)
 : ArgumentPolicy (0)
{
  abscissa = u.abscissa;
}

//! Assignment operator
const MEAL::UnivariatePolicy&
MEAL::UnivariatePolicy::operator = (const UnivariatePolicy& u)
{
  if (this != &u)
    set_abscissa( u.get_abscissa() );
  return *this;
}

//! Clone construtor
MEAL::UnivariatePolicy*
MEAL::UnivariatePolicy::clone (Function* context) const
{
  UnivariatePolicy* retval = new UnivariatePolicy (context);
  *retval = *this;
  return retval;
}

//! Set the abscissa value
void MEAL::UnivariatePolicy::set_abscissa (double value)
{
  if (abscissa == value)
    return;

  if (Function::verbose)
    cerr << "MEAL::UnivariatePolicy::set_abscissa " << value << endl;

  abscissa = value;
  get_context()->set_evaluation_changed ();
}

//! Get the abscissa value
double MEAL::UnivariatePolicy::get_abscissa () const
{
  return abscissa;
}

void MEAL::UnivariatePolicy::set_argument (unsigned dim, Argument* argument)
{
  if (Function::verbose)
    std::cerr << "MEAL::UnivariatePolicy::set_argument" << std::endl;

  if (dim != 0)
    return;

  Axis<double>* axis = dynamic_cast< Axis<double>* > (argument);
  if (!axis)
    return;

  axis->signal.connect (this, &MEAL::UnivariatePolicy::set_abscissa);
}
