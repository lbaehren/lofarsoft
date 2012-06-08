/***************************************************************************
 *
 *   Copyright (C) 2004-2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "MEAL/ParameterTracer.h"
#include "MEAL/Function.h"

using namespace std;

MEAL::ParameterTracer::ParameterTracer (Function* _model, unsigned param)
{
  parameter = param;
  current_value = 0.0;

  if (_model)
    watch (_model, param);
}

void MEAL::ParameterTracer::watch (Function* _model, unsigned param) try
{
  parameter = param;
  Tracer::watch (_model);
}
catch (Error& error)
{
  throw error += "MEAL::ParameterTracer::watch";
}

void MEAL::ParameterTracer::report () try
{
  double latest = model->get_param (parameter);

  if (latest == current_value)
    return;

  current_value = latest;

  cerr << "MEAL::ParameterTracer " << model->get_name() << " "
       << model->get_param_name(parameter) << " " 
       << model->get_param(parameter) << endl;
}
catch (Error& error)
{
  throw error += "MEAL::ParameterTracer::report";
}

