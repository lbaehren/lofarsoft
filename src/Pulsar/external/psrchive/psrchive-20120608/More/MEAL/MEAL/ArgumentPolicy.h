//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/ArgumentPolicy.h,v $
   $Revision: 1.4 $
   $Date: 2006/10/06 21:13:53 $
   $Author: straten $ */

#ifndef __MEAL_ArgumentPolicy_H
#define __MEAL_ArgumentPolicy_H

#include "MEAL/FunctionPolicy.h"

namespace MEAL {

  class Argument;

  //! Managers of Function arguments
  class ArgumentPolicy : public FunctionPolicy {

  public:

    //! Default constructor
    ArgumentPolicy (Function* context);

    //! Clone operator 
    virtual ArgumentPolicy* clone (Function* context) const = 0;

    //! Set the independent variable of the specified dimension
    virtual void set_argument (unsigned dimension, Argument* axis) = 0;

  };

}

#endif
