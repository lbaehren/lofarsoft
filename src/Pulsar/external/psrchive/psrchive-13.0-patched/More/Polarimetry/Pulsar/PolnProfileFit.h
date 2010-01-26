//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2003-2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/PolnProfileFit.h,v $
   $Revision: 1.39 $
   $Date: 2009/08/17 02:19:18 $
   $Author: straten $ */

#ifndef __Pulsar_PolnProfileFit_h
#define __Pulsar_PolnProfileFit_h

#include "Pulsar/CoherencyMeasurementSet.h"
#include "Pulsar/PhaseWeight.h"

#include "MEAL/Axis.h"
#include "Matrix.h"
#include "Estimate.h"
#include "Stokes.h"
#include "FTransformAgent.h"
#include "toa.h"

// forward declarations
namespace MEAL
{
  class Complex2;
  class PhaseGradients;
}

namespace Calibration
{
  class ReceptionModel;
  class TemplateUncertainty;
  class StandardSpectra;
}

namespace Pulsar
{
  class PolnProfile;
  class Profile;

  //! The matrix template matching algorithm
  /*! This class may be used to find the full polarimetric best-fit
    between observed and standard pulse profiles.  The fit yields both
    the polarimetric transformation and the phase shift between the
    two profiles, which may be used to calibrate the instrumental
    response and/or calculate arrival time estimates. */

  class PolnProfileFit : public Reference::Able
  {
    
  public:

    //! Verbosity flag
    static bool verbose;

    //! Default constructor
    PolnProfileFit ();

    //! Copy constructor
    PolnProfileFit (const PolnProfileFit& fit);

    //! Assignment operator
    PolnProfileFit& operator = (const PolnProfileFit& fit);

    //! Destructor
    ~PolnProfileFit ();

    //! Clone
    PolnProfileFit* clone () const;

    //! Set the maximum number of harmonics to include in fit
    void set_maximum_harmonic (unsigned max);

    //! Get the maximum number of harmonics to include in fit
    unsigned get_maximum_harmonic () const { return maximum_harmonic; }

    //! Get the number of harmonics to be included in fit
    unsigned get_nharmonic () const { return n_harmonic; }

    //! Set the on-pulse and baseline regions
    void set_regions (const PhaseWeight& pulse, const PhaseWeight& baseline);

    //! Set the standard to which observations will be fit
    void set_standard (const PolnProfile* standard);

    //! Get the standard to which observations will be fit
    const PolnProfile* get_standard () const;

    //! Add the specified observation to be fit to the standard
    void add_observation (const PolnProfile* observation);

    //! Set the only observation to be fit to the standard
    void set_observation (const PolnProfile* observation);

    //! Set the template from which measurment sets will be constructed
    void set_measurement_set (const Calibration::CoherencyMeasurementSet&);

    //! Set the transformation between the standard and observation
    void set_transformation (MEAL::Complex2* xform);

    //! Get the transformation between the standard and the observation
    MEAL::Complex2* get_transformation () const;

    //! Set the debug mode in the ReceptionModel
    void set_fit_debug (bool flag = true);

    //! Normalize each Stokes vector by the mean on-pulse invariant
    void set_normalize_by_invariant (bool set = true);

    //! Fit the specified observation to the standard
    void fit (const PolnProfile* observation);

    //! Fit all observations to the standard
    void solve ();

    //! Set the fourier transform plan
    void set_plan (FTransform::Plan*);

    //! Get the measurement equation used to model the fit
    Calibration::ReceptionModel* get_equation ();
    const Calibration::ReceptionModel* get_equation () const;

    //! Get the phase offset between the standard and the observation
    Estimate<double> get_phase () const;

    //! Set the phase offset between the observation and the standard
    void set_phase (const Estimate<double>& phase);

    //! Lock pulse phase; i.e., do not allow pulse phase to vary in fit
    void set_phase_lock (bool locked);

    //! Remove pulse phase from model (may be more efficient, but irreversible)
    void remove_phase ();

    //! Get the arrival time estimate
    Tempo::toa get_toa (const PolnProfile* observation,
			const MJD& mjd, double period,
			const std::string& nsite);

    //! Return the phase shift based on the cross correlation function
    float ccf_max_phase (const Profile* std, const Profile* obs) const;

    //! Set true when set_standard should choose the maximum harmonic
    bool choose_maximum_harmonic;

    //! Set true when only the total intensity should be used
    bool emulate_scalar;

    //! Manage the equation transformation
    bool manage_equation_transformation;

    //! Get the statistical interface to the data
    Calibration::StandardSpectra* get_spectra ();

  protected:

    friend class MTMAnalysis;
    friend class STMAnalysis;

    //! The maximum number of harmonics to include in the fit
    unsigned maximum_harmonic;

    //! The number of harmonics in the fit
    unsigned n_harmonic;

    //! The standard to which observations will be fit
    Reference::To<const PolnProfile> standard;

    //! The Fourier transform of the standard
    Reference::To<const PolnProfile> standard_fourier;

    //! The transformation between the standard and observation
    Reference::To<MEAL::Complex2> transformation;

    //! Normalization by total invariant interval with error propagation
    Reference::To<Calibration::StandardSpectra> standard_data;

    //! The measurement equation used to model the fit
    Reference::To<Calibration::ReceptionModel> equation;

    //! Least-squares normalization includes variable template contribution
    std::vector< Reference::To<Calibration::TemplateUncertainty> > uncertainty;

    //! The phase gradient model for each observation added
    Reference::To<MEAL::PhaseGradients> phases;

    //! The phase axis
    MEAL::Axis<double> phase_axis;

    //! The gradient index axis
    MEAL::Axis<unsigned> index_axis;

    //! The template from which measurement sets are constructed
    Calibration::CoherencyMeasurementSet measurement_set;

    //! The reduced chi-squared after fitting
    double reduced_chisq;

    //! The fit debug flag
    bool fit_debug;

    //! Construtor helper
    void init ();

    Stokes< std::complex<double> > standard_variance;

  private:

    //! True when the regions have been set
    bool regions_set;

  };

}

#endif

