//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/Constant.h,v $
   $Revision: 1.5 $
   $Date: 2006/10/06 21:13:53 $
   $Author: straten $ */

#ifndef __MEAL_Constant_H
#define __MEAL_Constant_H

#include "MEAL/NoParameters.h"

namespace MEAL {

  //! Represents a constant model with no parameters
  /*! The Constant primarily serves as an optimization convention.
    The Composite will not map a Constant, thereby reducing the number
    of Function instances managed by the Composite and improving performance */

  class Constant : public NoParameters {

  public:


  };

}

#endif
