//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/StokesCovariance.h,v $
   $Revision: 1.3 $
   $Date: 2006/10/06 21:13:54 $
   $Author: straten $ */

#ifndef __MEAL_StokesCovariance_H
#define __MEAL_StokesCovariance_H

#include "Pauli.h"
#include "Reference.h"

namespace MEAL {

  //! Propagates Stokes parameter covariances through congruence transformation
  class StokesCovariance : public Reference::Able {

  public:

    //! Default constructor
    StokesCovariance ();

    //! Set the variances of the input Stokes parameters
    void set_variance (const Stokes<double>&);

    //! Set the transformation
    void set_transformation (const Jones<double>&);

    //! Set the transformation
    void set_transformation (const Matrix<4,4,double>&);

    //! Get the variances of the output Stokes parameters
    Matrix<4,4,double> get_covariance () const;

    //! Set the transformation gradient component
    void set_transformation_gradient (const Jones<double>&);

    //! Set the transformation gradient component
    void set_transformation_gradient (const Matrix<4,4,double>&);

    //! Get the variances of the output Stokes parameters gradient component
    Matrix<4,4,double> get_covariance_gradient () const;

    //! Get the input variance (not transformed)
    Stokes<double> get_input_variance () const;

  protected:

    //! The input Stokes parameter variances
    Stokes<double> input;

    //! The Jones matrix of the transformation
    Jones<double> jones;

    //! The Jones matrix of the transformation gradient component
    Jones<double> jones_grad;

    //! Flag set to false when xform_grad must be recomputed
    bool built;

    //! Computes xform_grad
    void build();

    //! The Mueller matrix of the transformation
    Matrix<4,4,double> xform;

    //! The Mueller matrix of the transformation gradient component
    Matrix<4,4,double> xform_grad;

  };

}

#endif
