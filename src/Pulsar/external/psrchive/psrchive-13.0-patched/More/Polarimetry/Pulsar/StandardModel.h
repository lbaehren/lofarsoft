//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2005-2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/StandardModel.h,v $
   $Revision: 1.27 $
   $Date: 2009/11/23 05:28:23 $
   $Author: straten $ */

#ifndef __Calibration_StandardModel_H
#define __Calibration_StandardModel_H

// Reception Model and its management
#include "Pulsar/ReceptionModel.h"
#include "Pulsar/ReceptionModelSolver.h"

#include "Pulsar/CalibratorType.h"
#include "Pulsar/MeanPolar.h"
#include "Pulsar/MeanSingleAxis.h"
#include "Pulsar/ConvertMJD.h"

#include "MEAL/ProductRule.h"
#include "MEAL/ChainRule.h"
#include "MEAL/Polynomial.h"
#include "MEAL/Gain.h"
#include "MEAL/Axis.h"
#include "MEAL/Real4.h"

namespace Calibration
{
  //! Stores the various elements related to the calibration model
  class StandardModel : public Reference::Able
  {
  public:

    //! Verbosity flag
    static bool verbose;

    //! Default constructor
    StandardModel (Pulsar::Calibrator::Type*);

    //! Include an impurity transformation
    void set_impurity (MEAL::Real4*);

    //! Set the transformation to the receptor basis
    void set_basis (MEAL::Complex2*);

    //! Set true when the pulsar Stokes parameters have been normalized
    void set_constant_pulsar_gain (bool = true);

    //! Set the transformation to be cloned for each calibrator transformation
    void set_foreach_calibrator (const MEAL::Complex2*);

    //! Set gain to the univariate function of time
    void set_gain (MEAL::Univariate<MEAL::Scalar>*);

    //! Set differential gain to the univariate function of time
    void set_diff_gain (MEAL::Univariate<MEAL::Scalar>*);

    //! Set differential phase to the univariate function of time
    void set_diff_phase (MEAL::Univariate<MEAL::Scalar>*);

    //! Set gain to the univariate function of time
    const MEAL::Scalar* get_gain () const;

    //! Set differential gain to the univariate function of time
    const MEAL::Scalar* get_diff_gain () const;

    //! Set differential phase to the univariate function of time
    const MEAL::Scalar* get_diff_phase () const;

    //! Add a step if any of the above functions require it
    void add_calibrator_epoch (const MJD&);

    //! Record the epochs of observations
    void add_observation_epoch (const MJD&);

    //! Set the reference epoch
    void set_reference_epoch (const MJD& epoch);

    //! Update the relevant estimate
    void update ();

    //! Work around the fact that no reference calibrators are available
    void no_reference_calibrators ();

    //! Attempt to reduce the number of degrees of freedom in the model
    bool reduce_nfree ();

    //! Check all relevant constraints
    void check_constraints ();

    //! Solve the measurement equation
    void solve ();

    //! Add a new signal path for the poln calibrator observations
    void add_polncal_backend ();

    //! Add a new signal path for the flux calibrator observations
    void add_fluxcal_backend ();

    //! Fix the rotation about the line of sight
    void fix_orientation ();

    //! Get the index for the signal path experienced by the flux calibrator
    unsigned get_fluxcal_path () const { return FluxCalibrator_path; }

    //! Get the index for the signal path experienced by the reference source
    unsigned get_polncal_path () const { return ReferenceCalibrator_path; }

    //! Get the index for the signal path experienced by the pulsar
    unsigned get_pulsar_path () const { return Pulsar_path; }

    //! Integrate a calibrator solution
    void integrate_calibrator (const MEAL::Complex2* xform, bool fluxcal);

    //! Get the measurement equation solver
    Calibration::ReceptionModel* get_equation ();

    //! Set the measurement equation
    void set_equation (Calibration::ReceptionModel*);

    //! Set the algorithm used to solve the measurement equation
    void set_solver (Calibration::ReceptionModel::Solver*);

