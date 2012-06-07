//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/Gaussian.h,v $
   $Revision: 1.7 $
   $Date: 2006/10/06 21:13:53 $
   $Author: straten $ */

#ifndef __Gaussian_H
#define __Gaussian_H

#include "MEAL/Univariate.h"
#include "MEAL/Scalar.h"
#include "MEAL/Parameters.h"

namespace MEAL {

  //! Gaussian function 
  class Gaussian : public Univariate<Scalar> {

  public:

    Gaussian ();

    //! Set the centre
    void set_centre (double centre);

    //! Get the centre
    double get_centre () const;

    //! Set the width
    void set_width (double width);

    //! Get the width
    double get_width () const;

    //! Set the height
    void set_height (double height);

    //! Get the height
    double get_height () const;

    //! Set the period
    void set_period (double period);

    //! Get the period
    double get_period () const;

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

    //! Set to greater than zero if the Gaussian is periodic
    double period;

  private:

    //! Parameter policy
    Parameters parameters;

  };

}

#endif
