/***************************************************************************
 *
 *   Copyright (C) 2006 by Russell Edwards
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "MEAL/VonMises.h"
#include "MEAL/InverseRule.h"
#include "MEAL/ScalarBesselI0.h"
#include "MEAL/ScalarConstant.h"
 
using namespace std;

//InverseRule needs this... maybe not the best place to define it??
static inline double inv(double x) { return 1.0/x;} 

void MEAL::VonMises::init ()
{
  centre.set_value_name("centre");
  concentration.set_value_name("concentration");
  
  ScalarArgument* argument = new ScalarArgument; 

  ScalarMath numerator = exp (ScalarMath(concentration)
			      *cos(ScalarMath(argument)-ScalarMath(centre)));

  ScalarBesselI0 *bessel = new ScalarBesselI0;
  bessel->set_model(&concentration);
  ScalarMath denominator(*bessel);
  
  ScalarMath result = numerator / (2.0*M_PI*denominator);

  expression = result.get_expression();

  copy_parameter_policy  (expression);
  copy_evaluation_policy (expression);
  copy_univariate_policy (argument);
}

MEAL::VonMises::VonMises ()
{
  init ();
}

//! Copy constructor
MEAL::VonMises::VonMises (const VonMises& copy)
{
  init ();
  operator = (copy);
}

//! Assignment operator
MEAL::VonMises& MEAL::VonMises::operator = (const VonMises& copy)
{
  Univariate<Scalar>::operator = (copy);
  return *this;
}

MEAL::VonMises::~VonMises ()
{
}

//! Set the centre
void MEAL::VonMises::set_centre (double centre_)
{
  centre.set_param(0, centre_);
}

//! Get the centre
double MEAL::VonMises::get_centre () const
{
  return centre.get_param (0);
}

//! Set the concentration
void MEAL::VonMises::set_concentration (double concentration_)
{
  concentration.set_param (0, concentration_);
}

//! Get the concentration
double MEAL::VonMises::get_concentration () const
{
  return concentration.get_param (0);
}

#if 0
void MEAL::VonMises::set_argument (unsigned dimension,
	 			   Argument* argument)
 
{
  x.set_argument(dimension, argument);
}
#endif

std::string 
MEAL::VonMises::get_name() const
{
  return "VonMises";
}

void MEAL::VonMises::parse (const string& line)
{
  Function::parse(line); // avoid using inherited GroupRule::parse()
}

