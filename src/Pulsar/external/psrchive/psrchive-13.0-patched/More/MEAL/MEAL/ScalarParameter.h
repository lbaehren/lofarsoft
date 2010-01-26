//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/ScalarParameter.h,v $
   $Revision: 1.5 $
   $Date: 2009/10/22 17:28:32 $
   $Author: straten $ */

#ifndef __ScalarParameter_H
#define __ScalarParameter_H

#include "MEAL/Scalar.h"

namespace MEAL {

  //! Represents a scalar parameter, \f$ x \f$
  class ScalarParameter : public Scalar {

  public:

    //! Default constructor
    ScalarParameter (const Estimate<double>& value = 0.0);

    void set_value (const Estimate<double>& value);
    Estimate<double> get_value () const;

    //! Get the name of the parameter
    std::string get_value_name () const;

    //! Set the name of the parameter
    void set_value_name (const std::string& name);

    // ///////////////////////////////////////////////////////////////////
    //
    // Function implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Return the name of the class
    std::string get_name () const;

  protected:

    // ///////////////////////////////////////////////////////////////////
    //
    // Optimized implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Return the value (and gradient, if requested) of the function
    void calculate (double& x, std::vector<double>* grad=0);

  };

}

#endif

