//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/VectorRule.h,v $
   $Revision: 1.10 $
   $Date: 2009/06/13 05:09:24 $
   $Author: straten $ */

#ifndef __MEAL_VectorRule_H
#define __MEAL_VectorRule_H

#include "MEAL/ProjectGradient.h"
#include "MEAL/Composite.h"
#include "stringtok.h"

namespace MEAL {

  //! VectorRule implements a vector of Functions with a current index
  template<class T>
  class VectorRule : public T
  {

  public:

    typedef typename T::Result Result;

    //! Default constructor
    VectorRule (Composite* policy = 0);

    //! Copy constructor
    VectorRule (const VectorRule& v);

    //! Assignment operator
    VectorRule& operator = (const VectorRule& copy);

    //! Destructor
    ~VectorRule () { }

    //! Add an element to the array
    void push_back (T* model);

    //! Assign the current element of the array
    void assign (T* model);

    //! Get the current element of the array
    T* get_current ();

    //! Get the projection of the current element of the array
    Project<T>& get_projection ();

    //! Get the size of the array
    unsigned size () const { return model.size(); }

    //! Set the current element of the array
    void set_index (unsigned index);

    //! Get the current element of the array
    unsigned get_index () const { return model_index; }

    //! Return the name of the class
    std::string get_name () const
    { return "VectorRule<" + std::string(T::Name)+ ">"; }

  protected:

    //! Return the result and its gradient
    void calculate (Result& result, std::vector<Result>* grad);

  private:

    //! The models and their mappings
    std::vector< Project<T> > model;

    //! The current index in the array
    unsigned model_index;

    //! Composite parameter policy
    Reference::To<Composite> composite;

  };
  
}

template<class T>
MEAL::VectorRule<T>::VectorRule (Composite* policy)
{
  if (policy)
    composite = policy;
  else
    composite = new Composite (this);

  model_index = 0;
}

//! Copy constructor
template<class T>
MEAL::VectorRule<T>::VectorRule (const VectorRule& v)
{
  composite = new Composite (this);
  operator = (v);
}

template<class T>
MEAL::VectorRule<T>&
MEAL::VectorRule<T>:: operator = (const VectorRule& copy)
{
  if (this == &copy)
    return *this;

  unsigned nmodel = copy.model.size();
  for (unsigned imodel=0; imodel < nmodel; imodel++)
    push_back (copy.model[imodel]);

  model_index = copy.model_index;

  return *this;
}

template<class T>
void MEAL::VectorRule<T>::push_back (T* x)
{
  if (T::very_verbose)
    std::cerr << get_name() + "push_back" << std::endl;

  model_index = model.size();
  model.push_back (Project<T>(x));
  composite->map (model.back());
}

template<class T>
void MEAL::VectorRule<T>::assign (T* x)
{
  if (T::very_verbose)
    std::cerr << get_name() + "assign" << std::endl;

  if (!model.size())
  {
    push_back (x);
    return;
  }

  if (model[model_index])
  {
    if (T::very_verbose)
      std::cerr << get_name() + "assign unmap old" << std::endl;

    composite->unmap (model[model_index]);
  }

  model[model_index] = x;

  if (!x)
    return;

  if (T::very_verbose)
    std::cerr << get_name() + "assign map new" << std::endl;

  composite->map (model[model_index]);
}

template<class T>
T* MEAL::VectorRule<T>::get_current ()
{
  if (model_index >= model.size())
   throw Error (InvalidRange, "MEAL::"+get_name()+"::get_current",
		 "index=%d >= nmodel=%d", model_index, model.size());

  if (!model[model_index])
    throw Error (InvalidState, "MEAL::"+get_name()+"::get_current",
                 "index=%d not set; nmodel=%d", model_index);

  return model[model_index];
}

template<class T>
MEAL::Project<T>& MEAL::VectorRule<T>::get_projection ()
{
  if (model_index >= model.size())
   throw Error (InvalidRange, "MEAL::"+get_name()+"::get_projection",
		 "index=%d >= nmodel=%d", model_index, model.size());

  if (!model[model_index])
    throw Error (InvalidState, "MEAL::"+get_name()+"::get_projection",
                 "index=%d not set; nmodel=%d", model_index);

  return model[model_index];
}

template<class T>
void MEAL::VectorRule<T>::set_index (unsigned index)
{
  if (index == model_index)
    return;

  if (index >= model.size())
    throw Error (InvalidRange, "MEAL::"+get_name()+"::set_index",
		 "index=%d >= nmodel=%d", index, model.size());

  model_index = index;
  composite->attribute_changed( Function::Evaluation );
}

template<class T>
void MEAL::VectorRule<T>::calculate (Result& result,
				     std::vector<Result>* grad)
{
  unsigned nmodel = model.size();
  if (T::very_verbose)
    std::cerr << get_name() + "calculate nmodel=" << nmodel << std::endl;

  // the gradient of each component
  std::vector<Result> comp_gradient;

  // the pointer to the above array, if grad != 0
  std::vector<Result>* comp_gradient_ptr = 0;

  if (grad)
    comp_gradient_ptr = &comp_gradient;

  if (this->get_verbose())
    std::cerr << get_name() + "calculate evaluate " 
	      << model[model_index]->get_name() << std::endl;

  try {

    // evaluate the model and its gradient
    result = model[model_index]->evaluate (comp_gradient_ptr);

    if (this->get_verbose())
      std::cerr << get_name() + "calculate " 
		<< model[model_index]->get_name()
		<< " result=" << result << std::endl;

  }
  catch (Error& error) {
    error += get_name() + "calculate";
    throw error << " model=" << model[model_index]->get_name();
  }
  
  if (grad)
  {
    if (model[model_index]->get_nparam() != comp_gradient.size())
      throw Error (InvalidState, (get_name() + "calculate").c_str(),
		   "model[%d]=%s.get_nparam=%d != gradient.size=%d",
		   model_index, model[model_index]->get_name().c_str(),
		   model[model_index]->get_nparam(), comp_gradient.size());
    
    /* re-map the components of the gradient into the Composite space,
       summing duplicates implements both the sum and product rules. */

    unsigned nparam = this->get_nparam();

    grad->resize (nparam);
    // set each element of the gradient to zero
    for (unsigned iparam=0; iparam<nparam; iparam++)
      (*grad)[iparam] = 0.0;

    // this verion of ProjectGradient initializes the gradient vector to zero
    ProjectGradient (model[model_index], comp_gradient, *grad);
  }

  if (T::very_verbose)
  {
    std::cerr << get_name() + "calculate model index=" << model_index
	      << " result\n   " << result << std::endl;
    if (grad)
    {
      std::cerr << get_name() + "calculate gradient" << std::endl;
      for (unsigned i=0; i<grad->size(); i++)
	std::cerr << "   " << i << ":" << this->get_infit(i) 
		  << "=" << (*grad)[i] << std::endl;
    }
  }
}

#endif

