//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/Tracer.h,v $
   $Revision: 1.6 $
   $Date: 2008/04/07 00:38:18 $
   $Author: straten $ */

#ifndef __MEAL_Tracer_H
#define __MEAL_Tracer_H

#include "MEAL/Function.h"

namespace MEAL
{
  //! Traces changes in a single Function instance
  class Tracer : public Reference::Able
  {

  public:

    //! Default constructor
    Tracer ();

    //! Destructor
    ~Tracer ();

    //! Trace changes in the specified function
    virtual void watch (Function*);

  protected:

    //! The function to watch
    Reference::To<Function> model;

    //! Method called when a Function attribute has changed
    virtual void attribute_changed (Function::Attribute attribute);

    //! Method called to report state
    virtual void report () = 0;

  };

}

#endif

