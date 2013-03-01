/***************************************************************************
 *
 *   Copyright (C) 2004-2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "MEAL/Tracer.h"
#include "MEAL/Function.h"

using namespace std;

MEAL::Tracer::Tracer ()
{
}

MEAL::Tracer::~Tracer ()
{
  if (Function::verbose)
    cerr << "MEAL::Tracer::~Tracer" << endl;

  if (model)
    model->changed.disconnect (this, &Tracer::attribute_changed);
}

void MEAL::Tracer::watch (Function* _model) try
{
  if (model)
    model->changed.disconnect (this, &Tracer::attribute_changed);

  model = _model;

  if (!model)
    return;

  model->changed.connect (this, &Tracer::attribute_changed);
  report ();
}
catch (Error& error)
{
  throw error += "MEAL::Tracer::watch";
}

//! Method called when a Function attribute has changed
void MEAL::Tracer::attribute_changed (Function::Attribute attribute) try
{
  if (!model)
    throw Error (InvalidState, "MEAL::Tracer::attribute_changed",
		 "method called with no model being watched");

  report ();
}
catch (Error& error)
{
  throw error += "MEAL::Tracer::attribute_changed";
}

