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
  ScalarMath result = exp ((cos(ScalarMath(x)-centre)+*minus_one)*concentration) 
    * height;
//     * height;
//  ScalarMath result(*new ScalarConstant(-1.0));
//   ScalarMath result = exp (ScalarMath(concentration)
// 			 *(ScalarMath(*new ScalarConstant(-1.0))
// 				+cos(ScalarMath(x)-ScalarMath(centre))))
//     * height;
  expression = result.get_expression();

  copy_parameter_policy  (expression);
  copy_evaluation_policy (expression);
  copy_univariate_policy (x);
}

MEAL::ScaledVonMises::ScaledVonMises () 
{ init();}

//! Copy constructor
MEAL::ScaledVonMises::ScaledVonMises (const ScaledVonMises& copy)
{
  init ();
  operator = (copy);
}

//! Assignment operator
MEAL::ScaledVonMises&
MEAL::ScaledVonMises::operator = (const ScaledVonMises& copy)
{
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
  height.set_value (height_);
}

//! Get the height
 Estimate<double> MEAL::ScaledVonMises::get_height () const
{
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

