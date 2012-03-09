//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/StokesError.h,v $
   $Revision: 1.5 $
   $Date: 2006/10/06 21:13:54 $
   $Author: straten $ */

#ifndef __MEAL_StokesError_H
#define __MEAL_StokesError_H

#include "Pauli.h"
#include "Reference.h"

namespace MEAL {

  //! Propagates Stokes parameter uncertainty through congruence transformation
  class StokesError : public Reference::Able {

  public:

    //! Default constructor
    StokesError ();

    //! Set the variances of the input Stokes parameters
    void set_variance (const Stokes<double>&);

    //! Set the transformation
    void set_transformation (const Jones<double>&);

    //! Get the variances of the output Stokes parameters
    Stokes<double> get_variance () const;

    //! Set the transformation gradient component
    void set_transformation_gradient (const Jones<double>&);

    //! Get the variances of the output Stokes parameters gradient component
    Stokes<double> get_variance_gradient () const;

    //! Get the input variance (not transformed)
    Stokes<double> get_input_variance () const;

  protected:

    //! The input Stokes parameter variances
    Stokes<double> input;

    //! The Jones matrix of the transformation
    Jones<double> jones;

    //! The Jones matrix of the transformation gradient component
    Jones<double> jones_grad;

    //! Flag set to false when del must be recomputed
    bool built;

    //! Computes del
    void build();

    //! The Mueller matrix of the transformation
    Matrix<4,4,double> xform;

    //! The Mueller matrix of the transformation gradient component
    Matrix<4,4,double> del;

  };

}

#endif
