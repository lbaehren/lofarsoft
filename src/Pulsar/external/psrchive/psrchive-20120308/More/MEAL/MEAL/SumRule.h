//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/SumRule.h,v $
   $Revision: 1.6 $
   $Date: 2006/10/06 21:13:54 $
   $Author: straten $ */

#ifndef __SumRule_H
#define __SumRule_H

#include "MEAL/GroupRule.h"

namespace MEAL {

  //! Represents a sum of models
  template<class T>
  class SumRule : public GroupRule<T>
  {

  public:

    typedef typename T::Result Result;

    //! Add the model to the sum
    void operator += (T* model) { add_model (model); }

    // ///////////////////////////////////////////////////////////////////
    //
    // Function implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Return the name of the class
    std::string get_name () const
    { return "SumRule<" + std::string(T::Name)+ ">"; }

  protected:
    
    // ///////////////////////////////////////////////////////////////////
    //
    // GroupRule implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Return the identity of the addition group (zero)
    const Result get_identity () const { return 0.0; }

    //! Add the element to the total
    void operate (Result& total, const Result& element) { total += element; }

    //! Neighbouring terms do not affect each other's partial derivatives
    const Result partial (const Result& element) const { return 0; }

  };
  
}


#endif

