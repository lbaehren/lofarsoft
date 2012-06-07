//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/ForwardChanged.h,v $
   $Revision: 1.3 $
   $Date: 2006/10/06 21:13:53 $
   $Author: straten $ */

#ifndef __MEAL_ForwardChanged_H
#define __MEAL_ForwardChanged_H

#include "MEAL/FunctionPolicy.h"
#include "MEAL/Function.h"

namespace MEAL {

  //! Forwards changed signals from other classes
  class ForwardChanged : public FunctionPolicy {

  public:

    //! Default constructor
    ForwardChanged (Function* context);

    //! Begin to forward the changed status of argument to context
    void manage (Function* argument);

    //! Cease to forward the changed status of argument to context
    void unmanage (Function* argument);

  protected:

    //! Method called when a Function attribute has changed
    void attribute_changed (Function::Attribute attribute);

  };

}

#endif
