//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/ArgumentPolicyAdapter.h,v $
   $Revision: 1.3 $
   $Date: 2006/10/06 21:13:53 $
   $Author: straten $ */

#ifndef __MEAL_ArgumentPolicyAdapter_H
#define __MEAL_ArgumentPolicyAdapter_H

#include "MEAL/ArgumentPolicy.h"

namespace MEAL {

  //! Adapts an object with the ArgumentPolicy interface
  template<class T>
  class ArgumentPolicyAdapter : public ArgumentPolicy {

  public:

    //! Default constructor
    ArgumentPolicyAdapter (Function* context, T* adapt)
     : ArgumentPolicy (context), adaptee (adapt) { }

    //! Clone operator 
    ArgumentPolicyAdapter* clone (Function* context) const
    { return new ArgumentPolicyAdapter (context, adaptee); }

    //! Set the independent variable of the specified dimension
    void set_argument (unsigned dimension, Argument* axis)
    { adaptee->set_argument (dimension, axis); }

  protected:

    //! The adapted object
    Reference::To<T> adaptee;

  };

}

#endif
