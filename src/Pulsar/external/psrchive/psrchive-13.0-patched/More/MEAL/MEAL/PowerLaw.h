//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Aidan Hotan
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/PowerLaw.h,v $
   $Revision: 1.1 $
   $Date: 2006/11/09 02:56:48 $
   $Author: ahotan $ */

#ifndef __PowerLaw_H
#define __PowerLaw_H

#include "MEAL/Univariate.h"
#include "MEAL/Scalar.h"
#include "MEAL/Parameters.h"

namespace MEAL {

  //! PowerLaw function 
  class PowerLaw : public Univariate<Scalar> {

  public:

    PowerLaw ();

    //! Set the amplitude scale factor
    void set_scale (double scale);

    //! Get the centre
    double get_scale () const;

    //! Set the horizontal offset
    void set_offset (double offset);

    //! Get the horizontal offset
    double get_offset () const;

    //! Set the power
    void set_power (double power);

    //! Get the power
    double get_power () const;

    // ///////////////////////////////////////////////////////////////////
    //
    // Function implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Return the name of the class
    std::string get_name () const;

  protected:

    //! Return the value (and gradient, if requested) of the function
    void calculate (double& x, std::vector<double>* grad=0);

  private:

    //! Parameter policy
    Parameters parameters;

  };

}

#endif
