//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/FunctionInterface.h,v $
   $Revision: 1.1 $
   $Date: 2009/08/16 21:15:19 $
   $Author: straten $ */

#ifndef __MEAL_FunctionInterface_h
#define __MEAL_FunctionInterface_h

#include "MEAL/Function.h"
#include "TextInterface.h"

namespace MEAL
{

  //! Provides a text interface to get and set Function attributes
  class Function::Interface : public TextInterface::To<Function>
  {

  public:

    //! Default constructor that takes an optional instance
    Interface ( Function* = 0 );

    //! Get the interface name
    std::string get_interface_name() const { return "Function::Interface"; }
  };

}


#endif
