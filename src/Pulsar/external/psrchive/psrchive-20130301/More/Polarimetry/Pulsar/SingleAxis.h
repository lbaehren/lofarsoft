//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/SingleAxis.h,v $
   $Revision: 1.11 $
   $Date: 2009/08/07 14:05:40 $
   $Author: straten $ */

#ifndef __Calibration_SingleAxis_H
#define __Calibration_SingleAxis_H

#include "MEAL/ProductRule.h"
#include "MEAL/Complex2.h"

#include "Estimate.h"
#include "Stokes.h"

namespace MEAL
{
  template<typename> class Gain;
  class Boost1;
  class Rotation1;
}

namespace Calibration {

  //! Instrumental gain, differential gain, and differential phase
  /*! The SingleAxis class may be used to represent three commonly
    encountered instrumental transformations: absolute gain,
    differential gain, and differential phase.  Differential gain and
    phase transformations are respectively hyperbolic and
    trigonometric rotations about a single axis defined by the
    polarization of the instrument.  With circular-polarized
    receptors, the single axis is the Stokes V axis; with
    linear-polarized receptors, it is normally the Stokes Q axis. */
  class SingleAxis : public MEAL::ProductRule<MEAL::Complex2> {

  public:

    //! Default Constructor
    SingleAxis ();

    //! Copy Constructor
    SingleAxis (const SingleAxis& s);

    //! Assignement Operator
    const SingleAxis& operator = (const SingleAxis& s);

    //! Destructor
    ~SingleAxis ();

    //! Clone operator
    SingleAxis* clone () const;

    //! Set cyclical bounds on the differential phase
    void set_cyclic (bool flag = true);

    /** @name Free parameters
     *  These parameters may be varied during least-squares minimization
     */
    //@{

    //! Get the instrumental gain, \f$ G \f$, in calibrator flux units
    Estimate<double> get_gain () const;

    //! Get the differential gain, \f$ \gamma \f$, in hyperbolic radians
    Estimate<double> get_diff_gain () const;
    
    //! Get the differential phase, \f$ \phi \f$, in radians
    Estimate<double> get_diff_phase () const;

    //! Set the instrumental gain, \f$ G \f$, in calibrator flux units
    void set_gain (const Estimate<double>& gain);

    //! Set the differential gain, \f$ \gamma \f$, in hyperbolic radians
    void set_diff_gain (const Estimate<double>& gamma);
    
    //! Set the differential phase, \f$ \phi \f$, in radians
    void set_diff_phase (const Estimate<double>& phi);
 
    //@}

    // Non-free parameters

    //! Set the axis along which the boost and rotation occur
    void set_axis (const Vector<3, double>& axis);

    //! Get the unit-vector along which the boost and rotation occur
    Vector<3, double> get_axis () const;

    //! Solve for gain, boost, and rotation given output cal state
    void solve (const std::vector< Estimate<double> >& output);

    //! Invert the transformation represented by this instance
    void invert ();

    //! Combine the transformation from other into this instance
    const SingleAxis& operator *= (const SingleAxis& other);

    // ///////////////////////////////////////////////////////////////////
    //
    // Model implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Return the name of the class
    std::string get_name () const;

    //! Get the gain transformation
    MEAL::Gain<Complex2>* get_gain_transformation ();

    //! Get the boost transformation
    MEAL::Boost1* get_boost_transformation ();

    //! Get the rotation transformation
    MEAL::Rotation1* get_rotation_transformation ();

  protected:

    //! Gain Complex2
    Reference::To< MEAL::Gain<Complex2> > gain;

    //! Boost Complex2
    Reference::To<MEAL::Boost1> boost;

    //! Rotation Complex2
    Reference::To<MEAL::Rotation1> rotation;

  private:

    //! Initialize function used by constructors
    void init ();

  };

}

#endif

