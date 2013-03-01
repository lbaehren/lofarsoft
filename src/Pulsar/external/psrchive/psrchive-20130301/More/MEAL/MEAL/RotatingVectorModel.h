//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2005-2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/RotatingVectorModel.h,v $
   $Revision: 1.7 $
   $Date: 2010/05/24 06:38:07 $
   $Author: straten $ */

#ifndef __RotatingVectorModel_H
#define __RotatingVectorModel_H

#include "MEAL/Scalar.h"
#include "MEAL/SumRule.h"
#include "MEAL/Univariate.h"

namespace MEAL {

  class ScalarParameter;

  //! The Rotating Vector Model (RVM) of Radhakrishnan & Cooke
  /*! The abscissa to this model is the pulsar rotational phase in radians */
  class RotatingVectorModel : public Univariate<Scalar>
  {

  public:

    //! Default constructor
    RotatingVectorModel ();

    //! Copy constructor
    RotatingVectorModel (const RotatingVectorModel& copy);

    //! Assignment operator
    RotatingVectorModel& operator = (const RotatingVectorModel& copy);

    //! Destructor
    ~RotatingVectorModel ();

    //! psi0: position angle at magnetic meridian
    /*! effectively rotation of pulsar about the line of sight */
    Reference::To<ScalarParameter> reference_position_angle;

    //! zeta: colatitude of line of sight with respect to spin axis
    Reference::To<ScalarParameter> line_of_sight;

    //! alpha: colatitude of magnetic axis with respect to spin axis
    Reference::To<ScalarParameter> magnetic_axis;

    //! phi0: longitude of the magnetic merdian
    Reference::To<ScalarParameter> magnetic_meridian;

    //! beta: colatitude of line of sight with respect to magnetic axis
    Reference::To<ScalarParameter> impact;

    //! Switch to using impact as a free parameter
    void use_impact (bool);

    // ///////////////////////////////////////////////////////////////////
    //
    // Function implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Return the name of the class
    std::string get_name () const;

    //! Get the projection onto North
    Scalar* get_north () { return north; }
    //! Get the projection onto East
    Scalar* get_east () { return east; }

  private:

    void calculate (double&, std::vector<double>*) {}
    void init ();

    // the magnetic field line projected onto North
    Reference::To<Scalar> north;

    // the magnetic field line projected onto East
    Reference::To<Scalar> east;

    // the answer
    Reference::To<Scalar> expression;

    // the sum rule used to switch between zeta and beta
    Reference::To< SumRule<Scalar> > zeta_sum;

  };

}

#endif
