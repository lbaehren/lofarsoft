/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "MEAL/Steps.h"
#include <assert.h>

using namespace std;

MEAL::Steps::Steps ()
  : parameters (this)
{
  current_step = -1;
}

//! Copy constructor
MEAL::Steps::Steps (const Steps& copy)
  : parameters (this)
{
  operator = (copy);
}

//! Assignment operator
MEAL::Steps& MEAL::Steps::operator = (const Steps& copy)
{
  parameters = copy.parameters;
  step = copy.step;
  current_step = copy.current_step;

  return *this;
}

//! Clone operator
MEAL::Steps* MEAL::Steps::clone () const
{
  return new Steps( *this );
}

//! Return the name of the class
string MEAL::Steps::get_name () const
{
  return "Steps";
}

//! Add a step at the given point on the abscissa
/*! This method will insert the step so that they remain sorted in
  order of increasing abscissa */
void MEAL::Steps::add_step (double x)
{
  unsigned size = step.size();
  assert ( size == parameters.get_nparam() );

  step.resize( size + 1 );
  parameters.resize( size + 1 );

  // A) find the first step greater than the new step ...
  unsigned i=0;
  for (; i < size; i++)
    if (step[i] > x)
      break;

  // B) insert the new step in it's place (possibly the last element) ...
  double temp_step = step[i];
  double temp_parameter = parameters.get_param(i);

  step[i] = x;
  parameters.set_param(i, 0.0);
  current_step = i;

  // c) and shift the remaining values to the right.
  for (i++; i < size+1; i++) {
    std::swap (temp_step, step[i]);
    parameters.swap_param(i, temp_parameter);
  }
}

void MEAL::Steps::remove_step (unsigned istep)
{
  assert( istep < step.size() );
  step.erase (step.begin() + istep);
  parameters.erase (istep);
}

//! Set the abscissa of the specified step
void MEAL::Steps::set_step (unsigned istep, double x)
{
  assert( istep < step.size() );
  step[istep] = x;
  current_step = -1;
}

//! Get the abscissa of the specified step
double MEAL::Steps::get_step (unsigned istep) const
{
  assert( istep < step.size() );
  return step[istep];
}

//! Get the number of steps
unsigned MEAL::Steps::get_nstep () const
{
  return step.size();
}

//! Set the abscissa value
void MEAL::Steps::set_abscissa (double value)
{
  // A) search backward for the last step abscissa less than value
  int nstep = step.size();
  int istep = nstep - 1;

  if (verbose)
    cerr << "MEAL::Steps::set_abscissa value=" << value 
	 << " nstep=" << nstep << endl;

  while (istep >= 0) {
    if (step[istep] < value)
      break;
    istep --;
  }

  // B) if found, set the abscissa
  if (istep >= 0)
    Univariate<Scalar>::set_abscissa (step[istep]);

  if (verbose)
    cerr << "MEAL::Steps::set_abscissa step=" << istep << endl;

  current_step = istep;
}

unsigned MEAL::Steps::get_step () const
{
  if (current_step < 0)
    throw Error (InvalidState, "MEAL::Steps::get_step",
		 "current step unknown or invalid");
  return current_step;
}

//! Return the value (and gradient, if requested) of the function
void MEAL::Steps::calculate (double& result, std::vector<double>* grad)
{
  if (step.size() == 0)
    throw Error (InvalidState, "MEAL::Steps::calculate",
		 "no steps and no free parameters");

  if (current_step < 0)
    throw Error (InvalidState, "MEAL::Steps::calculate",
		 "current step unknown or invalid");

  result = parameters.get_param (current_step);

  if (grad) {
    // set all elements of the gradient equal to zero ...
    grad->resize (get_nparam());
    for (unsigned ig=0; ig<get_nparam(); ig++)
      (*grad)[ig] = 0.0;
    // except the current step, for which dresult/dparam = 1
    (*grad)[current_step] = 1.0;
  }

  if (verbose) {
    cerr << "MEAL::Steps::calculate result\n"
	 "   " << result << endl;
    if (grad) {
      cerr << "MEAL::Steps::calculate gradient" << endl;
      for (unsigned i=0; i<grad->size(); i++)
	cerr << "   " << i << ":" << get_infit(i) << "=" << (*grad)[i] << endl;
    }
  }

}