    //! Copy the parameters for the signal path experienced by the pulsar
    void copy_transformation (const MEAL::Complex2*);

    //! Get the instrumental transformation
    const MEAL::Complex2* get_transformation () const;

    //! Get the instrumental transformation
    MEAL::Complex2* get_transformation ();

    //! Get the full signal path experienced by the pulsar
    const MEAL::Complex2* get_pulsar_transformation () const;

    //! Get the covariance vector at the specified epoch
    void get_covariance( std::vector<double>& covar, const MJD& epoch );

    //! The time axis
    MEAL::Axis<MJD> time;

    //! The known transformations from the sky to the receptors
    MEAL::Axis< Jones<double> > projection;

    //! validity flag
    bool valid;

    //! Deactivate time variations and set the Instrument to the given epoch
    void disengage_time_variations (const MJD& epoch);

    //! Activate time varations
    void engage_time_variations ();

 protected:

    //! ReceptionModel
    Reference::To< Calibration::ReceptionModel > equation;

    //! The algorithm used to solve the measurement equation
    Reference::To< Calibration::ReceptionModel::Solver > solver;

    //! Used to convert MJD to double
    Calibration::ConvertMJD convert;

    //! The signal path experienced by the calibrator
    Reference::To< MEAL::ChainRule<MEAL::Complex2> > pcal_gain_chain;
    Reference::To< MEAL::Gain<MEAL::Complex2> > pcal_gain;
    //! Transformation cloned for each calibrator observation
    Reference::To< const MEAL::Complex2 > foreach_pcal;

    //! The signal path experienced by the pulsar
    Reference::To< MEAL::ProductRule<MEAL::Complex2> > pulsar_path;

    //! The instrumental model and any additional transformations
    Reference::To< MEAL::ProductRule<MEAL::Complex2> > instrument;

    //! The Mueller transformation
    Reference::To< MEAL::Real4 > impurity;

    //! The instrumental response
    Reference::To< MEAL::Complex2 > response;

    //! The best estimate of the backend
    Calibration::MeanSingleAxis backend_estimate;

    //! The best estimate of the polar model
    Calibration::MeanPolar polar_estimate;

    //! The basis transformation
    Reference::To<MEAL::Complex2> basis;

    //! The backend variation transformations
    Reference::To< MEAL::Scalar > gain;
    Reference::To< MEAL::Scalar > diff_gain;
    Reference::To< MEAL::Scalar > diff_phase;

    void integrate_parameter (MEAL::Scalar* function, double value);

    void update_parameter (MEAL::Scalar* function, double value);

    void compute_covariance( unsigned index, 
			     std::vector< std::vector<double> >& covar,
			     std::vector<unsigned>& function_imap, 
			     MEAL::Scalar* function );

    //! Set the minimum step if Scalar is a Steps
    void set_min_step (MEAL::Scalar* function, double minimum);

    //! Offset the steps if Scalar is a Steps
    void offset_steps (MEAL::Scalar* function, double offset);

    //! Add a step if Scalar is a Steps
    void add_step (MEAL::Scalar* function, double step);

    bool time_variations_engaged;
    bool constant_pulsar_gain;

    // ////////////////////////////////////////////////////////////////////
    //
    //! Additional backend required for flux calibrator signal path
    Reference::To< Calibration::SingleAxis > fluxcal_backend;

    //! The best estimate of the flux calibration backend
    Calibration::MeanSingleAxis fluxcal_backend_estimate;

    //! The signal path of the FluxCalibrator source
    unsigned FluxCalibrator_path;

    //! The signal path of the ReferenceCalibrator source
    unsigned ReferenceCalibrator_path;

    //! The signal path of the Pulsar phase bin sources
    unsigned Pulsar_path;

    MJD min_epoch, max_epoch;

    //! Add a transformation to the ReceptionModel
    void add_transformation (MEAL::Complex2*);

  private:

    //! Parameterization of the instrument
    Reference::To<Pulsar::Calibrator::Type> type;

    //! built flag
    bool built;

    //! build method
    void build ();

    //! build called from const method
    void const_build () const;

  };
  
}

#endif
