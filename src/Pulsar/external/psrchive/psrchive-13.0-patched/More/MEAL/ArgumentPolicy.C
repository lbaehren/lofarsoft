/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "MEAL/ArgumentPolicy.h"
#include "MEAL/Function.h"

MEAL::ArgumentPolicy::ArgumentPolicy (Function* context)
 : FunctionPolicy (context)
{
  context->argument_policy = this;
}

