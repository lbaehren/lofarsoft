//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/GroupRule.h,v $
   $Revision: 1.10 $
   $Date: 2009/06/13 05:09:24 $
   $Author: straten $ */

#ifndef __GroupRule_H
#define __GroupRule_H

#include "MEAL/ProjectGradient.h"
#include "MEAL/Composite.h"
#include "stringtok.h"

namespace MEAL {

  //! Abstract base class of closed, associative, binary operators
  /*! Because the binary operation is associative, this class is
    implemented as a series of elements; that is, an arbitrary number
    of models can be added.  By inheriting this class and defining the
    get_identity(), operate(), and partial() pure virtual methods,
    derived classes may define the closed, associative binary
    operation, such as the product or sum. */

  template<class T>
  class GroupRule : public T
  {

  public:

    typedef typename T::Result Result;

    //! Default constructor
    GroupRule () : composite(this) { }

    //! Copy constructor
    GroupRule (const GroupRule& meta) : composite(this) { operator = (meta); }

    //! Assignment operator
    GroupRule& operator = (const GroupRule& meta);

    //! Destructor
    ~GroupRule () { }

    //! Add an element to the result
    void add_model (T* model);

    // ///////////////////////////////////////////////////////////////////
    //
    // Function implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Parse the values of model parameters and fit flags from a string
    void parse (const std::string& text);

    // ///////////////////////////////////////////////////////////////////
    //
    // Composite template implementation
    //
    // ///////////////////////////////////////////////////////////////////

    std::string class_name() const
    { return "MEAL::GroupRule[" + this->get_name() + "]::"; }

  protected:

    // ///////////////////////////////////////////////////////////////////
    //
    // Function implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Prints the values of model parameters and fit flags to a string
    void print_parameters (std::string& text, const std::string& sep) const;

    //! Return the result and its gradient
    void calculate (Result& result, std::vector<Result>* grad);

    //! Return the group identity
    virtual const Result get_identity () const = 0;

    //! Set the total equal to "total group operation element"
    virtual void operate (Result& total, const Result& element) = 0;

    //! Neighbouring terms stay in each other's partial derivatives
    virtual const Result partial (const Result& element) const = 0;

  private:

    //! The models and their mappings
    std::vector< Project<T> > model;

    //! The result
    Result result;

    //! The gradient
    std::vector<Result> gradient;

    //! Initialize the result and gradient attributes
    void initialize ();

    //! Composite parameter policy
    Composite composite;

  };
  
}


template<class T>
MEAL::GroupRule<T>&
MEAL::GroupRule<T>:: operator = (const GroupRule& meta)
{
  if (this == &meta)
    return *this;

  unsigned nmodel = meta.model.size();
  for (unsigned imodel=0; imodel < nmodel; imodel++)
    add_model (meta.model[imodel]);

  // name = meta.name;

  return *this;
}

template<class T>
void MEAL::GroupRule<T>::print_parameters (std::string& text,
					   const std::string& sep) const
{
  unsigned nmodel = model.size();
  for (unsigned imodel=0; imodel < nmodel; imodel++) {
    text += sep + model[imodel]->get_name ();
    model[imodel]->print_parameters (text, sep + " ");
  }
}

template<class T>
void MEAL::GroupRule<T>::parse (const std::string& line)
{
  if (model.size()) try {
    model.back()->parse(line);
    return;
  }
  catch (Error& error) {
  }

  // the key should be the name of a new class to be added
  std::string temp = line;
  std::string key = stringtok (temp, " \t");

  if (this->get_verbose())
    std::cerr << class_name() << "::parse key '" << key << "'" << std::endl;

  Function* model = Function::factory (key);

  T* mtype = dynamic_cast<T*>(model);
  if (!mtype)
    throw Error (InvalidParam, class_name()+"parse",
		 model->get_name() + " is not of type " + 
                 std::string(T::Name));

  add_model (mtype);
}

