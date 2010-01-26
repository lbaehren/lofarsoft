/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "MEAL/PhaseGradients.h"
#include "MEAL/OneParameter.h"

#include <cassert>
using namespace std;

MEAL::PhaseGradients::PhaseGradients (unsigned ncoef)
  : parameters (this)
{
  resize (ncoef);
}

//! Copy constructor
MEAL::PhaseGradients::PhaseGradients (const PhaseGradients& copy)
  : parameters (this)
{
  operator = (copy);
}

//! Assignment operator
MEAL::PhaseGradients& 
MEAL::PhaseGradients::operator = (const PhaseGradients& copy)
{
  if (&copy == this)
    return *this;

  parameters = copy.parameters;
  offsets = copy.offsets;
  igradient = copy.igradient;

  set_evaluation_changed();

  return *this;
}

//! Clone operator
MEAL::PhaseGradients* MEAL::PhaseGradients::clone () const
{
  return new PhaseGradients( *this );
}

//! Set the current phase gradient
void MEAL::PhaseGradients::set_igradient (unsigned i)
{
  if (i == igradient)
    return;

  if (i >= get_ngradient())
    throw Error (InvalidParam, "MEAL::PhaseGradients::set_igradient",
		 "invalid index=%d >= ngradient=%d", i, get_ngradient());

  igradient = i;
  set_evaluation_changed ();
}

//! Get the current phase gradient index
unsigned MEAL::PhaseGradients::get_igradient () const
{
  return igradient;
}

//! Set the current phase gradient index
void MEAL::PhaseGradients::set_offset (unsigned igradient, double offset)
{
#if 0
  cerr << "MEAL::PhaseGradients::set_offset"
    " i=" << igradient << " offset=" << offset << endl;
#endif
  offsets[igradient] = offset;
}

//! Get the current phase gradient index
double MEAL::PhaseGradients::get_offset (unsigned igradient) const
{
  return offsets[igradient];
}

//! Set the number of gradients
void MEAL::PhaseGradients::resize (unsigned ngradient)
{
  unsigned current = get_ngradient();

  if (current == ngradient)
    return;

  parameters.resize( ngradient );
  offsets.resize (ngradient);

  for (unsigned i=current; i<ngradient; i++)
  {
    parameters.set_name (i, "phi_" + tostring(i));
    offsets[i] = 0.0;
  }

  if (ngradient)
    set_igradient (ngradient-1);
}

//! Add another gradient to the set
void MEAL::PhaseGradients::add_gradient ()
{
  resize ( get_ngradient() + 1 );
}

//! Get the number of gradients
unsigned MEAL::PhaseGradients::get_ngradient () const
{
  return parameters.get_nparam();
}

//! Return the name of the class
string MEAL::PhaseGradients::get_name () const
{
  return "PhaseGradients";
}

//! Calculate the Jones matrix and its gradient, as parameterized by gain
void MEAL::PhaseGradients::calculate (Jones<double>& result,
				      vector<Jones<double> >* grad)
{
  double x = get_abscissa();

  double phase = ( get_param(igradient) + get_offset(igradient) ) * x;

  if (verbose)
    cerr << "MEAL::PhaseGradients::calculate phase=" << phase << endl;

  double cos_phase = cos(phase);
  double sin_phase = sin(phase);

  result = complex<double>(cos_phase, sin_phase);

  if (grad)
  {
    for (unsigned i=0; i<grad->size(); i++)
      (*grad)[i] = 0.0;
    (*grad)[igradient] = x * complex<double>(-sin_phase, cos_phase);
    
    if (verbose)
      cerr << "MEAL::PhaseGradients::calculate gradient" << endl
	   << "   " << (*grad)[igradient] << endl;
  }
  
}

