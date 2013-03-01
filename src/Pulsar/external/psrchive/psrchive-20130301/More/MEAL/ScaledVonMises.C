/***************************************************************************
 *
 *   Copyright (C) 2006 by Russell Edwards
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "MEAL/ScaledVonMises.h"
#include "MEAL/InverseRule.h"
#include "MEAL/ScalarBesselI0.h"
#include "MEAL/ScalarConstant.h"
 
using namespace std;

void
MEAL::ScaledVonMises::init()
{
  ScalarArgument* x = new ScalarArgument; 
  height.set_value_name("height");
  centre.set_value_name("centre"); 
  concentration.set_value_name("concentration");

  ScalarConstant *minus_one = new ScalarConstant(-1.0);
  ScalarMath result = exp ((cos(ScalarMath(x)-centre)+*minus_one)*concentration);

  if (log_height)
      result *= exp( height );
  else
      result *= height;

  expression = result.get_expression();

  copy_parameter_policy  (expression);
  copy_evaluation_policy (expression);
  copy_univariate_policy (x);
}

MEAL::ScaledVonMises::ScaledVonMises (bool _log_height) 
{
    log_height = _log_height;
    init ();
}

//! Copy constructor
MEAL::ScaledVonMises::ScaledVonMises (const ScaledVonMises& copy)
{
  log_height = copy.log_height;
  init ();
  operator = (copy);
}

//! Assignment operator
MEAL::ScaledVonMises&
MEAL::ScaledVonMises::operator = (const ScaledVonMises& copy)
{
  if (copy.log_height != this->log_height)
  {
      log_height = copy.log_height;
      init ();
  }

  Univariate<Scalar>::operator = (copy);
  return *this;
}
  
 

//! Set the centre
void MEAL::ScaledVonMises::set_centre (const Estimate<double>&centre_)
{
  centre.set_value(centre_);
}

//! Get the centre
Estimate<double> MEAL::ScaledVonMises::get_centre () const
{
  return centre.get_value ();
}

//! Set the concentration
void MEAL::ScaledVonMises::set_concentration (const Estimate<double>& concentration_)
{
  concentration.set_value (concentration_);
}

//! Get the concentration
Estimate<double> MEAL::ScaledVonMises::get_concentration () const
{
  return concentration.get_value ();
} 

//! Set the height
void MEAL::ScaledVonMises::set_height (const Estimate<double>& height_)
{
    if (log_height)
	height.set_value (log(height_));
    else
	height.set_value (height_);
}

//! Get the height
 Estimate<double> MEAL::ScaledVonMises::get_height () const
{
    if (log_height)
	return exp(height.get_value());
    else
	return height.get_value();
}


std::string 
MEAL::ScaledVonMises::get_name() const
{
  return "ScaledVonMises";
}

// void MEAL::ScaledVonMises::parse (const string& line)
// {
//   Function::parse(line); // avoid using inherited GroupRule::parse()
// }

