/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "MEAL/Invariant.h"
#include "MEAL/ScalarMath.h"

void MEAL::Invariant::init ()
{
  input = Vector< 4, Estimate<double> > ();

  result = input.invariant();

  bias = Estimate<double>();
}

MEAL::Invariant::Invariant ()
{
  init ();
}

MEAL::Invariant::Invariant (const Invariant&)
{
  init ();
}

MEAL::Invariant::~Invariant ()
{

}

MEAL::Invariant&
MEAL::Invariant::operator = (const Invariant&)
{
  return *this;
}

void MEAL::Invariant::set_Stokes (const Stokes<Estimate<float> >& stokes)
{
  Stokes<Estimate<double> > temp = stokes;
  set_Stokes (temp);
}

void MEAL::Invariant::set_Stokes (const Stokes<Estimate<double> >& stokes)
{
  double the_bias = stokes[0].get_variance();

  for (unsigned ipol=0; ipol<stokes.size(); ipol++) {
    input[ipol].get_expression()->set_Estimate( 0, stokes[ipol] );
    if (ipol)
      the_bias -= stokes[ipol].get_variance();
  }

  bias.get_expression()->set_Estimate( 0, the_bias );
}

//! Get the invariant interval
Estimate<double> MEAL::Invariant::get_invariant () const
{
  return result.get_Estimate();
}

//! Get the estimated bias due to measurement error
double MEAL::Invariant::get_bias () const
{
  return bias.get_Estimate().get_value();
}

MEAL::ScalarMath MEAL::Invariant::get_correct_result () const
{
  return result - bias;
}

