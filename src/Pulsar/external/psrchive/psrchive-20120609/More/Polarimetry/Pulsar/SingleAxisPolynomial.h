//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/SingleAxisPolynomial.h,v $
   $Revision: 1.6 $
   $Date: 2009/06/13 05:11:05 $
   $Author: straten $ */

#ifndef __SingleAxisPolynomial_H
#define __SingleAxisPolynomial_H

#include "MEAL/ChainRule.h"
#include "MEAL/Complex2.h"
#include "MEAL/Polynomial.h"

namespace Calibration {

  class SingleAxis;

  //! A SingleAxis parameterized by Polynomial ordinates
  /*! This class provides an example use of the ChainRule class.  The
    SingleAxis Complex2 is used so that the gain, differential
    gain, and differential phase vary separately as polynomial
    functions of an independent variable.  This class is used in
    test_ReceptionModel as a verification of the underlying
    assumptions of the ChainRule. */

  class SingleAxisPolynomial : public MEAL::ChainRule<MEAL::Complex2>
  {

  public:

    //! The number of free parameters in the SingleAxis model
    static const unsigned npoly;

    //! Default constructor
    SingleAxisPolynomial (unsigned ncoef = 0);

    //! Copy constructor
    SingleAxisPolynomial (const SingleAxisPolynomial&);

    //! Assignment operator
    SingleAxisPolynomial& operator = (const SingleAxisPolynomial&);

    //! Destructor
    ~SingleAxisPolynomial ();

    //! Set the abscissa of each of the Polynomial functions
    void set_abscissa (double value);

    //! Set the fit flag for the specified coefficient of each Polynomial
    void set_infit (unsigned coef, bool fit);

    //! Generate random Polynomial coefficients
    void random (double max_result, double max_abscissa);

    // ///////////////////////////////////////////////////////////////////
    //
    // Model implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Return the name of the class
    std::string get_name () const;

    //! Connect the set_abscissa method to the axis value
    void set_argument (unsigned dimension, MEAL::Argument* axis);

  protected:

    //! The Polynomial Functions
    std::vector< Reference::To<MEAL::Polynomial> > polynomial;

    //! The SingleAxis Complex2
    Reference::To<SingleAxis> xform;

  private:

    //! Initialize function used by constructors
    void init (unsigned ncoef);

  };

}

#endif

