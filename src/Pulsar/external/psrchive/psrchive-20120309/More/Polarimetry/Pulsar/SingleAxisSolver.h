//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/SingleAxisSolver.h,v $
   $Revision: 1.6 $
   $Date: 2006/10/06 21:13:54 $
   $Author: straten $ */

#ifndef __Calibration_SingleAxisSolver_H
#define __Calibration_SingleAxisSolver_H

#include "Quaternion.h"
#include "Stokes.h"
#include "Estimate.h"
#include "MEAL/ScalarMath.h"

namespace Calibration {

  class SingleAxis;

  //! Derives SingleAxis parameters from input and output states
  class SingleAxisSolver : public Reference::Able {

  public:

    //! Default constructor
    SingleAxisSolver ();

    //! Copy constructor
    SingleAxisSolver (const SingleAxisSolver&);

    //! Destructor
    ~SingleAxisSolver ();

    //! Assignment operator
    SingleAxisSolver& operator = (const SingleAxisSolver&);

    //! Set the input Stokes parameters
    void set_input (const Stokes< Estimate<double> >& stokes);

    //! Set the input Stokes parameters
    void set_output (const Stokes< Estimate<double> >& stokes);

    //! Set the SingleAxis parameters with the current solution
    void solve (SingleAxis* model);

  protected:

    //! The input Stokes parameters
    Quaternion<MEAL::ScalarMath,Hermitian> input;

    //! The output Stokes parameters
    Quaternion<MEAL::ScalarMath,Hermitian> output;

    //! The Axis about which the rotations take place
    Vector<3, MEAL::ScalarMath> axis;

    //! The gain solution
    MEAL::ScalarMath gain;

    //! The differential gain solution
    MEAL::ScalarMath diff_gain;

    //! The differential phase solution
    MEAL::ScalarMath diff_phase;

  private:

    //! Does the work for the constructors
    void init ();

  };

}

#endif
