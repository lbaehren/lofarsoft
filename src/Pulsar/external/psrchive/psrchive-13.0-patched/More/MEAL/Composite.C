/***************************************************************************
 *
 *   Copyright (C) 2004-2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "MEAL/Composite.h"
#include "MEAL/Constant.h"
#include "MEAL/ArgumentPolicyAdapter.h"

#include "Error.h"
#include <assert.h>

using namespace std;

//! Default constructor
MEAL::Composite::Composite (Function* context)
  : ParameterPolicy (context)
{
  new ArgumentPolicyAdapter<Composite> (context, this);

  nparameters = 0;
  current_model = 0;
  current_index = 0;
  remap_needed = false;
  disable_callbacks = false;
}

//! Return the number of parameters
unsigned MEAL::Composite::get_nparam () const
{
  if (remap_needed)
    const_cast<Composite*>(this)->remap ();

  return nparameters;
}

//! Return the name of the specified parameter
string MEAL::Composite::get_name (unsigned index) const
{
  const Function* function = get_Function (index);
  return function->get_param_name (index);
}

//! Return the description of the specified parameter
string MEAL::Composite::get_description (unsigned index) const
{
  const Function* function = get_Function (index);
  return function->get_param_description (index);
}

//! Return the value of the specified parameter
double MEAL::Composite::get_param (unsigned index) const
{
  const Function* function = get_Function (index);
  return function->get_param (index);
}

//! Set the value of the specified parameter
void MEAL::Composite::set_param (unsigned index, double value)
{
  Function* function = get_Function (index);
  function->set_param (index, value);
}

//! Return the variance of the specified parameter
double MEAL::Composite::get_variance (unsigned index) const
{
  const Function* function = get_Function (index);
  return function->get_variance (index);
}

//! Set the variance of the specified parameter
void MEAL::Composite::set_variance (unsigned index, double variance)
{
  Function* function = get_Function (index);
  function->set_variance (index, variance);
}

//! Return true if parameter at index is to be fitted
bool MEAL::Composite::get_infit (unsigned index) const
{
  const Function* function = get_Function (index);
  return function->get_infit (index);
}

//! Set flag for parameter at index to be fitted
void MEAL::Composite::set_infit (unsigned index, bool flag)
{
  Function* function = get_Function (index);
  function->set_infit (index, flag);
}


//! The most common abscissa type needs a simple interface
void MEAL::Composite::set_argument (unsigned dimension, Argument* axis)
{
  if (Function::verbose)
    cerr << "MEAL::Composite::set_argument nmodel=" << models.size() << endl;

  for (unsigned imodel=0; imodel < models.size(); imodel++)
  {
    reference_check (imodel, "set_axis");
    models[imodel] -> set_argument (dimension, axis);
  }
}

// ///////////////////////////////////////////////////////////////////
//
// Add and access parameters
//
// ///////////////////////////////////////////////////////////////////

string MEAL::Composite::class_name() const
{
  return "MEAL::Composite[" + get_context()->get_name() + "]::";
}

//! Get the number of Functions
unsigned MEAL::Composite::get_nmodel () const
{
  if (remap_needed)
    const_cast<Composite*>(this)->remap ();
  return models.size ();
}


void MEAL::Composite::map (Projection* modelmap)
{
#ifdef _DEBUG
  cerr << class_name() + "map (Projection* = " << modelmap << ")" << endl;
#endif

  if (!modelmap)
    throw Error (InvalidState, class_name() + "map", "null Projection");

  bool already_mapped = false;
  
  if (modelmap->meta)
  {
    if (modelmap->meta != this)
      throw Error (InvalidState, class_name() + "map",
                   "Projection already mapped into another Composite");

    unsigned imap = find_Projection (modelmap);
    if (imap < maps.size())
      already_mapped = true;
    else
      throw Error (InvalidState, class_name() + "map",
		   "Projection partially mapped into this");   
  }
  
  try
  {
    Function* model = modelmap->get_Function();

    modelmap->imap.resize(0);
    add_component (model, modelmap->imap);

    if (!component_shares_this)
    {
      if (modelmap->imap.size() != model->get_nparam())
	throw Error (InvalidState, class_name() + "map",
		     "map size=%d != nparam=%d",
		     modelmap->imap.size(), model->get_nparam());
      
      if (Function::very_verbose)
      {
	cerr << class_name() + "map Function maps into" << endl;
	for (unsigned i=0; i<modelmap->imap.size(); i++)
	  cerr << "   " << i << ":" << modelmap->imap[i] << endl;
      }
    }

    if (Function::very_verbose)
      cerr << class_name() + "map connect to changed callback model="
	   << model << endl;
      
    model->changed.connect (this, &Composite::attribute_changed);

    if (!already_mapped)
    {
      if (Function::very_verbose)
	cerr << class_name() + "map new Projection" << endl;
      maps.push_back (modelmap);
      modelmap->meta = this;
    }

    callbacks ();

  }
  catch (Error& error) {
    throw error += class_name() + "map";
  }

}



//! Map the Function indeces
void MEAL::Composite::add_component (Function* model, vector<unsigned>& imap)
{
  if (!model)
    return;

  if (Function::very_verbose)
    cerr << class_name() + "add_component [" <<model->get_name()<< "]" << endl;

  component_shares_this = false;

  const Constant* constant = 0;
  constant = dynamic_cast<const Constant*>(model->get_parameter_policy());

  if (constant)
  {
    if (Function::very_verbose)
      cerr << class_name() + "add_component Constant" << endl;
    return;
  }

  const Composite* meta = 0;
  meta = dynamic_cast<const Composite*>(model->get_parameter_policy());

  if (meta)
  {
    if (meta == this)
    {
      if (Function::very_verbose)
	cerr << class_name() + "add_component [" << model->get_name() << "]" 
	  " this is shared" << endl;

      component_shares_this = true;
      return;
    }

    if (Function::very_verbose)
      cerr << class_name() + "add_component Composite" << endl;

    unsigned nmodel = meta->get_nmodel();
    for (unsigned imodel=0; imodel<nmodel; imodel++)
      add_component (meta->models[imodel], imap);

    if (imap.size() != meta->get_nparam())
      meta->recount();
  }
  else
  {   
    if (Function::very_verbose)
      cerr << class_name() + "add_component nmodel=" << models.size() << endl;

    unsigned iparam = 0;
    unsigned imodel = 0;

    for(; imodel < models.size(); imodel++)
    {
      reference_check (imodel, "add_component");
      
      if (models[imodel].ptr() == model)
        break;
      
      iparam += models[imodel]->get_nparam();
    }

    unsigned nparam = model->get_nparam();

    // add the mapped indeces (works for both cases: new model or old model)
    for (unsigned jparam=0; jparam < nparam; jparam++)
      imap.push_back(iparam + jparam);

    // the model exists in the current list
    if (imodel < models.size())
      return;

    if (Function::very_verbose)
      cerr << class_name() + "add_component add new Function" << endl;

    // add the new model
    nparameters += model->get_nparam();
    models.push_back (model);
  }
}

void MEAL::Composite::unmap (Projection* modelmap)
{
  if (!(modelmap->meta))
    throw Error (InvalidParam, class_name() + "unmap",
		 "Undefined projection");
  
  if ((modelmap->meta).get() != this)
    throw Error (InvalidParam, class_name() + "unmap",
		 "Projection not mapped into this");
  
  // erase the mapping
  unsigned imap = find_Projection (modelmap);
  if (imap == maps.size())
  {
    //if (Function::very_verbose)
      cerr << class_name() + "unmap Projection not found" << endl;
    return;
  }
  maps.erase (maps.begin()+imap);

  // remove the component
  Function* model = modelmap->get_Function();
  remove_component (model);

  // disconnect the callback
  model->changed.disconnect (this, &Composite::attribute_changed);

  // flag the mapping as unmanaged
  modelmap->meta = 0;

  // remap later
  remap_later ();
}


//! Remove a Function from the list
void MEAL::Composite::remove_component (Function* model)
{
  if (Function::very_verbose)
    cerr << class_name() + "remove_component" << endl;

  const Constant* constant = 0;
  constant = dynamic_cast<const Constant*>(model->get_parameter_policy());

  if (constant)
  {
    if (Function::very_verbose)
      cerr << class_name() + "remove_component no need to remove Constant"
	   << endl;
    return;
  }

  const Composite* meta = 0;
  meta = dynamic_cast<const Composite*>(model->get_parameter_policy());

  if (meta)
  {
    if (Function::very_verbose)
      cerr << class_name() + "remove_component remove Composite" << endl;

    unsigned nmodel = meta->get_nmodel();
    for (unsigned imodel=0; imodel<nmodel; imodel++)
      remove_component (meta->models[imodel]);
  }
  else
  {   
    unsigned imodel = find_Function (model);

    if (Function::very_verbose)
      cerr << class_name() + "remove_component imodel=" << imodel << endl;

    if (imodel >= models.size())
    {
      if (Function::very_verbose)
	cerr << class_name() + "remove_component function not found" << endl;
      return;
    }

    reference_check (imodel, "remove_component");

    nparameters -= model->get_nparam();
    models.erase (models.begin()+imodel);
  }
}

void MEAL::Composite::recount () const
{
  nparameters = 0;
  for (unsigned imodel=0; imodel < models.size(); imodel++)
  {
    reference_check (imodel, "recount");
    nparameters += models[imodel]->get_nparam();
  }
}

//! Recount the number of parameters
void MEAL::Composite::remap ()
{ 
  if (Function::very_verbose)
    cerr << class_name() << "remap remove indirect Function instances" << endl;

  models.resize (0);

  if (Function::very_verbose)
    cerr << class_name() << "remap remap Projection instances" << endl;

  disable_callbacks = true;

  try
  {
    for (unsigned imap=0; imap < maps.size(); imap++)
      map (maps[imap]);
  }
  catch (Error& error)
  {
    throw error += class_name() + "remap";
  }

  if (Function::very_verbose)
    cerr << class_name() << "remap recount parameters" << endl;

  recount ();

  if (Function::very_verbose)
    cerr << class_name() << "remap nparam=" << nparameters << endl;

  disable_callbacks = false;
  remap_needed = false;
}

void MEAL::Composite::remap_later ()
{
  if (Function::very_verbose)
    cerr << class_name() << "remap_later" << endl;

  if (remap_needed)
    return;

  remap_needed = true;

  callbacks ();
}

void MEAL::Composite::callbacks ()
{
  if (disable_callbacks)
    return;

  if (Function::very_verbose) 
    cerr << class_name() + "callbacks send changed ParameterCount" << endl;

  get_context()->changed.send (Function::ParameterCount);

  if (Function::very_verbose) 
    cerr << class_name() + "callbacks set_evaluation_changed" << endl;

  get_context()->set_evaluation_changed ();
}

void MEAL::Composite::attribute_changed (Function::Attribute attribute) 
{
  if (Function::very_verbose)
    cerr << class_name() << "attribute_changed" << endl;

  if (attribute == Function::ParameterCount)
  {
    if (Function::very_verbose)
      cerr << class_name() << "attribute_changed remap" << endl;
    remap_later ();
  }

  if (attribute == Function::Evaluation)
  {
    if (Function::very_verbose)
      cerr << class_name() << "attribute_changed set_evaluation_changed"
	   << endl;

    get_context()->set_evaluation_changed ();
  }
}

//! Get the const Function that corresponds to the given index
const MEAL::Function*
MEAL::Composite::get_Function (unsigned& index) const
{
  return const_cast<Composite*>(this)->get_Function (index);
}

//! Get the Function that corresponds to the given index
MEAL::Function* MEAL::Composite::get_Function (unsigned& index)
{
  if (remap_needed)
    remap ();

  unsigned imodel = current_model;
  
  if (index < current_index)
  {
    current_index = 0;
    imodel = 0;
  }
  else
    index -= current_index;
  
  while (imodel < models.size())
  {    
    reference_check (imodel, "get_Function");

    unsigned nparam = models[imodel]->get_nparam();
    
    if (index < nparam)
    {
      current_model = imodel;
      return models[imodel];
    }
    
    index -= nparam;
    current_index += nparam;
    imodel ++;
  }

  throw Error (InvalidRange, class_name() + "get_Function",
	       "index=%d > nparam=%d", index, get_nparam());
}


//! Check the the reference to the specified model is still valid
void MEAL::Composite::reference_check (unsigned i, char* method) const
{
  if (!models[i])
    throw Error (InvalidState, class_name() + method, 
		 "model[%d] is an invalid reference", i);
}


//! Return the index for the specified model
unsigned MEAL::Composite::find_Function (Function* model) const
{
  if (Function::very_verbose) cerr << class_name() + "find_Function nmodel="
			 << models.size() << endl;

  for (unsigned imodel=0; imodel < models.size(); imodel++)
  {
    reference_check (imodel, "find_Function");
    if (models[imodel] && models[imodel].get() == model)
      return imodel;
  }
  return models.size();
}

//! Return the index for the specified map
unsigned MEAL::Composite::find_Projection (Projection* modelmap) const
{
  if (Function::very_verbose) cerr << class_name() + "find_Projection nmap="
				   << maps.size() << endl;

  for (unsigned imap=0; imap < maps.size(); imap++)
  {
    if (maps[imap] && maps[imap].get() == modelmap)
      return imap;
  }
  
  return maps.size();
}

//! Map the Function indeces
/*! If you haven't added the Function using a Projection, but still
  want to know its mapping, call this function. */

