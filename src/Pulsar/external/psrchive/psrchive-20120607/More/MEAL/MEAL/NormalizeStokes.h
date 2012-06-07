//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/NormalizeStokes.h,v $
   $Revision: 1.8 $
   $Date: 2008/03/03 07:21:53 $
   $Author: straten $ */

#ifndef __MEAL_NormalizeStokes_h
#define __MEAL_NormalizeStokes_h

#include "MEAL/Invariant.h"
#include "MEAL/ScalarParameter.h"

namespace MEAL {

  class Scalar;

  //! Normalize Stokes parameters by the invariant interval
  /*! This class properly handles the propagation of errors during
    normalization. */
  class NormalizeStokes : public Reference::Able {

  public:

    //! Default constructor
    NormalizeStokes ();

    //! Copy constructor
    NormalizeStokes (const NormalizeStokes&);

    //! Destructor
    ~NormalizeStokes ();

    //! Set the other term in the denominator
    void set_other (const Estimate<double>& other);

    //! Assignment operator
    NormalizeStokes& operator = (const NormalizeStokes&);

    //! Normalize the Stokes parameters by their invariant interval
    void normalize (Stokes< Estimate<float> >& stokes);

    //! Normalize the Stokes parameters by their invariant interval
    void normalize (Stokes< Estimate<double> >& stokes);

    //! Normalize the Stokes parameters by the determinant
    void normalize (Stokes< Estimate<double> >& stokes, Estimate<double> det);

  protected:

    //! Calculates the invariant interval and its bias
    Reference::To<Invariant> invariant;

    //! The other term in the denominator
    Reference::To<ScalarParameter> other;

    //! The output Stokes parameters
    Stokes<ScalarMath> output;

  private:

    //! Does the work for the constructors
    void init ();

  };

}

#endif

