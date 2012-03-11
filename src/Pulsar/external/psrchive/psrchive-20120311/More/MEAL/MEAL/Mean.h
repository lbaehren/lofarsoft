//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/Mean.h,v $
   $Revision: 1.9 $
   $Date: 2006/10/06 21:13:53 $
   $Author: straten $ */

#ifndef __MEAL_Mean_H
#define __MEAL_Mean_H

#include "ReferenceAble.h"

namespace MEAL {

  //! Used to accumulate a running mean of Complex2 model parameters
  /*! This pure virtual template base class defines the interface by
    which mean Function parameters may be accumulated and used. */

  template<class T>
  class Mean : public Reference::Able {

  public:

    //! Add the Function parameters to the running mean
    virtual void integrate (const T* model) = 0;

    //! Update the model parameters with the current value of the mean
    virtual void update (T* model) const = 0;

    //! Return the normalized difference between the mean and model
    virtual double chisq (const T* model) const { return 0; }

  };

}

#endif