void MEAL::Composite::get_imap (const Function* model,
				vector<unsigned>::iterator& imap) const
{
  if (!model)
    return;

  const Constant* constant = 0;
  constant = dynamic_cast<const Constant*>(model->get_parameter_policy());

  if (constant)
    return;

  if (remap_needed)
    const_cast<Composite*>(this)->remap ();

  const Composite* meta = 0;
  meta = dynamic_cast<const Composite*>(model->get_parameter_policy());

  if (meta)
  {
    unsigned nmodel = meta->get_nmodel();
    for (unsigned imodel=0; imodel<nmodel; imodel++)
      get_imap (meta->models[imodel], imap);
    return;
  }

  unsigned iparam = 0;
  unsigned imodel = 0;

  for (imodel = 0; imodel < models.size(); imodel++)
  {  
    reference_check (imodel, "get_imap");
    if (models[imodel].ptr() == model)
      break;
    iparam += models[imodel]->get_nparam();
  }

  // the model was not found in the list
  if (imodel == models.size())
    throw Error (InvalidParam, "MEAL::Composite::get_imap",
		 model->get_name() + " is not mapped by this Composite");
  
  unsigned nparam = model->get_nparam();

  // add the mapped indeces (works for both cases: new model or old model)
  for (unsigned jparam=0; jparam < nparam; jparam++)
  {
    *imap = iparam + jparam;
    imap ++;
  }
}

void MEAL::get_imap (const Function* composite,
		     const Function* component,
		     std::vector<unsigned>& imap)
{
  const Composite* policy = 0;
  policy = dynamic_cast<const Composite*>(composite->get_parameter_policy());

  if (!policy)
    throw Error (InvalidParam, "MEAL::get_imap",
		 composite->get_name() + " is not a Composite");

  imap.resize( component->get_nparam() );
  vector<unsigned>::iterator imapi = imap.begin();
  policy->get_imap( component, imapi );

  assert ( imapi == imap.end() );
}

