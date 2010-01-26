//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/Invariant.h,v $
   $Revision: 1.5 $
   $Date: 2007/12/24 20:01:58 $
   $Author: straten $ */

#ifndef __MEAL_Invariant_h
#define __MEAL_Invariant_h

#include "Stokes.h"
#include "Estimate.h"
#include "ScalarMath.h"

namespace MEAL {

  class Scalar;

  //! Computes the invariant interval
  /*! Properly handles error propagation and bias correction. */

  class Invariant : public Reference::Able {

  public:

    //! Default constructor
    Invariant ();

    //! Copy constructor
    Invariant (const Invariant&);

    //! Destructor
    ~Invariant ();

    //! Assignment operator
    Invariant& operator = (const Invariant&);

    //! Set the Stokes parameters
    void set_Stokes (const Stokes< Estimate<float> >&);

    //! Set the Stokes parameters
    void set_Stokes (const Stokes< Estimate<double> >&);

    //! Get the invariant interval
    Estimate<double> get_invariant () const;

    //! Get the estimated bias due to measurement error
    double get_bias () const;

    //! Get the bias-corrected estimate
    ScalarMath get_correct_result () const;

    //! Get the input Stokes parameters
    Stokes<ScalarMath> get_input () const { return input; }

  protected:

    //! The input Stokes parameters
    Stokes<ScalarMath> input;

    //! The bias due to measurment error
    ScalarMath bias;

    //! The invariant interval, I^2 - Q^2 - U^2 - V^2
    ScalarMath result;

  private:

    //! Does the work for the constructors
    void init ();

  };

}

#endif

