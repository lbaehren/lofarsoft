//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/NoParameters.h,v $
   $Revision: 1.7 $
   $Date: 2008/05/07 01:21:11 $
   $Author: straten $ */

#ifndef __MEAL_NoParameters_H
#define __MEAL_NoParameters_H

#include "MEAL/ParameterPolicy.h"

namespace MEAL {

  //! Represents a model with no parameters
  class NoParameters : public ParameterPolicy {

  public:

    //! Default constructor
    NoParameters () : ParameterPolicy (0) { }

    //! Clone construtor
    NoParameters* clone (Function*) const { return new NoParameters; }

    // ///////////////////////////////////////////////////////////////////
    //
    // ParameterPolicy implementation
    //
    // ///////////////////////////////////////////////////////////////////
 
    //! Return the number of parameters
    unsigned get_nparam () const { return 0; }

    //! Return the name of the specified parameter
    std::string get_name (unsigned index) const { return ""; }

    //! Return the name of the specified parameter
    std::string get_description (unsigned index) const { return ""; }

    //! Return the value of the specified parameter
    double get_param (unsigned index) const { return 0.0; }

    //! Set the value of the specified parameter
    void set_param (unsigned index, double value) { }

    //! Return the variance of the specified parameter
    double get_variance (unsigned index) const { return 0; }
    
    //! Set the variance of the specified parameter
    void set_variance (unsigned index, double value) { }

    //! Return true if parameter at index is to be fitted
    bool get_infit (unsigned index) const { return false; }

    //! Set flag for parameter at index to be fitted
    void set_infit (unsigned index, bool flag) { }

  };

}

#endif
