//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/Steps.h,v $
   $Revision: 1.4 $
   $Date: 2009/08/19 21:06:54 $
   $Author: straten $ */

#ifndef __Steps_H
#define __Steps_H

#include "MEAL/Univariate.h"
#include "MEAL/Scalar.h"
#include "MEAL/Parameters.h"

namespace MEAL {

  //! A function with an arbitrary number of steps

  /*! Each step occurs at specified points on the abscissa and, at
    each step, the result of the function is a new free parameter */

  class Steps : public Univariate<Scalar> {

  public:

    //! Default constructor
    Steps ();

    //! Copy constructor
    Steps (const Steps&);

    //! Assignment operator
    Steps& operator = (const Steps&);

    //! Clone operator
    Steps* clone () const;

    //! Add a step at the given point on the abscissa
    void add_step (double x);

    //! Set the abscissa of the specified step
    void set_step (unsigned istep, double x);

    //! Remove a step
    void remove_step (unsigned istep);

    //! Get the abscissa of the specified step
    double get_step (unsigned istep) const;

    //! Get the number of steps
    unsigned get_nstep () const;

    //! Set the abscissa value
    void set_abscissa (double value);

    //! Get the current step index
    unsigned get_step () const;

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

    //! The step abscissa
    std::vector<double> step;

    //! The current step
    int current_step;

  };

}

#endif
