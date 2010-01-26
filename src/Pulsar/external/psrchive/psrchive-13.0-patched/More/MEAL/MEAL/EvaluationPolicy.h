//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/EvaluationPolicy.h,v $
   $Revision: 1.5 $
   $Date: 2008/06/15 16:12:34 $
   $Author: straten $ */

#ifndef __MEAL_EvaluationPolicy_H
#define __MEAL_EvaluationPolicy_H

#include "MEAL/FunctionPolicy.h"
#include <vector>

namespace MEAL {

  //! Managers of Function value and gradient
  template<class Result>
  class EvaluationPolicy : public FunctionPolicy {

  public:

    //! Default destructor
    EvaluationPolicy (Function* context) : FunctionPolicy (context) { }

    //! Return the result (and its gradient, if requested)
    virtual Result evaluate (std::vector<Result>* grad=0) const = 0;

  };

  //! Return the default evaluation policy
  template<typename T>
  static EvaluationPolicy<typename T::Result>* default_evaluation_policy (T*);

}

#include "MEAL/Cached.h"
#include "MEAL/NotCached.h"

template<typename T>
MEAL::EvaluationPolicy<typename T::Result>*
MEAL::default_evaluation_policy (T* thiz)
{
  if (Function::cache_results)
    return new Cached<T> (thiz);
  else
    return new NotCached<T> (thiz);
}

#endif
