//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/Polynomial.h,v $
   $Revision: 1.10 $
   $Date: 2007/11/09 04:47:59 $
   $Author: straten $ */

#ifndef __Polynomial_H
#define __Polynomial_H

#include "MEAL/Univariate.h"
#include "MEAL/Scalar.h"
#include "MEAL/Parameters.h"

namespace MEAL {

  //! Polynomial function with an arbitrary number of coefficients
  class Polynomial : public Univariate<Scalar> {

  public:

    //! Default constructor
    Polynomial (unsigned ncoef = 0);

    //! Copy constructor
    Polynomial (const Polynomial&);

    //! Assignment operator
    Polynomial& operator = (const Polynomial&);

    //! Clone operator
    Polynomial* clone () const;

    //! Resize
    void resize (unsigned ncoef);

    //! Construct from array of polynomial coefficients
    template<class T>
    Polynomial (const std::vector<T>& coefs) : parameters (this, coefs.size())
    {
      init ();
      for (unsigned ic=0; ic < coefs.size(); ic++)
        set_param (ic, coefs[ic]);
    }

    //! Setter
    void set_abscissa_offset(double x0)
    { x_0 = x0; }

    // ///////////////////////////////////////////////////////////////////
    //
    // Function implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Return the name of the class
    std::string get_name () const;

    //! Parses the values of model parameters and fit flags from a string
    void parse (const std::string& text);

  protected:

    //! Return the value (and gradient, if requested) of the function
    void calculate (double& x, std::vector<double>* grad=0);

    //! Print the values of model parameters and fit flags to a string
    void print_parameters (std::string& text, const std::string& sep) const;

  private:

    //! Abscissa offset
    double x_0;

    //! Parameter policy
    Parameters parameters;

    //! Initialization
    void init ();

  };

}

#endif
