//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/ParameterTracer.h,v $
   $Revision: 1.2 $
   $Date: 2008/04/07 00:38:18 $
   $Author: straten $ */

#ifndef __MEAL_ParameterTracer_H
#define __MEAL_ParameterTracer_H

#include "MEAL/Tracer.h"

namespace MEAL
{

  //! Traces changes to a single parameter of a single Function
  class ParameterTracer : public Tracer
  {
  public:

    //! Default constructor
    ParameterTracer (Function* model = 0, unsigned param = 0);

    //! Trace the specified parameter of the specified function
    virtual void watch (Function* model, unsigned param);

  protected:

    //! The parameter to watch
    unsigned parameter;

    //! The current value of the watched parameter
    double current_value;

    //! Report the parameter value
    void report ();

  };

}

#endif

