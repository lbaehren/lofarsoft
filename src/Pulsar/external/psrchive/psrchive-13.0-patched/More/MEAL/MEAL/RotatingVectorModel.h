//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2005-2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/RotatingVectorModel.h,v $
   $Revision: 1.6 $
   $Date: 2009/06/11 07:33:31 $
   $Author: straten $ */

#ifndef __RotatingVectorModel_H
#define __RotatingVectorModel_H

#include "MEAL/Scalar.h"
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

    Reference::To<ScalarParameter> reference_position_angle;
    Reference::To<ScalarParameter> line_of_sight;
    Reference::To<ScalarParameter> magnetic_axis;
    Reference::To<ScalarParameter> magnetic_meridian;

    // ///////////////////////////////////////////////////////////////////
    //
    // Function implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Return the name of the class
    std::string get_name () const;

  private:

    void calculate (double&, std::vector<double>*) {}
    void init ();

    Reference::To<Scalar> expression;

  };

}

#endif
