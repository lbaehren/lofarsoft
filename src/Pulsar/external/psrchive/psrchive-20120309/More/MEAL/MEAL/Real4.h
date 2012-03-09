//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/Real4.h,v $
   $Revision: 1.5 $
   $Date: 2009/06/06 11:07:20 $
   $Author: straten $ */

#ifndef __MEAL_Real4_H
#define __MEAL_Real4_H

#include "MEAL/Evaluable.h"
#include "Matrix.h"

namespace MEAL {

  //! Pure virtual base class of all real-valued 4x4 matrix functions
  class Real4 : public Evaluable< Matrix<4,4,double> > 
  {
  public:

    //! The name of the class
    static const char* Name;

    //! Clone method
    virtual Real4* clone () const;
  };

}

#endif