template<class T>
void MEAL::GroupRule<T>::add_model (T* x)
{
  model.push_back (Project<T>(x));
  composite.map (model.back());

  if (this->get_verbose())
    std::cerr << class_name() + "add_model size=" << model.size() << std::endl;
}

template<class T>
void MEAL::GroupRule<T>::initialize ()
{
  result = get_identity();

  for (unsigned jgrad=0; jgrad<gradient.size(); jgrad++)
    gradient[jgrad] = get_identity();
}

template<class T>
void MEAL::GroupRule<T>::calculate (Result& retval,
				    std::vector<Result>* grad)
{
  unsigned nmodel = model.size();

  if (this->get_verbose())
    std::cerr << class_name() + "calculate nmodel=" << nmodel << std::endl;

  // the result of each component
  Result comp_result;

  // the gradient of each component
  std::vector<Result> comp_gradient;

  // the pointer to the above array, if grad != 0
  std::vector<Result>* comp_gradient_ptr = 0;
  
  unsigned total_nparam = 0;

  if (grad)
  {
    for (unsigned imodel=0; imodel < nmodel; imodel++)
      total_nparam += model[imodel]->get_nparam();

    comp_gradient_ptr = &comp_gradient;
    gradient.resize (total_nparam);
  }

  // initialize the result and gradient attributes
  initialize ();

  unsigned igradient = 0;

  for (unsigned imodel=0; imodel < nmodel; imodel++)
  {
    if (this->get_verbose()) 
      std::cerr << class_name() + "calculate evaluate " 
	        << model[imodel]->get_name() << std::endl;

    try {

      // evaluate the model and its gradient
      comp_result = model[imodel]->evaluate (comp_gradient_ptr);

      if (this->get_verbose()) 
	std::cerr << class_name() + "calculate " 
		  << model[imodel]->get_name() 
		  << " result=" << comp_result << std::endl;

      operate( result, comp_result );
      
      if (grad)
      {
	unsigned jgrad;
	unsigned ngrad = comp_gradient_ptr->size();

	for (jgrad=0; jgrad<igradient; jgrad++)
	  operate( gradient[jgrad], partial(comp_result) );
	
	for (jgrad=0; jgrad<ngrad; jgrad++)
	  operate( gradient[igradient + jgrad], (*comp_gradient_ptr)[jgrad] );
	
	for (jgrad=igradient+ngrad; jgrad<gradient.size(); jgrad++)
	  operate( gradient[jgrad], partial(comp_result) );
      }
    }
    catch (Error& error)
    {
      error += class_name() + "calculate";
      throw error << " model=" << model[imodel]->get_name();
    }

    if (grad)
    {
      if (model[imodel]->get_nparam() != comp_gradient.size())
	throw Error (InvalidState, (class_name() + "calculate").c_str(),
		     "model[%d]=%s.get_nparam=%d != gradient.size=%d",
		     imodel, model[imodel]->get_name().c_str(),
		     model[imodel]->get_nparam(), comp_gradient.size());
      
      igradient += comp_gradient.size();
    }
  }

  retval = result;

  if (grad)
  {
    /* re-map the components of the gradient into the Composite space,
       summing duplicates implements both the sum and product rules. */

    // sanity check, ensure that all elements have been set
    if (igradient != total_nparam)
      throw Error (InvalidState, (class_name() + "calculate").c_str(),
		   "after calculation igrad=%d != total_nparam=%d",
		   igradient, total_nparam);

    grad->resize (this->get_nparam());

    // this verion of ProjectGradient initializes the gradient vector to zero
    ProjectGradient (model, gradient, *grad);
  }

  if (this->get_verbose())
  {
    std::cerr << class_name() + "calculate result\n   " << retval << std::endl;
    if (grad)
    {
      std::cerr << class_name() + "calculate gradient" << std::endl;
      for (unsigned i=0; i<grad->size(); i++)
	std::cerr << "   " << i << ":" << this->get_infit(i) 
		  << "=" << (*grad)[i] << std::endl;
    }
  }
}

#endif

