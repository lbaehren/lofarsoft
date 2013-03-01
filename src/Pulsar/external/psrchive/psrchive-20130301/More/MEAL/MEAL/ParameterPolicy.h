//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/ParameterPolicy.h,v $
   $Revision: 1.7 $
   $Date: 2008/05/07 01:21:11 $
   $Author: straten $ */

#ifndef __MEAL_ParameterPolicy_H
#define __MEAL_ParameterPolicy_H

#include "MEAL/FunctionPolicy.h"

namespace MEAL {

  //! Managers of Function parameters
  class ParameterPolicy : public FunctionPolicy {

  protected:

    //! Default destructor
    ParameterPolicy (Function* context);

  public:

    //! Install policy upon construction
    static bool auto_install;

    //! Clone constructor
    virtual ParameterPolicy* clone (Function* new_context) const = 0;

    //! Return the number of parameters
    virtual unsigned get_nparam () const = 0;

    //! Return the name of the specified parameter
    virtual std::string get_name (unsigned index) const = 0;

    //! Return the name of the specified parameter
    virtual std::string get_description (unsigned index) const = 0;

    //! Return the value of the specified parameter
    virtual double get_param (unsigned index) const = 0;

    //! Set the value of the specified parameter
    virtual void set_param (unsigned index, double value) = 0;

    //! Return the variance of the specified parameter
    virtual double get_variance (unsigned index) const = 0;

    //! Set the variance of the specified parameter
    virtual void set_variance (unsigned index, double value) = 0;

    //! Return true if parameter at index is to be fitted
    virtual bool get_infit (unsigned index) const = 0;
    
    //! Set flag for parameter at index to be fitted
    virtual void set_infit (unsigned index, bool flag) = 0;

  };

}

#endif
