//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/Transformation.h,v $
   $Revision: 1.1 $
   $Date: 2008/06/15 14:36:46 $
   $Author: straten $ */

#ifndef __MEAL_Transformation_H
#define __MEAL_Transformation_H

#include "MEAL/Composite.h"

namespace MEAL {

  //! Base class of transformations

  template<typename T>
  class Transformation : public T
  {

  public:

    //! Default constructor
    Transformation ();

    //! Destructor
    ~Transformation () { }

    //! Set the input, \f$ \rho \f$
    virtual void set_input (T* input);

    //! Get the input, \f$ \rho \f$
    virtual T* get_input () { return input; }

    //! Optimization: set composite that contains this transformation
    virtual void set_composite (Composite* c);

    //! Optimization: set input already mapped into containing composite
    virtual void set_input (Project<T>& _input) { input = _input; }

  protected:

    //! The input, \f$ \rho \f$
    Project<T> input;

    //! Composite parameter policy
    Reference::To<Composite> composite;

  };

}

template<typename T>
MEAL::Transformation<T>::Transformation ()
{
  composite = new Composite(this);
  this->set_parameter_policy_context = false;
}

/*! This method unmaps the old input before mapping xform */
template<typename T>
void MEAL::Transformation<T>::set_input (T* _input) try
{
  if (!_input)
    return;

  if (input)
  {
    if (T::verbose)
      std::cerr << "MEAL::Transformation::set_input unmap old input"
		<< std::endl;

    composite->unmap (input);
  }

  input = _input;

  if (T::verbose)
    std::cerr << "MEAL::Transformation::set_input map new input" 
	      << std::endl;

  composite->map (input);
}
catch (Error& error)
{
  throw error += "MEAL::Transformation[" + this->get_name() + "]::set_input";
}

template<typename T>
void MEAL::Transformation<T>::set_composite (Composite* c)
{
  composite = c;
  this->set_parameter_policy (c);
}

#endif

