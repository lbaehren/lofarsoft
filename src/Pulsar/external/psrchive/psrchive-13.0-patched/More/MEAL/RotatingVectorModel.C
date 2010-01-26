/***************************************************************************
 *
 *   Copyright (C) 2005-2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "MEAL/RotatingVectorModel.h"
#include "MEAL/ScalarMath.h"
#include "MEAL/ScalarParameter.h"
#include "MEAL/ScalarArgument.h"

using namespace std;

void MEAL::RotatingVectorModel::init ()
{
  if (verbose)
    cerr << "MEAL::RotatingVectorModel::init" << endl;

  ScalarArgument* argument = new ScalarArgument; 

  reference_position_angle = new ScalarParameter;
  reference_position_angle->set_value_name ("PA_0");

  line_of_sight = new ScalarParameter;
  line_of_sight->set_value_name ("zeta");

  magnetic_axis = new ScalarParameter;
  magnetic_axis->set_value_name ("alpha");

  magnetic_meridian = new ScalarParameter;
  magnetic_meridian->set_value_name ("phi_0");

  ScalarMath longitude = *argument - *magnetic_meridian;

  ScalarMath numerator = sin(*magnetic_axis) * sin(longitude);

  /*
    The original RVM sign convention for PA is opposite to that of the IAU.
    See Everett & Weisberg (2001; ApJ 553:341) for more details.
  */
  ScalarMath denominator = 
    sin(*magnetic_axis) * cos(*line_of_sight) * cos(longitude)
  - cos(*magnetic_axis) * sin(*line_of_sight);

  ScalarMath result = atan(numerator/denominator) + *reference_position_angle;

  expression = result.get_expression();

  copy_parameter_policy  (expression);
  copy_evaluation_policy (expression);
  copy_univariate_policy (argument);
}

MEAL::RotatingVectorModel::RotatingVectorModel ()
{
  init ();
}

//! Copy constructor
MEAL::RotatingVectorModel::RotatingVectorModel (const RotatingVectorModel& copy)
{
  init ();
  operator = (copy);
}

//! Assignment operator
MEAL::RotatingVectorModel&
MEAL::RotatingVectorModel::operator = (const RotatingVectorModel& copy)
{
  Univariate<Scalar>::operator = (copy);
  return *this;
}

MEAL::RotatingVectorModel::~RotatingVectorModel ()
{
}

//! Return the name of the class
string MEAL::RotatingVectorModel::get_name () const
{
  return "RotatingVectorModel";
}

