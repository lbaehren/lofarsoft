/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "MEAL/ParameterPolicy.h"
#include "MEAL/Function.h"

/*! By default, ParameterPolicy derived instances are installed in
  the context during construction. */
bool MEAL::ParameterPolicy::auto_install = true;

MEAL::ParameterPolicy::ParameterPolicy (Function* context) :
 FunctionPolicy (context)
{
  if (auto_install && context)
    context->parameter_policy = this;
}
