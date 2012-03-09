//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/CalculatePolicy.h,v $
   $Revision: 1.3 $
   $Date: 2006/10/06 21:13:53 $
   $Author: straten $ */

#ifndef __MEAL_CalculatePolicy_H
#define __MEAL_CalculatePolicy_H

#include "MEAL/EvaluationPolicy.h"

namespace MEAL {

  //! Abstract base class of Function parameter policies
  template<class T>
  class CalculatePolicy : public EvaluationPolicy<typename T::Result> {

    typedef typename T::Result Result;

  public:

    //! Default destructor
    CalculatePolicy (T* context) : EvaluationPolicy<Result> (context) 
    { calculate_context = context; }

    void calculate (Result& r, std::vector<Result>* grad) const
    { calculate_context->calculate (r, grad); }

  protected:

    //! The object that implements a calculate method
    T* calculate_context;

  };

}

#endif
