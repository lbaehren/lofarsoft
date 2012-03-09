//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/ForwardResult.h,v $
   $Revision: 1.3 $
   $Date: 2006/10/06 21:13:53 $
   $Author: straten $ */

#ifndef __ForwardResult_H
#define __ForwardResult_H

#include "MEAL/CalculatePolicy.h"
#include "MEAL/ForwardChanged.h"
#include <iostream>

namespace MEAL {
  
  //! This evaluation policy forwards the result of another function

  template<class T>
  class ForwardResult : public CalculatePolicy<T>  {

  public:

    typedef typename T::Result Result;

    //! Default constructor
    ForwardResult (T* context, T* function) 
      : CalculatePolicy<T> (context),
	forward_changed (context)
    {
      evaluator = function;
      forward_changed.manage (function);
    }

    //! Calls to context->evaluate are forwarded to evaluator
    Result evaluate (std::vector<Result>* gradient = 0) const
    {
      if (Function::very_verbose)
	std::cerr << class_name() + "evaluate" << std::endl;

      if (gradient)
	gradient->resize (0);

      return evaluator->evaluate();
    }

    //! Return the name of the class for debugging
    std::string class_name() const
    { return "MEAL::ForwardResult[" + this->get_context()->get_name() + 
	"->" + evaluator->get_name() + "]::"; }

  protected:

    //! The class that will do the evaluating
    Reference::To<T,false> evaluator;

    //! Forward the changed signals from the evaluator to the context
    ForwardChanged forward_changed;

  };

}

#endif
