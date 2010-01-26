/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "MEAL/PositionAngles.h"
#include "MEAL/ScalarMath.h"
#include "MEAL/ScalarParameter.h"
#include "MEAL/ScalarValue.h"

void MEAL::PositionAngles::init ()
{
  rotation_measure = new ScalarParameter;
  frequency = new ScalarValue;

  // speed of light in Mm/s
  double speed_of_light = 299.792458;

  ScalarMath wavelength = speed_of_light / *frequency;
  ScalarMath delta_PA = wavelength * *rotation_measure;
  delta_pa = delta_PA.get_expression ();

  copy_parameter_policy  (&pa);
  copy_evaluation_policy (&pa);
}

MEAL::PositionAngles::PositionAngles ()
{
  init ();
}

//! Copy constructor
MEAL::PositionAngles::PositionAngles (const PositionAngles& copy)
{
  init ();
  operator = (copy);
}

//! Assignment operator
MEAL::PositionAngles&
MEAL::PositionAngles::operator = (const PositionAngles& copy)
{
  pa = copy.pa;
  return *this;
}

//! Destructor
MEAL::PositionAngles::~PositionAngles ()
{
}

//! Get the size of the array
unsigned MEAL::PositionAngles::size () const
{
  return pa.size();
}

//! Set the index of the array
void MEAL::PositionAngles::set_index (unsigned index)
{
  pa.set_index (index);
}

//! Get the index of the array
unsigned MEAL::PositionAngles::get_index () const
{
  return pa.get_index ();
}

void MEAL::PositionAngles::push_back (const Estimate<double>& p)
{
  ScalarParameter* pa0 = new ScalarParameter (p);
  ScalarMath PA = *pa0 + *delta_pa;
  pa.push_back (PA.get_expression());
}

//! Set the position angle at infinite frequency
void MEAL::PositionAngles::set_position_angle (unsigned index, 
					       const Estimate<double>& pa)
{
  position_angle[index]->set_value (pa);
}

//! Get the position angle at infinite frequency
Estimate<double>
MEAL::PositionAngles::get_position_angle (unsigned index) const
{
  return position_angle[index]->get_value ();
}

//! Set the rotation measure
void MEAL::PositionAngles::set_rotation_measure (const Estimate<double>& rm)
{
  rotation_measure->set_value (rm);
}

//! Get the rotation measure
Estimate<double> MEAL::PositionAngles::get_rotation_measure () const
{
  return rotation_measure->get_value ();
}

void MEAL::PositionAngles::set_frequency (double freq)
{
  frequency->set_value (freq);
}

double MEAL::PositionAngles::get_frequency () const
{
  return frequency->get_value ();
}

//! Return the name of the class
std::string MEAL::PositionAngles::get_name () const
{
  return "PositionAngles";
}

