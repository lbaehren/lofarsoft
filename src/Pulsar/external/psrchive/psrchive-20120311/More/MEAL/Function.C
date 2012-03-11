/***************************************************************************
 *
 *   Copyright (C) 2004-2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "MEAL/Function.h"
#include "MEAL/FunctionInterface.h"

bool MEAL::Function::verbose = false;
bool MEAL::Function::very_verbose = false;
bool MEAL::Function::check_zero = false;
bool MEAL::Function::check_variance = false;
bool MEAL::Function::cache_results = false;

void MEAL::Function::init ()
{
  set_parameter_policy_context = true;
  evaluation_changed = true;
  this_verbose = false;
} 

MEAL::Function::Function ()
{
#ifdef _DEBUG
  cerr << "MEAL::Function default constructor" << endl;
#endif
  init ();
}

MEAL::Function::Function (const Function& model)
{
#ifdef _DEBUG
  cerr << "MEAL::Function copy constructor" << endl;
#endif
  init ();

  parameter_policy = model.parameter_policy->clone (this);

  if (model.argument_policy)
    argument_policy = model.argument_policy->clone (this);
}

MEAL::Function::~Function ()
{
#ifdef _DEBUG
  cerr << "MEAL::Function::~Function" << endl;
#endif
}

MEAL::Function& MEAL::Function::operator = (const Function& model)
{
  copy (&model);
  return *this;
}

void MEAL::Function::copy (const Function* model)
{
#ifdef _DEBUG
  cerr << "MEAL::Function::copy" << endl;
#endif

  if (this == model)
    return;

  unsigned nparam = get_nparam ();

  if (nparam != model->get_nparam())
    throw Error (InvalidParam, "MEAL::Function::operator =",
		 "nparam=%d != copy.nparam=%d", nparam, model->get_nparam());

  for (unsigned iparam=0; iparam<nparam; iparam++) {

    set_param( iparam, model->get_param(iparam) );
    set_variance( iparam, model->get_variance(iparam) );
    set_infit( iparam, model->get_infit(iparam) );

  }

  // name = model->name;

  set_evaluation_changed();
}

//! Clone
MEAL::Function* MEAL::Function::clone () const
{
  throw Error (InvalidState, "MEAL::Function::clone", "not implemented" );
}

void MEAL::Function::copy_parameter_policy (const Function* function)
{
  parameter_policy = function->parameter_policy;
}

void MEAL::Function::set_parameter_policy (ParameterPolicy* policy)
{
  parameter_policy = policy;
  if (set_parameter_policy_context)
    parameter_policy->context = this;
}

Estimate<double> MEAL::Function::get_Estimate (unsigned index) const
{
  return Estimate<double> ( get_param(index), get_variance(index) );
}

void MEAL::Function::set_Estimate (unsigned index,
				   const Estimate<double>& estimate)
{
  set_param( index, estimate.val );
  set_variance( index, estimate.var );
}

//! Set the verbosity of this instance
void MEAL::Function::set_verbose (bool flag)
{
  this_verbose = flag;
}

//! Get the verbosity of this instance
bool MEAL::Function::get_verbose () const
{
  return this_verbose || verbose;
}

//! Set true if the Function evaluation has changed
void MEAL::Function::set_evaluation_changed (bool _changed) 
{
  if (!evaluation_changed && _changed && cache_results)
  {
    if (very_verbose)
      std::cerr << "MEAL::Function::set_evaluation_changed issuing callback" 
		<< std::endl;

    evaluation_changed = _changed;
    changed.send (Evaluation);
  }
  else if (evaluation_changed && _changed && very_verbose)
    std::cerr << "MEAL::Function::set_evaluation_changed already changed ptr="
	      << this << std::endl;

  evaluation_changed = _changed;
}

//! Return a text interface that can be used to access this instance
TextInterface::Parser* MEAL::Function::get_interface ()
{
  return new Interface (this);
}
