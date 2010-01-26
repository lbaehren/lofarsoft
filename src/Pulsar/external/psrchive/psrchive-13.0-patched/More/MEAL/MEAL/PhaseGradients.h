//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/PhaseGradients.h,v $
   $Revision: 1.2 $
   $Date: 2008/01/22 05:36:41 $
   $Author: straten $ */

#ifndef __MEAL_PhaseGradients_H
#define __MEAL_PhaseGradients_H

#include "MEAL/Univariate.h"
#include "MEAL/Complex2.h"
#include "MEAL/Parameters.h"

namespace MEAL {

  //! Multiple phase gradients parameterized by their slopes
  class PhaseGradients : public Univariate<Complex2> {

  public:

    //! Default constructor
    PhaseGradients (unsigned ngradient = 0);

    //! Copy constructor
    PhaseGradients (const PhaseGradients&);

    //! Assignment operator
    PhaseGradients& operator = (const PhaseGradients&);

    //! Clone operator
    PhaseGradients* clone () const;

    //! Get the number of gradients
    unsigned get_ngradient () const;

    //! Set the current phase gradient index
    void set_igradient (unsigned igradient);

    //! Get the current phase gradient index
    unsigned get_igradient () const;

    //! Set the current phase gradient index
    void set_offset (unsigned igradient, double offset);

    //! Get the current phase gradient index
    double get_offset (unsigned igradient) const;

    //! Add another gradient to the set
    void add_gradient ();

    //! Set the number of gradients
    void resize (unsigned ngradient);

    // ///////////////////////////////////////////////////////////////////
    //
    // Function implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Return the name of the class
    std::string get_name () const;

  protected:

    //! Calculate the Jones matrix and its gradient
    void calculate (Jones<double>& result, std::vector<Jones<double> >*);
   
    //! Parameter policy
    Parameters parameters;

    //! The phase offsets
    std::vector<double> offsets;

    //! The current phase gradient
    unsigned igradient;

  };

}

#endif
