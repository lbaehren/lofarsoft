//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/Univariate.h,v $
   $Revision: 1.9 $
   $Date: 2009/06/12 09:10:26 $
   $Author: straten $ */

#ifndef __MEAL_Univariate_H
#define __MEAL_Univariate_H

#include "MEAL/UnivariatePolicy.h"
#include "Reference.h"

namespace MEAL {

  //! Abstract template base class of univariate Function implementations
  template<class T>
  class Univariate : public T {

  public:

    //! Default constructor
    Univariate ()
    { set_univariate_policy ( new UnivariatePolicy (this) ); }

    //! Copy constructor
    Univariate (const Univariate& copy) : T (copy) { }

    //! Assignment operator
    Univariate& operator = (const Univariate& copy);

    //! Clone
    Univariate<T>* clone () const
    { throw Error (InvalidState, "Univariate<T>::clone", "not implemented"); }

    //! Set the abscissa value
    virtual void set_abscissa (double value)
    { univariate_policy->set_abscissa(value); }

    //! Get the abscissa value
    double get_abscissa () const
    { return univariate_policy->get_abscissa (); }

    //! Evaluate the function
    typename T::Result compute (double x)
    { set_abscissa (x); return this->evaluate (); }

  protected:

    void set_univariate_policy (UnivariatePolicy* policy)
    { this->argument_policy = univariate_policy = policy; }

    UnivariatePolicy* get_univariate_policy () const
    { return univariate_policy; }

    //! Copy the parameter policy of another instance
    void copy_univariate_policy (const Univariate* other)
    { set_univariate_policy (other->get_univariate_policy()); }

  private:

    //! The univariate implementation
    Reference::To<UnivariatePolicy> univariate_policy;

  };

}

template<class T>
MEAL::Univariate<T>& MEAL::Univariate<T>::operator = (const Univariate& copy)
{ 
  if (&copy != this) {
    set_abscissa( copy.get_abscissa() ); 
    T::operator=( copy );
  }
  return *this;
}

#endif

