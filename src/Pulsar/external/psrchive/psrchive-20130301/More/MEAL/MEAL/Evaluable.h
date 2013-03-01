//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/Evaluable.h,v $
   $Revision: 1.2 $
   $Date: 2009/06/06 11:07:20 $
   $Author: straten $ */

#ifndef __MEAL_Evaluable_H
#define __MEAL_Evaluable_H

#include "MEAL/Function.h"
#include "MEAL/EvaluationPolicy.h"
#include "Estimate.h"

//! Works for most scalar types
template<typename T>
struct EstimateTraits {
  typedef Estimate<T> type;
};

template<typename T>
void add_variance (typename EstimateTraits<T>::type& result,
		   double var, const T& grad)
{
  result.var += var * grad * grad;
}

namespace MEAL {

  //! Template base class of functions with an evaluate method
  template<typename T>
  class Evaluable : public Function {

  public:

    //! The return type of the evaluate method
    typedef T Result;

    //! Default constructor
    Evaluable ()
    {
      evaluation_policy = default_evaluation_policy (this);
    }

    //! Copy constructor
    Evaluable (const Evaluable& copy) : Function (copy)
    {
      evaluation_policy = default_evaluation_policy (this);
    }

    //! Assignment operator
    Evaluable& operator = (const Evaluable& copy)
    {
      Function::operator = (copy);
      return *this;
    }

    //! Return the Jones matrix and its gradient
    T evaluate (std::vector<T>* grad=0) const
    { 
      return evaluation_policy->evaluate (grad);
    }

    //! Return the Jones Estimate matrix
    typename EstimateTraits<T>::type estimate () const
    {
      std::vector< T > gradient;
      typename EstimateTraits<T>::type result = evaluate (&gradient);

      const unsigned nparam = get_nparam();

      if (gradient.size() != nparam)
	throw Error (InvalidState, "MEAL::Evaluable::evaluate",
		     "gradient.size=%d != nparam=%d", gradient.size(), nparam);

      for (unsigned iparam=0; iparam<nparam; iparam++)
	add_variance( result, get_variance(iparam), gradient[iparam]);

      return result;
    }

  protected:

    template<class P> friend class CalculatePolicy;

    //! Calculate the complex value and its gradient
    virtual void calculate (T& result, std::vector<T>*) = 0;

    //! The policy for managing function evaluation
    Reference::To< EvaluationPolicy< T > > evaluation_policy;

    void copy_evaluation_policy (const Evaluable* other)
    {
      evaluation_policy = other->evaluation_policy;
    }

    //! Use the calculate method of another Evaluable instance
    void calculate (Evaluable* eval, T& result, std::vector<T>* grad)
    {
      eval->calculate (result, grad);
    }

  };

}

#endif
