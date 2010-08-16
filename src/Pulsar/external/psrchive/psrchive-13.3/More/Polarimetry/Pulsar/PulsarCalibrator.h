//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/PulsarCalibrator.h,v $
   $Revision: 1.37 $
   $Date: 2009/10/02 03:37:57 $
   $Author: straten $ */

#ifndef __Pulsar_PulsarCalibrator_H
#define __Pulsar_PulsarCalibrator_H

#include "Pulsar/SystemCalibrator.h"
#include "Pulsar/PhaseWeight.h"

#include "MEAL/Mean.h"

#include "toa.h"

#include <stdio.h>

namespace Pulsar {

  class Archive;
  class Integration;
  class PolnProfileFit;
  class ReferenceCalibrator;

  //! Uses PolnProfileFit to determine the system response
  /*! The PulsarCalibrator implements a technique of polarimetric
    calibration using a well-determined source.  This class requires a
    polarimetric standard and another observation of the same source.  */
  class PulsarCalibrator : public SystemCalibrator {
    
  public:
    
    //! Constructor
    PulsarCalibrator (Calibrator::Type* = 0);

    //! Destructor
    ~PulsarCalibrator ();

    //! When set, pulse phase is removed from the model
    void set_fixed_phase (bool flag = true);

    //! Get the number of frequency channels
    unsigned get_nchan () const;

    //! Get the number of pulsar harmonics (input polarization states)
    unsigned get_nstate_pulsar () const;

    //! Set the maximum number of harmonics to include in fit
    void set_maximum_harmonic (unsigned max);

    //! Allow software to choose the maximum harmonic
    void set_choose_maximum_harmonic (bool flag = true);

    //! Get the number of harmonics in use
    unsigned get_nharmonic () const;

    //! Normalize each Stokes vector by the mean on-pulse invariant
    void set_normalize_by_invariant (bool flag = true);

    //! Set the standard to which pulsar profiles will be fit
    void set_standard (const Archive* data);

    //! Solve each sub-integration (instead of global fit)
    void set_solve_each (bool = true);

    //! Unload the solution from each sub-integration
    void set_unload_each (Unloader*);

    //! Set the solution to the mean
    void update_solution ();

    //! Return the transformation to be used for precalibration
    MEAL::Complex2* get_transformation (const Archive* data,
					unsigned isub, unsigned ichan);

    //! The matrix template matching engine used to fit the specified channel
    const PolnProfileFit* get_mtm (unsigned ichan) const;

    //! Set true to detect gimbal lock when rotations are not quaternion
    bool monitor_gimbal_lock;

  protected:
    
    friend class MatrixTemplateMatching;

    //! The template/standard
    Reference::To<Pulsar::Archive> standard;

    //! Solve the measurement equation for each sub-integration
    bool solve_each;

    //! Store the solution derived from each sub-integration
    typedef std::map< unsigned, Reference::Vector<MEAL::Complex2> > Storage;
    Storage store_each;

    //! Unload the solution derived from each sub-integration
    Reference::To<Unloader> unload_each;

    //! Return a pointer to a newly constructed/initialized transformation
    MEAL::Complex2* new_transformation (unsigned ichan);

    //! The calibration model as a function of frequency
    Reference::Vector<PolnProfileFit> mtm;

    //! The reduced chi-squared as a function of frequency
    std::vector<float> reduced_chisq;

    typedef MEAL::Mean< MEAL::Complex2 > MeanXform;

    //! The array of transformation Model instances
    Reference::Vector<MeanXform> solution;

    //! Flag set when phase should be held fixed
    bool fixed_phase;

    //! The maximum number of harmonics to include in the fit
    unsigned maximum_harmonic;

    //! When set, the software will choose the maximum harmonic
    bool choose_maximum_harmonic;

    //! The maximum harmonic chosen
    unsigned chosen_maximum_harmonic;

    //! Normalize the Stokes parameters by the invariant interval
    bool normalize_by_invariant;

    //! The on-pulse region
    PhaseWeight onpulse;

    //! The baseline region
    PhaseWeight baseline;

    //! Initialize the StandardModel of the specified channel
    void init_model (unsigned ichan);

    //! Solve the measurement equation for the given channel
    void solve1 (const Integration* data, unsigned ichan);

    //! Set things up for the model in the given channel
    void setup (const Integration* data, unsigned ichan);

    //! Prepare to export the solution in current state; e.g. for plotting
    virtual void export_prepare () const;

    //! Ensure that the pulsar observation can be added to the data set
    virtual void match (const Archive*);

    //! Add data from the specified sub-integration
    virtual void add_pulsar (const Archive* data, unsigned isub);

    //! Add data from the specified channel
    virtual void add_pulsar (Calibration::CoherencyMeasurementSet&,
			     const Integration*, unsigned ichan);

  private:

    // used to communicate between solve and add_observation
    unsigned big_difference;

    //! Build the arrays
    void build (unsigned nchan);

  };

}

#endif
