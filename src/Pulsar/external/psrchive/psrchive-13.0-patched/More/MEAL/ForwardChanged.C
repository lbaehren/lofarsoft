/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "MEAL/ForwardChanged.h"

#include <iostream>
using namespace std;


MEAL::ForwardChanged::ForwardChanged (Function* context)
  : FunctionPolicy (context)
{
}

void MEAL::ForwardChanged::manage (Function* model)
{
  model->changed.connect (this, &ForwardChanged::attribute_changed);
}

void MEAL::ForwardChanged::unmanage (Function* model)
{
  model->changed.disconnect (this, &ForwardChanged::attribute_changed);
}

void MEAL::ForwardChanged::attribute_changed (Function::Attribute attribute) 
{
  if (attribute == Function::Evaluation) {
    if (Function::very_verbose)
      cerr << "ForwardChanged::attribute_changed Evaluation" << endl;
    get_context()->set_evaluation_changed ();
    return;
  } 

  if (Function::very_verbose)
    cerr << "ForwardChanged::attribute_changed" << endl;

  get_context()->changed.send (attribute);
}
